#pragma once
#include "../RHI/IPipelineResource.h"
#include "../ImageLoader/ImageLoader.h"
namespace FCT {
	class Texture : public IPipelineResource {
	public:
		enum class Format {
			R32F,
			RG32F,
			RGB32F,
			RGBA32F,
			RGB8,
			RGBA8
		};

		virtual ~Texture() = default;

		PipelineResourceType getType() const override {
			return PipelineResourceType::Texture;
		}

		virtual void create(unsigned int width, unsigned int height, Format format) = 0;
		virtual void setData(const void* data, size_t size) = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual Format getFormat() const = 0;
		virtual void bind() override = 0;
		virtual void unbind() override = 0;

		virtual void setSlot(unsigned int slot) = 0;
		virtual unsigned int getSlot() const = 0;
		bool loadFromFile(const std::string& filename, ImageLoader* imageLoader);
	};
}