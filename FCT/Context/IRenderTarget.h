#pragma once
namespace FCT {
    class Context;
	class IRenderTarget {
	public:
		virtual void bind(Context* ctx) = 0;
	private:

	};
}