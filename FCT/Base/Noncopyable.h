//
// Created by Administrator on 2025/5/9.
//

#ifndef FCT_NONCOPYABLE_H
#define FCT_NONCOPYABLE_H
namespace FCT {
    class Noncopyable {
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;
    public:
        Noncopyable(const Noncopyable&) = delete;
        void operator=(const Noncopyable&) = delete;
    };


}
#endif //FCT_NONCOPYABLE_H
