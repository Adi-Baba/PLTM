from setuptools import setup, find_packages, Extension
import os

# Define the C-Extension
# This ensures pip compiles the code on Linux/Mac during install
polylog_module = Extension(
    'pltm.polylog',
    sources=[
        'core/polylog.c',
        'core/kiss_fft/kiss_fft.c'
    ],
    include_dirs=['core/kiss_fft'],
    # extra_compile_args=['-O3'] # Optional optimization
)

setup(
    name="pltm",
    version="1.2.0",
    description="Polylogarithmic Long-Term Memory Pipeline",
    packages=['pltm'],
    # We include the extension module
    ext_modules=[polylog_module],
    include_package_data=True,
    package_data={
        'pltm': ['*.dll', '*.so', '*.pyd'], # Include pre-built items if present
    },
    install_requires=[
        "numpy",
        "scipy",
        "fastapi",
        "uvicorn",
        "google-generativeai"
    ],
    entry_points={
        'console_scripts': [
            'pltm-server=pltm.server:start',
        ],
    },
)
