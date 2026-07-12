# metaimg

A command-line image metadata viewer that reads basic metadata from common image file formats.

## Supported Metadata

| Format | Dimensions | Bit Depth | Color Type |
|--------|:----------:|:---------:|:----------:|
| PNG    | ✅         | ✅        | ✅         |
| BMP    | ✅         | ✅        | ✅         |
| JPEG   | ✅         | ✅        | ❌         |
| GIF    | ✅         | ✅        | ❌         |
| AVIF   | ❌         | ❌        | ❌         |

> [!NOTE]
> This project is a personal learning exercise for **C++23** features and modern C++ practices.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Run

```bash
./build/metaimg samples/lemon.png samples/lotus.jpg
```

## Test

```bash
cmake --build build && ctest --test-dir build
```
