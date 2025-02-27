#pragma once

#include <string>
#include <vector>

namespace FCT {

	class ImageLoader {
	public:
		struct ImageData {
			int width;
			int height;
			int channels;
			std::vector<unsigned char> data;
		};

		virtual ~ImageLoader() = default;

		virtual ImageData load(const std::string& filename) = 0;
		virtual bool save(const std::string& filename, const ImageData& imageData) = 0;
		virtual std::vector<std::string> getSupportedExtensions() const = 0;

	protected:
		ImageLoader() = default;
	};

} // namespace FCT