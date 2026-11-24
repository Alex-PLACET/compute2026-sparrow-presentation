# The Sparrow ecosystem: Arrow in modern minimal C++20

## Compute!, November 2026

Slides online: https://alex-placet.github.io/compute2026-sparrow-presentation/slides/

## Build locally

First make sure you have Node.js installed.

Then build the slides with:

```bash
npx @marp-team/marp-cli@4.5.1 sparrow-presentation.md
```

## Interactive notebooks

Try them online: https://alex-placet.github.io/compute2026-sparrow-presentation/lab/index.html

- `notebooks/sparrow.ipynb` (C++20): sparrow arrays, Arrow C Data Interface, record batches
- `notebooks/sparrow-ipc.ipynb` (C++20 and Python, [polyglott kernel](https://github.com/DerThorsten/jupyterlite-polyglott)): Arrow IPC stream files written by sparrow-ipc and read by pyarrow, and the reverse
- `notebooks/sparrow-compute.ipynb` (C++20): lazy sparrow-compute expressions
- `notebooks/rockfinch.ipynb` (Python): `rockfinch_demo`, a C++ Python extension module built on sparrow-rockfinch

Build and serve the JupyterLite site locally:

```bash
micromamba create -n sparrow-jupyterlite -f jupyterlite_build_environment.yml
micromamba activate sparrow-jupyterlite
rattler-build build --recipe recipes/sparrow-compute --recipe rockfinch-demo --target-platform emscripten-wasm32 -c https://prefix.dev/emscripten-forge-4x -c conda-forge --output-dir conda-channel
jupyter lite build
python -m http.server -d dist 8000
```

[sparrow-compute](https://github.com/sparrow-org/sparrow-compute) is not packaged upstream yet: `recipes/sparrow-compute` packages its headers at commit `ce8d064`.
`rockfinch-demo` is a Python extension module written in C++ (nanobind, sparrow, sparrow-compute, [sparrow-rockfinch](https://github.com/sparrow-org/sparrow-rockfinch) 0.2.2, linked statically).
`rattler-build` builds both packages into the local `conda-channel/`, from which `environment-cpp.yml` and `environment-python.yml` install them in the kernels.
