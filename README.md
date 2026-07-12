# metaimg

A command-line image metadata viewer that reads basic metadata from common image file formats.

> [!NOTE]
> This project is a personal learning exercise for **C++23** features and modern C++ practices.

## Supported Metadata

| Format | Dimensions | Bit Depth | Color Type |
|--------|:----------:|:---------:|:----------:|
| PNG    | ✅         | ✅        | ✅         |
| BMP    | ✅         | ✅        | ✅         |
| JPEG   | ✅         | ✅        | ❌         |
| GIF    | ✅         | ✅        | ❌         |
| AVIF   | ❌         | ❌        | ❌         |

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Run

```bash
# Human-readable
./build/metaimg samples/lemon.png

# JSON output
./build/metaimg --json samples/lemon.png samples/lotus.jpg
```

## Test

```bash
cmake --build build && ctest --test-dir build
```
