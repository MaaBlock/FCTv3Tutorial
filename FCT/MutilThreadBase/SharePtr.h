//
// Created by Administrator on 2025/5/7.
//

#ifndef SHAREPTR_H
#define SHAREPTR_H
#include "./RefCount.h"
namespace FCT {
template<typename T>
    struct IsRefCountBase {
    private:
        static constexpr bool test(const RefCount*) { return true; }
        static constexpr bool test(...) { return false; }
    public:
        static constexpr bool ret = test(static_cast<T*>(nullptr));
    };
    template<typename T,bool isRefCountBase = IsRefCountBase<T>::ret>
    class RefCountPolicy {
    public:
        void addRef() {

        }
        void release() {

        }
    private:
        T* ptr;
    };
    template<typename T>
    class RefCountPolicy<T,false> {
    public:
        void addRef() {

        }
        void release() {

        }
    private:

    };
    template<typename T>
    class SharePtr{
    private:
        T* m_ptr;
        RefCountPolicy<T> m_policy;
    public:
        SharePtr(T* ptr) : m_ptr(ptr)
        {

        }
        ~SharePtr() {
            m_policy.release();
        }
        SharePtr(const SharePtr& other) : m_ptr(other.m_ptr), m_policy(other.m_policy)
        {
            m_policy.addRef();
        }
        SharePtr& operator=(const SharePtr& other)
        {
            if (this!= &other) {
                m_policy.release();
                m_ptr = other.m_ptr;
                m_policy = other.m_policy;
                m_policy.addRef();
            }
            return *this;
        }
        T& operator*() const {
            return *m_ptr;
        }
        T* operator->() const {
            return m_ptr;
        }
    };
    template<typename T>
    class WeakPtr
    {
    private:
        T* m_ptr;
        RefCountPolicy<T> m_policy;
    public:
        WeakPtr(const SharePtr<T>& other) : m_ptr(other.m_ptr), m_policy(other.m_policy)
        {

        }
        bool expired() const
        {

        }
        operator bool() const
        {

        }
    };
    /*
    template<template T,typename... Args>
    SharePtr<T> MakeShared(Args&&... args) {
        return SharePtr<T>(FCT_NEW(T,std::forward<Args>(args)...));
    }*/
}
#endif //SHAREPTR_H