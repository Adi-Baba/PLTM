# Polylogarithmic Long-Term Memory (PLTM) SDK


> **Version:** 1.0.0
> **Architecture:** Hybrid (C Core / Python API)
> **License:** MIT

## Overview

PLTM (Polylogarithmic Long-Term Memory) is a high-performance memory pipeline designed to provide "Infinite Context" capabilities to AI models. It bridges advanced Geometric Singularity Analysis with efficient C-based embedded engineering.

The core engine uses Fast Fourier Transform (FFT) convolution to achieve $O(N \log N)$ complexity, making it significantly faster than traditional $O(N^2)$ attention mechanisms.

## Installation

You can install the PLTM SDK directly from GitHub using `pip`:

```bash
pip install git+https://github.com/Adi-Baba/PLTM.git
```

### Requirements
- Python 3.8+
- Systems: Windows (x64) or Linux (x64)

## Quick Start

### 1. Start the Memory Server

The SDK comes with a built-in FastAPI server to handle memory operations.

```bash
pltm-server
```
*Server will start on `http://0.0.0.0:8000`*

### 2. Python API Usage

You can also use the engine directly in your Python scripts:

```python
import numpy as np
from pltm import PLTM_Engine

# Initialize Engine (Context Size = 2048, Singularity Index s = 0.1)
engine = PLTM_Engine(context_size=2048, s=0.1)

# Generate dummy input data
input_data = np.random.rand(2048).astype(np.float32)

# Process data through the memory pipeline
output = engine.process(input_data)

print("Input Shape:", input_data.shape)
print("Output Shape:", output.shape)
print("Memory Summary:", output[-10:]) # View last 10 memory units
```

## Directory Structure

- **`pltm/`**: The core package.
    - **`polylog.dll`**: The compiled C engine (pre-built).
    - **`server.py`**: FastAPI server entry point.
- **`setup.py`**: Installation script.

## Architecture Details

1.  **C Core (`polylog.dll`)**: Handles the heavy lifting of FFT convolution and singularity math.
2.  **Python Interface**: Wraps the C core using `ctypes` for easy integration with modern AI stacks (PyTorch, TensorFlow, etc.).
3.  **API Layer**: Exposes memory operations via REST API for distributed systems.

## Performance

*   **C-Core Throughput**: **~15.52 Million Tokens/sec**
*   **Python API Throughput**: **~10.00 Million Tokens/sec**
*   **Latency**: ~0.06ms (Core) / ~0.20ms (Python API) per context window
*   **Speedup**: significantly faster than traditional attention.

---
*Built by the Aditya*
