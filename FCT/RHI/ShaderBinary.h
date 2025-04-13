//
// Created by Administrator on 2025/4/8.
//
#include "../ThirdParty.h"
#include "../Context/Uniform.h"
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
            void uniformLocation(std::map<std::string,std::pair<uint32_t,uint32_t>> uniformLocations)
            {
                m_uniformLocations = uniformLocations;
            }
            void constBufferLocation(std::map<std::string,std::pair<uint32_t,uint32_t>> constBufferLocations)
            {
                m_constBufferLocations = constBufferLocations;
            }
            std::map<std::string,std::pair<uint32_t,uint32_t>> constBufferLocations()
            {
                return m_constBufferLocations;
            }
            std::map<std::string,UniformLayout> constBufferLayout()
            {
                return m_constBufferLayout;
            }
            void addConstBufferLocation(UniformLayout layout, uint32_t set, uint32_t location)
            {
                m_constBufferLayout[layout.getName()] = layout;
                m_constBufferLocations[layout.getName()] = std::make_pair(set, location);
            }
        protected:
            std::vector<char> m_code;
            std::map<std::string, uint32_t> m_locations;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_uniformLocations;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_constBufferLocations;
            std::map<std::string,UniformLayout> m_constBufferLayout;
            bool m_reflected = false;
        };
    }
}



#endif //SHADERBINARY_H
