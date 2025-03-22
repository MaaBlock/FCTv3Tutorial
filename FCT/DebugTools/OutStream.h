//
// Created by Administrator on 2025/3/1.
//
#include "../ThirdParty.h"
#ifdef FCT_ANDROID
#include "./Android_Out.h"
#endif
#ifndef FCT_OUTSTREAM_H
#define FCT_OUTSTREAM_H

namespace FCT {
#ifdef FCT_ANDROID
#include "Android_Out.h"
    extern std::ostream& fout;
    extern std::ostream& ferr;
#else
    #include <iostream>
    extern std::ostream& fout;
    extern std::ostream& ferr;
#endif

}  // namespace FCT

#endif //FCT_OUTSTREAM_H