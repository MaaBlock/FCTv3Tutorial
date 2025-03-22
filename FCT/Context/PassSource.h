//
// Created by Administrator on 2025/3/20.
//
#include "../ThirdParty.h"

#ifndef ANDROIDTUTORIAL_PASSSOURCE_H
#define ANDROIDTUTORIAL_PASSSOURCE_H

namespace FCT {
    struct PassSource {
        PassSource(const std::string& source) { //不能有多余的空格，为了效率，并没有额外处理空格
            size_t dotPos = source.find('.');
            if (dotPos != std::string::npos) {
                oper = source.substr(0, dotPos);
                name = source.substr(dotPos + 1);
            } else {
                name = source;
            }
        }

        PassSource(const std::string& oper, const std::string& name)
                : oper(oper), name(name) {}

        PassSource operator|(const PassSource& rhs) const {
            PassSource result = *this;
            result.alternatives.push_back(rhs);
            return result;
        }

        const std::string& getName() const { return name; }

        const std::string& getOperator() const { return oper; }

        const std::vector<PassSource>& getAlternatives() const { return alternatives; }

        bool hasAlternatives() const { return !alternatives.empty(); }

        std::string getFullIdentifier() const {
            return oper.empty() ? name : (oper + "." + name);
        }

        std::string toString() const {
            std::string result = getFullIdentifier();
            for (const auto& alt : alternatives) {
                result += " | " + alt.getFullIdentifier();
            }
            return result;
        }

        std::string name;
        std::string oper;
        std::vector<PassSource> alternatives;
    };
}

#endif //ANDROIDTUTORIAL_PASSSOURCE_H