#pragma once
#include "./Context.h"
namespace FCT {
	class IRenderTarget {
	public:
		virtual void bind(Context* ctx) = 0;
	private:

	};
}