#pragma once
namespace FCT {
    class Context;
	enum class RenderTargetType
	{
		WindowTarget,
		ImageTarget,
	};
	class Image;
	class IRenderTarget {
	public:
		virtual RenderTargetType getType() const = 0;
		virtual void bind(Context* ctx) = 0;
		virtual Image* getImage() const = 0;
		virtual std::vector<Image*> getTargetImages() = 0;
	private:

	};
}