//
// Created by Administrator on 2025/4/8.
//
#include "../ThirdParty.h"
#include "../Context/Uniform.h"
#include "../Context/ResourceLayout.h"
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
            std::map<std::string,std::pair<uint32_t,uint32_t>> constBufferLocations()
            {
                return m_constBufferLocations;
            }
            std::map<std::string,ConstLayout> constBufferLayout()
            {
                return m_constBufferLayout;
            }
            void addConstBufferLocation(ConstLayout layout, uint32_t set, uint32_t location)
            {
                m_constBufferLayout[layout.getName()] = layout;
                m_constBufferLocations[layout.getName()] = std::make_pair(set, location);
            }

            void addTextureLocation(TextureElement texture, uint32_t set, uint32_t binding)
            {
                m_textureLayout[texture.getName()] = texture;
                m_textureLocations[texture.getName()] = std::make_pair(set, binding);
            }

            std::pair<uint32_t, uint32_t> getTextureLocation(const std::string& name) const
            {
                auto it = m_textureLocations.find(name);
                if (it != m_textureLocations.end()) {
                    return it->second;
                }
                return std::make_pair(0, 0);
            }

            TextureElement getTextureElement(const std::string& name) const
            {
                auto it = m_textureLayout.find(name);
                if (it != m_textureLayout.end()) {
                    return it->second;
                }
                return TextureElement();
            }

            std::map<std::string, std::pair<uint32_t, uint32_t>> textureLocations() const
            {
                return m_textureLocations;
            }

            std::map<std::string, TextureElement> textureLayout() const
            {
                return m_textureLayout;
            }

            void addSamplerLocation(SamplerElement sampler, uint32_t set, uint32_t binding)
            {
                m_samplerLayout[sampler.getName()] = sampler;
                m_samplerLocations[sampler.getName()] = std::make_pair(set, binding);
            }

            std::pair<uint32_t, uint32_t> getSamplerLocation(const std::string& name) const
            {
                auto it = m_samplerLocations.find(name);
                if (it != m_samplerLocations.end()) {
                    return it->second;
                }
                return std::make_pair(0, 0);
            }

            SamplerElement getSamplerElement(const std::string& name) const
            {
                auto it = m_samplerLayout.find(name);
                if (it != m_samplerLayout.end()) {
                    return it->second;
                }
                return SamplerElement();
            }

            std::map<std::string, std::pair<uint32_t, uint32_t>> samplerLocations() const
            {
                return m_samplerLocations;
            }

            std::map<std::string, SamplerElement> samplerLayout() const
            {
                return m_samplerLayout;
            }

        protected:
            std::vector<char> m_code;
            std::map<std::string, uint32_t> m_locations;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_uniformLocations;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_constBufferLocations;
            std::map<std::string,ConstLayout> m_constBufferLayout;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_textureLocations;
            std::map<std::string,TextureElement> m_textureLayout;
            std::map<std::string,std::pair<uint32_t,uint32_t>> m_samplerLocations;
            std::map<std::string,SamplerElement> m_samplerLayout;
            bool m_reflected = false;
        };
    }
}



#endif //SHADERBINARY_H
