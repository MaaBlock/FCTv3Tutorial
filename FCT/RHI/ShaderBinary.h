//
// Created by Administrator on 2025/4/8.
//
#include "../ThirdParty.h"
#ifndef SHADERBINARY_H
#define SHADERBINARY_H

namespace FCT
{
    namespace RHI
    {
        class ShaderBinary {
        public:
            ShaderBinary();
            virtual ~ShaderBinary() {}
            void code(std::vector<char> code)
            {
                m_code = code;
                m_reflected = false;
            }
            std::vector<char> code() const
            {
                return m_code;
            }
            void location(std::map<std::string, uint32_t> locations)
            {
                m_locations = locations;
            }
            int32_t locationBySemantic(const std::string& semantic) const
            {
                auto it = m_locations.find(semantic);
                if (it != m_locations.end()) {
                    return it->second;
                }
                return -1;
            }
        protected:
            std::vector<char> m_code;
            std::map<std::string, uint32_t> m_locations;
            bool m_reflected = false;
        };
    }
}



#endif //SHADERBINARY_H
