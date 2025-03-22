//
// Created by Administrator on 2025/3/7.
//
#include "../ThirdParty.h"
#ifndef FCT_ACTIVITYWRAPPER_H
#define FCT_ACTIVITYWRAPPER_H
static constexpr jni::Class FCTWindowJavaClass {
        "com/example/androidtutorial/FCTWindow",
        jni::Method{"getNativeWindowPtr", jni::Return<jlong>{}},
        //jni::Method{"getDecorView", jni::Return<jobject>{}},
        //jni::Method{"setFlags", jni::Return<void>{}, jni::Params<jint, jint>{}}
};
static constexpr jni::Class FCTMainActivityJavaClass {
        "com/example/androidtutorial/FCTMainActivity",
        jni::Method{"getDeviceWidth",jni::Return<jint>{}},
        jni::Method{"getDeviceHeight",jni::Return<jint>{}},
        jni::Method{"createWindow", jni::Return{FCTWindowJavaClass}, jni::Params<jlong,jint, jint, jint, jint>{}},
        };




#endif //FCT_ACTIVITYWRAPPER_H
