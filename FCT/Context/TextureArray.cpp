#include "../headers.h"

void FCT::TextureArray::loadFromFile(const std::vector<std::string>& filepaths, ImageLoader* imageLoader) {
    if (filepaths.size() != getLayers()) {
        std::cerr << "Error: Number of images does not match the array size." << std::endl;
        return;
    }

    for (size_t i = 0; i < filepaths.size(); ++i) {
        auto imageData = imageLoader->load(filepaths[i]);
        if (imageData.data.empty()) {
            std::cerr << "Error: Failed to load image: " << filepaths[i] << std::endl;
        }

        if (imageData.width != getWidth() || imageData.height != getHeight()) {
            std::cerr << "Error: Image dimensions do not match the expected size." << std::endl;
        }

        setData(i, imageData.data.data(), imageData.data.size());
    }
    GL_Check("FCT::TextureArray::loadFromFile");
}
