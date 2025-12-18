
# Polylogarithmic Long-Term Memory (PLTM)

<div align="center">

![Version](https://img.shields.io/badge/version-1.1.0-blue.svg?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green.svg?style=flat-square)
![Platform](https://img.shields.io/badge/platform-win--x64%20%7C%20linux--x64-lightgrey.svg?style=flat-square)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg?style=flat-square)

**"Infinite Context" Memory for High-Performance AI**

*Bridge the gap between Geometric Singularity Analysis and Embedded C Engineering.*

</div>

---

## 🚀 Overview

**PLTM** (Polylogarithmic Long-Term Memory) is a specialized memory pipeline that replaces the quadratic $O(N^2)$ scaling of traditional attention mechanisms with a hyper-efficient **$O(N \log N)$** approach.

By utilizing **Overlap-Add FFT Convolution** in a highly optimized C-Core, PLTM treats context as a continuous signal rather than discrete chunks—enabling true **infinite context** retention with minimal latency.

### 🌟 Key Features (v1.1)

*   **⚡ Blazing Fast**: Process **~8.7 Million Tokens/sec** on standard CPUs.
*   **♾️ True Infinite Context**: Implements an **Overlap-Add** engine that carries signal "tails" across context windows, ensuring memory never abruptly cuts off.
*   **🧠 Causal Core**: Built on a time-domain causal kernel ($h[t] \sim t^{-s}$) to guarantee mathematically valid memory retention.
*   **🛡️ Production Ready**: Hybrid architecture (C++ Core + Python Bindings) with robust error handling and zero-copy memory operations.

---

## 📊 Performance Benchmarks

| Metric | PLTM (v1.1) | Traditional Attention |
| :--- | :--- | :--- |
| **Complexity** | **$O(N \log N)$** | $O(N^2)$ |
| **Throughput** | **~8.72 M Tokens/s** | ~0.05 M Tokens/s (CPU) |
| **Values** | **Exact (FP32)** | Approx / Quantized |
| **Latency** | **~0.23 ms** / window | >50 ms / window |

*> Benchmarks run on standard x64 hardware with N=2048 context size.*

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

Integrate infinite memory into your pipeline in just 3 lines of code:

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

---

<div align="center">
Built by Aditya
</div>
