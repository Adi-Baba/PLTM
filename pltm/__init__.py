import ctypes
import os
import glob
import sys
import platform

# 1. Load the DLL / Shared Object
# Logic:
# 1. Try to find the setuptools-compiled extension (polylog*.so/pyd) in the current package dir.
# 2. Fallback to pre-compiled binaries if present (polylog.dll).

curr_dir = os.path.dirname(os.path.abspath(__file__))

# Patterns to search for
# Windows: .pyd (from setup.py) or .dll (pre-packaged)
# Linux/Mac: .so (from setup.py or pre-packaged)
patterns = [
    "polylog*.pyd",
    "polylog*.so",
    "polylog.dll"
]

dll_path = None

for pat in patterns:
    # Look in current directory (where pip installs it)
    matches = glob.glob(os.path.join(curr_dir, pat))
    if matches:
        dll_path = matches[0] # Pick the first match
        break

# Fallback Search Paths (Dev/Repo structure)
if not dll_path:
    possible_paths = [
        os.path.join(curr_dir, "../core/polylog.dll"),
        os.path.join(curr_dir, "../bin/polylog.dll"),
        os.path.join(curr_dir, "polylog.dll")
    ]
    for p in possible_paths:
        if os.path.exists(p):
            dll_path = os.path.abspath(p)
            break

if not dll_path:
    # Final check: Maybe it's installed as a system lib or in site-packages root?
    # Unlikely for this package structure.
    raise FileNotFoundError(f"Could not find compiled core (polylog). Checked in {curr_dir} with patterns {patterns}")

lib = ctypes.CDLL(dll_path)

# 2. Define C Types
class PolylogContext(ctypes.Structure):
    pass # Opaque pointer

# PolylogContext* ply_create_context(int N, float s);
lib.ply_create_context.argtypes = [ctypes.c_int, ctypes.c_float]
lib.ply_create_context.restype = ctypes.POINTER(PolylogContext)

# void ply_process(PolylogContext* ctx, const float* input, float* output);
lib.ply_process.argtypes = [ctypes.POINTER(PolylogContext), 
                           ctypes.POINTER(ctypes.c_float), 
                           ctypes.POINTER(ctypes.c_float)]

# void ply_destroy(PolylogContext* ctx);
lib.ply_destroy.argtypes = [ctypes.POINTER(PolylogContext)]

# 3. High-Level Python Wrapper
class PLTM_Engine:
    def __init__(self, context_size, s):
        self.N = context_size
        self.ctx = lib.ply_create_context(context_size, s)
        
    def process(self, input_array):
        # Ensure input is float32
        input_np = np.array(input_array, dtype=np.float32)
        
        # PAD to Context Size (N) if too short
        if len(input_np) < self.N:
            padded = np.zeros(self.N, dtype=np.float32)
            padded[:len(input_np)] = input_np
            input_data = padded
        else:
            input_data = input_np[:self.N] # Truncate if too long
            
        output_data = np.zeros_like(input_data, dtype=np.float32)
        
        # Pointers
        in_ptr = input_data.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
        out_ptr = output_data.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
        
        # Run C Core
        lib.ply_process(self.ctx, in_ptr, out_ptr)
        
        return output_data
        
    def __del__(self):
        if hasattr(self, 'ctx') and self.ctx:
            lib.ply_destroy(self.ctx)
