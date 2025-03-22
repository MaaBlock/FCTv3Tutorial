//
// Created by Administrator on 2025/3/7.
//
#include "../ThirdParty.h"
#include "./ActivityWrapper.h"
#include "../UI/Android_UICommon.h"
#ifndef FCT_ANDROID_RUNTIME_H
#define FCT_ANDROID_RUNTIME_H
namespace FCT {

    class Android_Runtime {
    public:
        void init(JNIEnv *env, jobject activity);

        void entry();

        void cleanUp();

        void term();

        int getDeviceHeight();

        int getDeviceWidth();

        auto getUICommon() const {
            return m_uiCommon;
        }
        auto getActivity() const {
            return m_activity;
        }
        auto getVm() const {
            return m_vm;
        }
        auto getEnv() const {
            return jni::JniEnv::GetEnv();
        }
        /*
        void test(){

            //(*m_activity)("createWindow",jint(0),jint(0),jint(100),jint(100));
        }*/
    private:
        jni::GlobalObject<FCTMainActivityJavaClass> *m_activity;
        std::thread *m_userThread;
        jni::JvmRef<jni::kDefaultJvm> *m_vm;
        Android_UICommon* m_uiCommon;
    };

    extern Android_Runtime *g_androidRuntime;
}

#endif //FCT_ANDROID_RUNTIME_H
