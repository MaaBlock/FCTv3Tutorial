#include "../ThirdParty.h"
#ifndef ANDROID_RUNTIME_H
#define ANDROID_RUNTIME_H

class GameActivityClass;
class InsetsClass;
class WindowInsetsCompatTypeClass;
class GameActivityObecct;

namespace FCT {
    class Android_Window;
}
int main();
namespace FCT {
    class Android_Runtime {
    public:
        Android_Runtime(){
            m_userInited = false;
            m_userRunning = false;
        }
        std::map<std::string, ARect> insets;

        void init(JNIEnv *env, jobject gameActivityObject);

        JavaVM *getVm() const;

        void term();

        GameActivityClass *getGameActivityClass() const;

        InsetsClass *getInsetsClass() const;

        WindowInsetsCompatTypeClass *getWindowInsetsCompatTypeClass() const;

        GameActivityObecct *getGameActivityObject() const;

        FCT::Android_Window *getWindow() const;

        std::thread* getUserThread() const;

        bool isUserRunning() const{
            return m_userRunning;
        }
        void createUserThread() {
            m_userThread = new std::thread([this]() {
                m_userInited = true;
                m_userRunning = true;
                main();
                m_userRunning = false;
            });
            m_userThread->detach();
            while (!m_userInited){
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    private:
        GameActivityClass *m_gameActivityClass;
        InsetsClass *m_insetsClass;
        WindowInsetsCompatTypeClass *m_windowInsetsCompatTypeClass;
        JavaVM *m_vm;
        GameActivityObecct *m_gameActivityObject;
        FCT::Android_Window *m_window;
        std::thread* m_userThread;
        bool m_userRunning;
        bool m_userInited;
    };

    extern Android_Runtime g_AndroidRuntime;
}
#endif //ANDROID_RUNTIME_H
