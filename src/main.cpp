#include "file.h"
#include "image.h"

#include <print>
#include <cstdlib>

consteval auto version() -> std::string_view {
    return "0.1.0";
}


auto main(int argc, char* argv[]) -> int {
    if (argc < 2) {
        std::println("metaimg v{} - Image metadata viewer", version());
        std::println("Usage: metaimg <image-file...>");
        return EXIT_FAILURE;
    }

    int exit_code = 0;

    for (int i = 1; i < argc; ++i) {
        std::string_view file = argv[i];
        std::println("File: {}", file);

        // Read image file and retrieve bytes.
        auto bytes = read_file(file);
        if (!bytes) {
            exit_code = EXIT_FAILURE;
            continue;
        };

        // Display size of image file and first couple of bytes.
        auto view = std::span{*bytes};

        auto ret = get_image_metadata(view);
        if (!ret) {
            exit_code = EXIT_FAILURE;
            continue;
        };

        auto metadata = ret.value();

        std::println("Format: {}", image_format_to_string(metadata.format));
        std::println("Size: {} bytes", bytes->size());
        std::println("Width: {}", metadata.dimensions.width);
        std::println("Height: {}", metadata.dimensions.height);
        std::println("Bit Depth: {}", metadata.bit_depth);
        std::println("Color Type: {}", color_type_to_string(metadata.color_type));

        if (argc > i + 1) {
            std::println("---");
        }
    }
    
    return exit_code;
}