//
// Created by Administrator on 2025/3/16.
//
#include "../ThirdParty.h"
#ifndef FCT_COMPUTATION_H
#define FCT_COMPUTATION_H
template<typename T>
class Computation {
private:
    std::future<T> m_future;
    T m_value;
    bool m_isFuture;
public:
    Computation(std::future<T>&& future) {
        m_future = future;
        m_isFuture = true;
    }
    Computation(T value) {
        m_value = value;
        m_isFuture = false;
    }
    T get() {
        if (m_isFuture) {
            return m_future;
        }
        return m_value;
    }
};
#endif //FCT_COMPUTATION_H
