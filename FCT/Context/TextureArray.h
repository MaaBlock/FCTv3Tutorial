#pragma once
#include "IPipelineResource.h"
#include "../ImageLoader/ImageLoader.h"
namespace FCT {
	class TextureArray : public IPipelineResource {
	public:
		enum class Format {
			R32F,
			RG32F,
			RGB32F,
			RGBA32F,
			RGB8,
			RGBA8
		};

		PipelineResourceType getType() const override {
			return PipelineResourceType::TextureArray;
		}
		virtual ~TextureArray() = default;
		virtual void create(unsigned int width, unsigned int height, unsigned int layers, Format format) = 0;
		virtual void setData(unsigned int layer, const void* data, size_t size) = 0;
		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual unsigned int getLayers() const = 0;
		virtual Format getFormat() const = 0;
		virtual void bind() override = 0;
		virtual void unbind() override = 0;

		virtual void setSlot(unsigned int slot) = 0;
		virtual unsigned int getSlot() const = 0;
		void loadFromFile(const std::vector<std::string>& filepaths, ImageLoader* imageLoader);
	};
}