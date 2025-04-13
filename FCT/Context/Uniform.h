#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <cstring>
#include "DataTypes.h"
#include "../Base/Flags.h"
#include "../Base/string.h"

namespace FCT
{
    // Uniform类型枚举
    enum class UniformType {
        // 矩阵类型
        ModelMatrix,
        ViewMatrix,
        ProjectionMatrix,
        MVPMatrix,
        Mat4,
        Mat3,

        // 向量类型
        Vec4,
        Vec3,
        Vec2,

        // 标量类型
        Float,
        Int,
        Bool,

        // 纹理类型
        Texture2D,
        TextureCube,

        // 自定义类型
        Custom
    };

    enum class ShaderStage : uint32_t {
        Vertex          = 0x00000001,
        Fragment        = 0x00000002,
        Compute         = 0x00000004,
        Geometry        = 0x00000008,
        TessControl     = 0x00000010,
        TessEvaluation  = 0x00000020,
        Mesh            = 0x00000040,
        Task            = 0x00000080,
        RayGen          = 0x00000100,
        AnyHit          = 0x00000200,
        ClosestHit      = 0x00000400,
        Miss            = 0x00000800,
        Intersection    = 0x00001000,
        Callable        = 0x00002000,
        All             = 0xFFFFFFFF
    };

    using ShaderStages = Flags<ShaderStage>;

#ifdef FCT_USE_VULKAN
#include <vulkan/vulkan.hpp>

    inline vk::ShaderStageFlags ConvertToVkShaderStageFlags(ShaderStages stages)
    {
        vk::ShaderStageFlags flags;

        if (stages & ShaderStage::Vertex)
            flags |= vk::ShaderStageFlagBits::eVertex;

        if (stages & ShaderStage::Fragment)
            flags |= vk::ShaderStageFlagBits::eFragment;

        if (stages & ShaderStage::Compute)
            flags |= vk::ShaderStageFlagBits::eCompute;

        if (stages & ShaderStage::Geometry)
            flags |= vk::ShaderStageFlagBits::eGeometry;

        if (stages & ShaderStage::TessControl)
            flags |= vk::ShaderStageFlagBits::eTessellationControl;

        if (stages & ShaderStage::TessEvaluation)
            flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;

        if (stages & ShaderStage::Task)
            flags |= vk::ShaderStageFlagBits::eTaskEXT;

        if (stages & ShaderStage::Mesh)
            flags |= vk::ShaderStageFlagBits::eMeshEXT;

        if (stages & ShaderStage::RayGen)
            flags |= vk::ShaderStageFlagBits::eRaygenKHR;

        if (stages & ShaderStage::AnyHit)
            flags |= vk::ShaderStageFlagBits::eAnyHitKHR;

        if (stages & ShaderStage::ClosestHit)
            flags |= vk::ShaderStageFlagBits::eClosestHitKHR;

        if (stages & ShaderStage::Miss)
            flags |= vk::ShaderStageFlagBits::eMissKHR;

        if (stages & ShaderStage::Intersection)
            flags |= vk::ShaderStageFlagBits::eIntersectionKHR;

        if (stages & ShaderStage::Callable)
            flags |= vk::ShaderStageFlagBits::eCallableKHR;

        if (!flags)
            flags = vk::ShaderStageFlagBits::eAll;

        return flags;
    }
#endif

    // 获取Uniform类型的大小
    constexpr size_t GetUniformSize(UniformType type) {
        switch (type) {
        case UniformType::ModelMatrix:
        case UniformType::ViewMatrix:
        case UniformType::ProjectionMatrix:
        case UniformType::MVPMatrix:
        case UniformType::Mat4:
            return sizeof(Mat4);
        case UniformType::Mat3:
            return sizeof(Mat3);
        case UniformType::Vec4:
            return sizeof(Vec4);
        case UniformType::Vec3:
            return sizeof(Vec3);
        case UniformType::Vec2:
            return sizeof(Vec2);
        case UniformType::Float:
            return sizeof(float);
        case UniformType::Int:
        case UniformType::Bool:
            return sizeof(int);
        case UniformType::Texture2D:
        case UniformType::TextureCube:
            return sizeof(int); // 纹理句柄
        case UniformType::Custom:
        default:
            return 0;
        }
    }

    // 获取Uniform类型的对齐要求
    constexpr size_t GetUniformAlignment(UniformType type)
    {
        switch (type) {
        case UniformType::ModelMatrix:
        case UniformType::ViewMatrix:
        case UniformType::ProjectionMatrix:
        case UniformType::MVPMatrix:
        case UniformType::Mat4:
        case UniformType::Mat3:
            return 16; // 矩阵对齐到16字节
        case UniformType::Vec4:
        case UniformType::Vec3:
            return 16; // 向量对齐到16字节
        case UniformType::Vec2:
            return 8;  // Vec2对齐到8字节
        case UniformType::Float:
        case UniformType::Int:
        case UniformType::Bool:
        case UniformType::Texture2D:
        case UniformType::TextureCube:
            return 4;  // 标量和纹理句柄对齐到4字节
        case UniformType::Custom:
        default:
            return 16; // 默认对齐到16字节
        }
    }
    inline constexpr const char* GetUniformDefaultName(UniformType type)
    {
        switch (type)
        {
            // 标准类型 - 不生成警告
        case UniformType::ModelMatrix:
            return "modelMatrix";
        case UniformType::ViewMatrix:
            return "viewMatrix";
        case UniformType::ProjectionMatrix:
            return "projectionMatrix";
        case UniformType::MVPMatrix:
            return "mvpMatrix";
        case UniformType::Mat4:
            return "u_Mat4";
        case UniformType::Mat3:
            return "u_Mat3";
        case UniformType::Vec4:
            return "u_Vec4";
        case UniformType::Vec3:
            return "u_Vec3";
        case UniformType::Vec2:
            return "u_Vec2";
        case UniformType::Float:
            return "u_Float";

        case UniformType::Int:
            return "u_Int";

        case UniformType::Bool:
            return "u_Bool";

        case UniformType::Texture2D:
            return "u_Texture2D";

        case UniformType::TextureCube:
            return "u_TextureCube";

        case UniformType::Custom:
        default:
            return "u_Custom";
        }
    }

    enum class UpdateFrequency {
        Static,     // 很少更新（如场景常量）
        PerFrame,   // 每帧更新（如相机矩阵）
        PerObject,  // 每个对象更新（如模型矩阵）
        Dynamic     // 频繁更新（如动画数据）
    };

    class UniformElement {
    public:
        constexpr UniformElement() noexcept
            : m_type(UniformType::Custom), m_name("") {}

        constexpr UniformElement(UniformType type) noexcept
            : m_type(type),m_name(GetUniformDefaultName(type)) {}

        constexpr UniformElement(UniformType type, const char* name) noexcept
            : m_type(type), m_name(name) {}

        constexpr UniformType getType() const noexcept { return m_type; }
        constexpr const char* getName() const noexcept { return m_name; }
        constexpr size_t getSize() const noexcept { return GetUniformSize(m_type); }
        constexpr size_t getAlignment() const noexcept { return GetUniformAlignment(m_type); }

    private:
        UniformType m_type;
        const char* m_name;
    };

    class UniformLayout {
    public:
        constexpr UniformLayout() noexcept
        : m_name(""), m_shaderStages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame),
          m_elementCount(0), m_size(0)
        {
            for (size_t i = 0; i < MaxElements; ++i) {
                m_elements[i] = UniformElement();
                m_offsets[i] = 0;
            }
        }
        template<typename... Args>
        constexpr UniformLayout(const char* layoutName, Args&&... args) noexcept
        : m_name(layoutName), m_updateFrequency(UpdateFrequency::PerFrame)
        {
            processArgs(std::forward<Args>(args)...);
        }

        constexpr const char* getName() const noexcept { return m_name; }

        constexpr UpdateFrequency getUpdateFrequency() const noexcept { return m_updateFrequency; }

        constexpr ShaderStages getShaderStages() const noexcept { return m_shaderStages; }

        constexpr void addElement(const UniformElement& element) noexcept {
            if (m_elementCount < MaxElements) {
                size_t alignment = element.getAlignment();
                m_size = (m_size + alignment - 1) & ~(alignment - 1);

                m_offsets[m_elementCount] = m_size;
                m_elements[m_elementCount++] = element;
                m_size += element.getSize();
            }
        }
        bool operator==(const UniformLayout& other) const noexcept {

            if (!StringEquals(m_name, other.m_name)) {
                return false;
            }
            if (m_elementCount != other.m_elementCount) {
                return false;
            }

            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() != other.m_elements[i].getType()) {
                    return false;
                }
            }
            return true;
        }

        constexpr size_t getElementCount() const noexcept { return m_elementCount; }

        constexpr const UniformElement& getElement(size_t index) const noexcept {
            return (index < m_elementCount) ? m_elements[index] : m_elements[0];
        }

        constexpr size_t getTotalSize() const noexcept {
            return (m_size + 15) & ~15;
        }

        constexpr int findElementIndex(const char* name) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (strcmp(m_elements[i].getName(), name) == 0) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr int findElementIndex(UniformType type) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() == type) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr size_t getElementOffset(size_t index) const noexcept {
            return (index < m_elementCount) ? m_offsets[index] : 0;
        }

        constexpr size_t getElementOffset(const char* name) const noexcept {
            int index = findElementIndex(name);
            return (index >= 0) ? m_offsets[index] : 0;
        }

        constexpr size_t getElementOffset(UniformType type) const noexcept {
            int index = findElementIndex(type);
            return (index >= 0) ? m_offsets[index] : 0;
        }

    private:
        constexpr void processArgs() noexcept {

        }

        template<typename... Rest>
        constexpr void processArgs(UpdateFrequency frequency, Rest&&... rest) noexcept {
            m_updateFrequency = frequency;
            processArgs(std::forward<Rest>(rest)...);
        }

        template<typename... Rest>
        constexpr void processArgs(const UniformElement& element, Rest&&... rest) noexcept {
            addElement(element);
            processArgs(std::forward<Rest>(rest)...);
        }

        template<typename... Rest>
        constexpr void processArgs(ShaderStages stages, Rest&&... rest) noexcept {
            m_shaderStages = stages;
            processArgs(std::forward<Rest>(rest)...);
        }

        static constexpr size_t MaxElements = 16;
        const char* m_name = "";
        ShaderStages m_shaderStages = ShaderStage::All;
        UpdateFrequency m_updateFrequency = UpdateFrequency::PerFrame;
        UniformElement m_elements[MaxElements]{};
        size_t m_offsets[MaxElements]{};
        size_t m_elementCount = 0;
        size_t m_size = 0;
    };

    namespace PredefinedUniforms {
        static constexpr UniformLayout MVP = {
            "transform",
            UpdateFrequency::PerFrame,
            UniformElement(UniformType::ModelMatrix),
            UniformElement(UniformType::ViewMatrix),
            UniformElement(UniformType::ProjectionMatrix)
        };

        static constexpr UniformLayout SingleMVP = {
            "mvpTransform",
            UniformElement(UniformType::MVPMatrix),
            UpdateFrequency::PerFrame
        };

        static constexpr UniformLayout ModelViewProj = {
            "combinedTransform",
            UniformElement(UniformType::ModelMatrix),
            UpdateFrequency::PerObject,
            UniformElement(UniformType::MVPMatrix)
        };

        static constexpr UniformLayout BasicPBRMaterial = {
            "pbrMaterial",
            UpdateFrequency::Static,
            UniformElement(UniformType::Vec4, "baseColorFactor"),
            UniformElement(UniformType::Float, "metallicFactor"),
            UniformElement(UniformType::Float, "roughnessFactor"),
            UniformElement(UniformType::Vec3, "emissiveFactor")
        };
    }

    class UniformBuffer {
    public:
        explicit UniformBuffer(const UniformLayout& layout)
            : m_layout(layout), m_size(layout.getTotalSize()), m_dirty(true) {
            m_data.resize(m_size, 0);
        }

        UpdateFrequency getUpdateFrequency() const { return m_layout.getUpdateFrequency(); }

        template<typename T>
        void setValue(const char* name, const T& value) {
            int index = m_layout.findElementIndex(name);
            if (index >= 0) {
                size_t offset = m_layout.getElementOffset(index);
                size_t elementSize = m_layout.getElement(index).getSize();

                if (sizeof(T) <= elementSize) {
                    std::memcpy(m_data.data() + offset, &value, sizeof(T));
                    m_dirty = true;
                }
            }
        }

        template<typename T>
        void setValue(UniformType type, const T& value) {
            int index = m_layout.findElementIndex(type);
            if (index >= 0) {
                size_t offset = m_layout.getElementOffset(index);
                size_t elementSize = m_layout.getElement(index).getSize();

                if (sizeof(T) <= elementSize) {
                    std::memcpy(m_data.data() + offset, &value, sizeof(T));
                    m_dirty = true;
                }
            }
        }

        const void* getData() const { return m_data.data(); }

        size_t getSize() const { return m_size; }

        const UniformLayout& getLayout() const { return m_layout; }

        bool isDirty() const { return m_dirty; }

        void clearDirty() { m_dirty = false; }
    private:
        UniformLayout m_layout;
        std::vector<uint8_t> m_data;
        size_t m_size;
        bool m_dirty;
    };
}