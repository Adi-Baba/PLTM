
import ctypes
import os
import numpy as np
import platform

# 1. Load the DLL
# Determine extension (.dll for Windows, .so for Linux)
# Determine extension (.dll for Windows, .so for Linux)
ext = ".dll" if platform.system() == "Windows" else ".so"

# Search Paths:
# 1. Dev Mode: ../core/polylog.dll
# 2. Prod Mode (SDK): ../bin/polylog.dll
# 3. Local Mode: ./polylog.dll
possible_paths = [
    os.path.join(os.path.dirname(__file__), "../core/polylog" + ext),
    os.path.join(os.path.dirname(__file__), "../bin/polylog" + ext),
    os.path.join(os.path.dirname(__file__), "polylog" + ext)
]

dll_path = None
for p in possible_paths:
    if os.path.exists(p):
        dll_path = os.path.abspath(p)
        break

if not dll_path:
    raise FileNotFoundError(f"Could not find compiled core. Checked: {[os.path.abspath(p) for p in possible_paths]}")

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
