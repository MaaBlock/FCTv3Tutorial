//
// Created by Administrator on 2025/3/16.
//

#ifndef FCT_TOOLDEFINE_H
#define FCT_TOOLDEFINE_H
#define FCT_SAFE_RELEASE(p) if (p) { p->release(); p = nullptr;  }
#define FCT_SAFE_ADDREF(p) if (p) { p->addRef(); }
#define FCT_WAIT_FOR(condition) do { while(!condition) { std::this_thread::sleep_for(std::chrono::milliseconds(1));} condition = false; } while(0)
#endif //FCT_TOOLDEFINE_H
