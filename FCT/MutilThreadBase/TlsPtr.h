//
// Created by Administrator on 2025/2/25.
//
#include "../ThirdParty.h"
#include "./RefCount.h"
#ifndef TLSPTR_H
#define TLSPTR_H
namespace FCT {
    template<typename T>
    class TlsPtr {
    public:
        TlsPtr(T *ptr) : m_ptr(ptr) {
#ifdef FCT_DEBUG_MODE
            m_id = std::this_thread::get_id();
#endif  // FCT_DEBUG_MODE
        }
        TlsPtr(const TlsPtr &other) : m_ptr(other.m_ptr) {
            safeAddRef(m_ptr);
        }
        ~TlsPtr() {
            safeRelease(m_ptr);
        }
        TlsPtr(TlsPtr &&other)  : m_ptr(other.m_ptr) {
#ifdef FCT_DEBUG_MODE
            m_id = std::move(other.m_id);
#endif  // FCT_DEBUG_MODE
            other.m_ptr = nullptr;
        }

        T *operator*() const {
#ifdef FCT_DEBUG_MODE
            if (std::this_thread::get_id()!= m_id) {
                //todo: error handling
            }
#endif  // FCT_DEBUG_MODE
            return m_ptr;
        }
        TlsPtr& operator=(TlsPtr&& other) {
            if (this != &other) {
                if (m_ptr) m_ptr->release();
                m_ptr = other.m_ptr;
#ifdef FCT_DEBUG_MODE
                m_id = std::move(other.m_id);
#endif  // FCT_DEBUG_MODE
                other.m_ptr = nullptr;
            }
            return *this;
        }

        T *get() const {
            return *this;
        }

        T *operator->() const {  // 添加 const 修饰符
#ifdef FCT_DEBUG_MODE
            if (std::this_thread::get_id() != m_id) {
                // TODO: error handling
            }
#endif  // FCT_DEBUG_MODE
            return m_ptr;  // 直接返回 m_ptr
        }

    private:
        T *m_ptr;
#ifdef FCT_DEBUG_MODE
        std::thread::id m_id;
#endif  // FCT_DEBUG_MODE
    };

#endif //TLSPTR_H
}
