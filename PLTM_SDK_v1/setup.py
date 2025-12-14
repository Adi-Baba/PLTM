from setuptools import setup, find_packages

setup(
    name="pltm",
    version="1.0.0",
    description="Polylogarithmic Long-Term Memory Pipeline",
    packages=['pltm'],
    include_package_data=True,
    package_data={
        'pltm': ['*.dll', '*.so'],
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
