
import numpy as np
import os
import sys

# Ensure we import local pltm if running from repo root
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
try:
    from pltm import PLTM_Engine
except ImportError:
    # If installed via pip
    from pltm import PLTM_Engine

def demo_retention():
    print("=== PLTM Memory Retention Demo ===")
    
    N = 1024
    s = 0.5
    # Initialize with unity gain (normalization) and slight damping
    engine = PLTM_Engine(context_size=N, s=s, gain=1.0, damping=0.99)
    
    print(f"Engine Initialized: N={N}, s={s}, gain=1.0, damping=0.99")
    
    # 1. Feed a constant signal
    # Without normalization, this would explode.
    print("\n[Test 1] Stability Check (Constant Input)")
    chunk = np.ones(N, dtype=np.float32)
    
    for i in range(5):
        out = engine.process(chunk)
        print(f"  Chunk {i+1}: Mean Output = {np.mean(out):.4f} (Expected ~1.0)")
        
    # 2. Reset and Demonstrate Impulse Decay
    print("\n[Test 2] Impulse Response & Damping")
    engine.reset()
    print("  Engine Reset.")
    
    impulse = np.zeros(N, dtype=np.float32)
    impulse[0] = 10.0 # Spark
    
    out1 = engine.process(impulse)
    print(f"  Impulse Chunk: Max Out = {np.max(out1):.4f}")
    
    # Feed silence to see the tail
    silence = np.zeros(N, dtype=np.float32)
    for i in range(3):
        out = engine.process(silence)
        print(f"  Silence Chunk {i+1} (Tail): Max Out = {np.max(out):.4f} (Decaying)")

if __name__ == "__main__":
    demo_retention()
