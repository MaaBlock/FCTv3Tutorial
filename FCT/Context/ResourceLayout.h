//
// Created by Administrator on 2025/4/16.
//
#include "../ThirdParty.h"
#include "DataTypes.h"
#include "../Base/Flags.h"
#include "../Base/string.h"
#include "Uniform.h"
#ifndef RESOURCELAYOUT_H
#define RESOURCELAYOUT_H

namespace FCT
{
    enum class TextureType {
        Texture2D,
        Texture3D,
        TextureCube,
        Texture2DArray,
        DepthTexture
    };

    inline constexpr const char* GetTextureDefaultName(TextureType type)
    {
        switch (type)
        {
        case TextureType::Texture2D:
            return "u_Texture2D";
        case TextureType::Texture3D:
            return "u_Texture3D";
        case TextureType::TextureCube:
            return "u_TextureCube";
        case TextureType::Texture2DArray:
            return "u_Texture2DArray";
        case TextureType::DepthTexture:
            return "u_DepthTexture";
        default:
            return "u_Texture";
        }
    }

    class TextureElement {
    public:
        constexpr TextureElement() noexcept
            : m_type(TextureType::Texture2D), m_name(""),
              m_stages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame) {}

        constexpr TextureElement(const char* name) noexcept
            : m_type(TextureType::Texture2D), m_name(name),
              m_stages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame) {}

        constexpr TextureElement(TextureType type) noexcept
            : m_type(type), m_name(GetTextureDefaultName(type)),
              m_stages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame) {}

        constexpr TextureElement(TextureType type, const char* name) noexcept
            : m_type(type), m_name(name),
              m_stages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame) {}

        constexpr TextureElement(TextureType type, const char* name, UpdateFrequency updateFreq) noexcept
            : m_type(type), m_name(name),
              m_stages(ShaderStage::All), m_updateFrequency(updateFreq) {}

        constexpr TextureElement(TextureType type, const char* name, ShaderStages stages,
                                UpdateFrequency updateFreq = UpdateFrequency::PerFrame) noexcept
            : m_type(type), m_name(name),
              m_stages(stages), m_updateFrequency(updateFreq) {}

        constexpr TextureType getType() const noexcept { return m_type; }
        constexpr const char* getName() const noexcept { return m_name; }
        constexpr ShaderStages getShaderStages() const noexcept { return m_stages; }
        constexpr UpdateFrequency getUpdateFrequency() const noexcept { return m_updateFrequency; }

        bool operator==(const TextureElement& other) const noexcept {
            return m_type == other.m_type &&
                   StringEquals(m_name, other.m_name) &&
                   m_stages == other.m_stages &&
                   m_updateFrequency == other.m_updateFrequency;
        }

        bool operator!=(const TextureElement& other) const noexcept {
            return !(*this == other);
        }

    private:
        TextureType m_type;
        const char* m_name;
        ShaderStages m_stages;
        UpdateFrequency m_updateFrequency;
    };

    class SamplerElement {
    public:
        constexpr SamplerElement() noexcept
            : m_name(""), m_stages(ShaderStage::All),
              m_updateFrequency(UpdateFrequency::Static) {}

        constexpr SamplerElement(const char* name) noexcept
            : m_name(name), m_stages(ShaderStage::All),
              m_updateFrequency(UpdateFrequency::Static) {}

        constexpr SamplerElement(const char* name, ShaderStages stages) noexcept
            : m_name(name), m_stages(stages),
              m_updateFrequency(UpdateFrequency::Static) {}

        constexpr SamplerElement(const char* name, UpdateFrequency updateFreq) noexcept
            : m_name(name), m_stages(ShaderStage::All),
              m_updateFrequency(updateFreq) {}

        constexpr SamplerElement(const char* name, ShaderStages stages,
                                UpdateFrequency updateFreq) noexcept
            : m_name(name), m_stages(stages),
              m_updateFrequency(updateFreq) {}

        constexpr const char* getName() const noexcept { return m_name; }
        constexpr ShaderStages getShaderStages() const noexcept { return m_stages; }
        constexpr UpdateFrequency getUpdateFrequency() const noexcept { return m_updateFrequency; }

        bool operator==(const SamplerElement& other) const noexcept {
            return StringEquals(m_name, other.m_name) &&
                   m_stages == other.m_stages &&
                   m_updateFrequency == other.m_updateFrequency;
        }

        bool operator!=(const SamplerElement& other) const noexcept {
            return !(*this == other);
        }

    private:
        const char* m_name;
        ShaderStages m_stages;
        UpdateFrequency m_updateFrequency;
    };

    class ResourceLayout {
    public:
        static constexpr size_t MaxElements = 64;

        constexpr ResourceLayout() noexcept
            : m_textureCount(0), m_samplerCount(0) {}

        template<typename... Args>
        constexpr ResourceLayout(Args&&... args) noexcept
            : m_textureCount(0), m_samplerCount(0) {
            processArgs(std::forward<Args>(args)...);
        }

        constexpr void addTexture(const TextureElement& element) noexcept {
            if (m_textureCount < MaxElements) {
                m_textures[m_textureCount++] = element;
            }
        }

        constexpr void addSampler(const SamplerElement& element) noexcept {
            if (m_samplerCount < MaxElements) {
                m_samplers[m_samplerCount++] = element;
            }
        }

        constexpr size_t getTextureCount() const noexcept { return m_textureCount; }
        constexpr size_t getSamplerCount() const noexcept { return m_samplerCount; }

        constexpr const TextureElement& getTexture(size_t index) const noexcept {
            return (index < m_textureCount) ? m_textures[index] : m_textures[0];
        }

        constexpr const SamplerElement& getSampler(size_t index) const noexcept {
            return (index < m_samplerCount) ? m_samplers[index] : m_samplers[0];
        }

        constexpr int findTextureIndex(const char* name) const noexcept {
            for (size_t i = 0; i < m_textureCount; ++i) {
                if (StringEquals(m_textures[i].getName(), name)) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr int findSamplerIndex(const char* name) const noexcept {
            for (size_t i = 0; i < m_samplerCount; ++i) {
                if (StringEquals(m_samplers[i].getName(), name)) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr TextureElement findTexture(const char* name) const noexcept {
            int index = findTextureIndex(name);
            return (index >= 0) ? m_textures[index] : TextureElement();
        }

        constexpr SamplerElement findSampler(const char* name) const noexcept {
            int index = findSamplerIndex(name);
            return (index >= 0) ? m_samplers[index] : SamplerElement();
        }

    private:
        constexpr void processArgs() noexcept {

        }
        template<typename... Rest>
        constexpr void processArgs(const ResourceLayout& other, Rest&&... rest) noexcept {
            for (size_t i = 0; i < other.m_textureCount; ++i) {
                addTexture(other.m_textures[i]);
            }

            for (size_t i = 0; i < other.m_samplerCount; ++i) {
                addSampler(other.m_samplers[i]);
            }

            processArgs(std::forward<Rest>(rest)...);
        }
        template<typename... Rest>
        constexpr void processArgs(const TextureElement& element, Rest&&... rest) noexcept {
            addTexture(element);
            processArgs(std::forward<Rest>(rest)...);
        }

        template<typename... Rest>
        constexpr void processArgs(const SamplerElement& element, Rest&&... rest) noexcept {
            addSampler(element);
            processArgs(std::forward<Rest>(rest)...);
        }

        TextureElement m_textures[MaxElements];
        SamplerElement m_samplers[MaxElements];
        size_t m_textureCount;
        size_t m_samplerCount;
    };
}
#endif //RESOURCELAYOUT_H
