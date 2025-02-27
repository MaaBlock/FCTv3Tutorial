//
// Created by Administrator on 2025/2/25.
//
#include "../ThirdParty.h"
#ifndef TLSPTR_H
#define TLSPTR_H
template<typename T>
class TlsPtr {
public:
    TlsPtr(T* ptr) : m_ptr(ptr) {
#ifdef FCT_DEBUG_MODE
        m_id = std::this_thread::get_id();
#endif  // FCT_DEBUG_MODE
    }
    ~TlsPtr() {
        m_ptr->release();
    }
    T* operator*() const {
#ifdef FCT_DEBUG_MODE
        if (std::this_thread::get_id()!= m_id) {
            //todo: error handling
        }
#endif  // FCT_DEBUG_MODE
        return m_ptr;
    }
    T* get() const {
        return *this;
    }
    T* operator->() const {
        return *this;
    }

private:
    T* m_ptr;
#ifdef FCT_DEBUG_MODE
    std::thread::id m_id;
#endif  // FCT_DEBUG_MODE
};

#endif //TLSPTR_H
