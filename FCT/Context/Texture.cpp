#include "../headers.h"
void GL_Check(const char* describe);
bool FCT::Texture::loadFromFile(const std::string& filename, ImageLoader* imageLoader)
{
	ImageLoader::ImageData imageData = imageLoader->load(filename);
	if (imageData.data.empty()) {
		return false;
	}

	Format format;
	switch (imageData.channels) {
	case 3: format = Format::RGB8; break;
	case 4: format = Format::RGBA8; break;
	default: return false;
	}

	GL_Check("before create");
	create(imageData.width, imageData.height, format);
	GL_Check("before setData");
	setData(imageData.data.data(), imageData.data.size());
	GL_Check("after setData");

	return true;
}
