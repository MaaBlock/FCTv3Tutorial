//
// Created by Administrator on 2025/4/11.
//

#ifndef STRING_H
#define STRING_H
namespace FCT {
    constexpr bool StringEquals(const char* a, const char* b) noexcept {
        if (a == b) return true;
        if (!a || !b) return false;

        while (*a && *b) {
            if (*a != *b) return false;
            ++a;
            ++b;
        }

        return *a == *b;
    }

}
#endif //STRING_H
