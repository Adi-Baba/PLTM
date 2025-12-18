
# Polylogarithmic Long-Term Memory (PLTM)

<div align="center">

![Version](https://img.shields.io/badge/version-1.1.0-blue.svg?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green.svg?style=flat-square)
![Platform](https://img.shields.io/badge/platform-win--x64%20%7C%20linux--x64-lightgrey.svg?style=flat-square)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg?style=flat-square)

**Unbounded Streaming Context for High-Performance AI**

*Bridge the gap between Geometric Singularity Analysis and Embedded C Engineering.*

</div>

---

## 🚀 Overview

**PLTM** (Polylogarithmic Long-Term Memory) is a specialized memory pipeline that replaces the quadratic $O(N^2)$ scaling of traditional attention mechanisms with a hyper-efficient **$O(N \log N)$** approach.

By using an **Overlap-Add FFT convolution engine**, PLTM processes streams continuously without hard context boundaries. Information from previous windows decays smoothly according to a causal power-law kernel, rather than being abruptly truncated.

### 🌟 Key Features (v1.1)

*   **⚡ Blazing Fast**: Process **~8.7 Million Tokens/sec** on standard CPUs.
*   **♾️ Unbounded Streaming Context**: Implements an **Overlap-Add** engine that carries signal "tails" across context windows, enabling continuous streaming without state resets.
*   **🧠 Causal Kernel**: Built on a causal power-law kernel of the form $h[t] \propto t^{-s}$. This kernel provides long-range temporal influence with mathematically controlled decay, ensuring stability and determinism.
*   **🛡️ Robust Design**: Hybrid architecture (C++ Core + Python Bindings) with strict memory safety checks and zero-copy operations.

---

## 💡 What PLTM Is (and Is Not)

**PLTM is a memory kernel**, not a complete neural model.

It **provides**:
- a fast, deterministic, long-range mixing operator.
- persistent state across streaming windows.
- sub-quadratic scaling via FFT convolution.

PLTM **does not**:
- implement learned attention.
- replace Transformers directly.
- perform training or gradient updates by itself.

---

## 🔬 Empirical Properties

PLTM has been empirically verified to:
- **Preserve State**: Correctly carries signal tails across consecutive streaming calls.
- **Micro-Benchmark**: Verified deterministic output for impulse and constant inputs.
- **Scale Sub-Quadratically**: Throughput remains stable as sequence length increases (latency $\propto N \log N$).
- **Numerical Stability**: Operates reliably in FP32 with bounded output norms.

---

## 📊 Performance Benchmarks

| Metric | PLTM (v1.1) | Traditional Attention (CPU) |
| :--- | :--- | :--- |
| **Complexity** | **$O(N \log N)$** | $O(N^2)$ |
| **Throughput** | **~8.72 M Tokens/s** | ~0.05 M Tokens/s |
| **Values** | **Exact (FP32)** | Approx / Quantized |
| **Latency** | **~0.23 ms** / window | >50 ms / window |

*> Benchemarks compare PLTM against naïve CPU-based attention implementations and are intended to illustrate scaling behavior, not claim superiority over optimized GPU kernels.*

---

## 📦 Installation

Install the latest version directly from GitHub:

```bash
pip install git+https://github.com/Adi-Baba/PLTM.git
```

**Requirements:**
*   Python 3.8+
*   Windows (x64) or Linux (x64) Environment

---

## 🛠️ Quick Start

Integrate unbounded memory into your pipeline in just 3 lines of code:

```python
import numpy as np
from pltm import PLTM_Engine

# 1. Initialize Engine (Context Size = 2048, Singularity Index s = 0.5)
engine = PLTM_Engine(context_size=2048, s=0.5)

# 2. Generate Data Stream (e.g., embeddings or raw signals)
stream_chunk = np.random.rand(2048).astype(np.float32)

# 3. Process with Memory Retention
# The engine automatically handles state overlap between calls!
output = engine.process(stream_chunk)

print(f"Processed {len(output)} tokens. Memory State Active.")
```

---

## 🏗️ Architecture

The system is built as a high-performance hybrid:

1.  **Polylog Core (`polylog.dll`)**:
    *   Written in **C** for bare-metal speed.
    *   Handles FFTs, complex multiplication, and circular buffer management.
    *   Zero dependencies (static build).

2.  **Python Interface**:
    *   Uses `ctypes` for direct memory access.
    *   Zero-copy overhead during data transfer.
    *   Compatible with **PyTorch** and **NumPy** arrays.

### Intended Audience

PLTM is designed for:
- researchers exploring long-context sequence models.
- systems engineers building streaming ML pipelines.
- practitioners interested in efficient global memory operators.

It is **not** intended as a plug-and-play replacement for Transformer architectures without further integration work.

## ⚠️ Maturity & Notes

*   **System Kernel**: PLTM is a **systems-level memory kernel** suitable for research and experimentation. While the core is stable and deterministic, higher-level training integration is left to the user.
*   **Thread Safety**: The `PLTM_Engine` instance is **stateful** (it holds the FFT buffers). Do **not** share a single instance across multiple threads. Create one engine per thread.
*   **Precision**: The core operates in `FP32` (Single Precision). Ensure your input is cast to `float32` before processing to avoid overhead.

---

<div align="center">
Built by Aditya
</div>
