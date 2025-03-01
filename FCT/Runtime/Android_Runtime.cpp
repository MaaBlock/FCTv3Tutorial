//
// Created by Administrator on 2025/2/27.
//

#include "../FCTAPI.h"
#include "runtime.h"

namespace FCT {
    struct Android_RuntimeCommon : public RuntimeCommon {
        Android_UICommon* androidUICommon;

    };
#define RuntimeBegin()  \
    Android_RuntimeCommon* common = (Android_RuntimeCommon*)m_commmon;

    void Runtime::init() {
        m_commmon = new Android_RuntimeCommon();
        RuntimeBegin();
        common->androidUICommon = &g_AndroidUiCommon;
    }
    void Runtime::term() {
        delete m_commmon;
        m_commmon = nullptr;
    }
    Window *Runtime::createWindow(int w, int h, const char *title) {
        RuntimeBegin();
        return common->androidUICommon->createWindow(w,h,title);
    }
}