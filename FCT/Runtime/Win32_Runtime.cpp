#include "../FCTAPI.h"

namespace FCT {
	struct Win32_RuntimeCommon : public RuntimeCommon {
	
	};
	void Runtime::init() {
		g_common = new Win32_RuntimeCommon();
	}
	void Runtime::term() {
	    delete g_common;
	}

}