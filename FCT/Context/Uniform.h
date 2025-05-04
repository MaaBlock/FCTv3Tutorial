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
    enum class ConstType {
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
    constexpr size_t GetUniformSize(ConstType type) {
        switch (type) {
        case ConstType::ModelMatrix:
        case ConstType::ViewMatrix:
        case ConstType::ProjectionMatrix:
        case ConstType::MVPMatrix:
        case ConstType::Mat4:
            return sizeof(Mat4);
        case ConstType::Mat3:
            return sizeof(Mat3);
        case ConstType::Vec4:
            return sizeof(Vec4);
        case ConstType::Vec3:
            return sizeof(Vec3);
        case ConstType::Vec2:
            return sizeof(Vec2);
        case ConstType::Float:
            return sizeof(float);
        case ConstType::Int:
        case ConstType::Bool:
            return sizeof(int);
        case ConstType::Texture2D:
        case ConstType::TextureCube:
            return sizeof(int); // 纹理句柄
        case ConstType::Custom:
        default:
            return 0;
        }
    }

    // 获取Uniform类型的对齐要求
    constexpr size_t GetUniformAlignment(ConstType type)
    {
        switch (type) {
        case ConstType::ModelMatrix:
        case ConstType::ViewMatrix:
        case ConstType::ProjectionMatrix:
        case ConstType::MVPMatrix:
        case ConstType::Mat4:
        case ConstType::Mat3:
            return 16; // 矩阵对齐到16字节
        case ConstType::Vec4:
        case ConstType::Vec3:
            return 16; // 向量对齐到16字节
        case ConstType::Vec2:
            return 8;  // Vec2对齐到8字节
        case ConstType::Float:
        case ConstType::Int:
        case ConstType::Bool:
        case ConstType::Texture2D:
        case ConstType::TextureCube:
            return 4;  // 标量和纹理句柄对齐到4字节
        case ConstType::Custom:
        default:
            return 16; // 默认对齐到16字节
        }
    }
    inline constexpr const char* GetUniformDefaultName(ConstType type)
    {
        switch (type)
        {
            // 标准类型 - 不生成警告
        case ConstType::ModelMatrix:
            return "modelMatrix";
        case ConstType::ViewMatrix:
            return "viewMatrix";
        case ConstType::ProjectionMatrix:
            return "projectionMatrix";
        case ConstType::MVPMatrix:
            return "mvpMatrix";
        case ConstType::Mat4:
            return "u_Mat4";
        case ConstType::Mat3:
            return "u_Mat3";
        case ConstType::Vec4:
            return "u_Vec4";
        case ConstType::Vec3:
            return "u_Vec3";
        case ConstType::Vec2:
            return "u_Vec2";
        case ConstType::Float:
            return "u_Float";

        case ConstType::Int:
            return "u_Int";

        case ConstType::Bool:
            return "u_Bool";

        case ConstType::Texture2D:
            return "u_Texture2D";

        case ConstType::TextureCube:
            return "u_TextureCube";

        case ConstType::Custom:
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

    class ConstElement {
    public:
        constexpr ConstElement() noexcept
            : m_type(ConstType::Custom), m_name("") {}

        constexpr ConstElement(ConstType type) noexcept
            : m_type(type),m_name(GetUniformDefaultName(type)) {}

        constexpr ConstElement(ConstType type, const char* name) noexcept
            : m_type(type), m_name(name) {}

        constexpr ConstType getType() const noexcept { return m_type; }
        constexpr const char* getName() const noexcept { return m_name; }
        constexpr size_t getSize() const noexcept { return GetUniformSize(m_type); }
        constexpr size_t getAlignment() const noexcept { return GetUniformAlignment(m_type); }
        constexpr explicit operator bool() const noexcept {
            return (m_name && m_name[0] != '\0');
        }
    private:
        ConstType m_type;
        const char* m_name;
    };

    class ConstLayout {
    public:
        constexpr ConstLayout() noexcept
        : m_name(""), m_shaderStages(ShaderStage::All), m_updateFrequency(UpdateFrequency::PerFrame),
          m_elementCount(0), m_size(0)
        {
            for (size_t i = 0; i < MaxElements; ++i) {
                m_elements[i] = ConstElement();
                m_offsets[i] = 0;
            }
        }
        template<typename... Args>
        constexpr ConstLayout(const char* layoutName, Args&&... args) noexcept
        : m_name(layoutName), m_updateFrequency(UpdateFrequency::PerFrame)
        {
            processArgs(std::forward<Args>(args)...);
        }

        constexpr const char* getName() const noexcept { return m_name; }

        constexpr UpdateFrequency getUpdateFrequency() const noexcept { return m_updateFrequency; }

        constexpr ShaderStages getShaderStages() const noexcept { return m_shaderStages; }

        constexpr void addElement(const ConstElement& element) noexcept {
            if (m_elementCount < MaxElements) {
                size_t alignment = element.getAlignment();
                m_size = (m_size + alignment - 1) & ~(alignment - 1);

                m_offsets[m_elementCount] = m_size;
                m_elements[m_elementCount++] = element;
                m_size += element.getSize();
            }
        }
        bool operator==(const ConstLayout& other) const noexcept {

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

        constexpr const ConstElement& getElement(size_t index) const noexcept {
            return (index < m_elementCount) ? m_elements[index] : m_elements[0];
        }

        constexpr const ConstElement& getElement(ConstType type) const noexcept {
            auto index = findElementIndex(type);
            return (index >= 0) ? m_elements[index] : ConstElement();
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

        constexpr int findElementIndex(ConstType type) const noexcept {
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

        constexpr size_t getElementOffset(ConstType type) const noexcept {
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
        constexpr void processArgs(const ConstElement& element, Rest&&... rest) noexcept {
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
        ConstElement m_elements[MaxElements]{};
        size_t m_offsets[MaxElements]{};
        size_t m_elementCount = 0;
        size_t m_size = 0;
    };

    namespace PredefinedUniforms {
        static constexpr ConstLayout MVP = {
            "transform",
            UpdateFrequency::PerFrame,
            ConstElement(ConstType::ModelMatrix),
            ConstElement(ConstType::ViewMatrix),
            ConstElement(ConstType::ProjectionMatrix)
        };

        static constexpr ConstLayout SingleMVP = {
            "mvpTransform",
            ConstElement(ConstType::MVPMatrix),
            UpdateFrequency::PerFrame
        };

        static constexpr ConstLayout ModelViewProj = {
            "combinedTransform",
            ConstElement(ConstType::ModelMatrix),
            UpdateFrequency::PerObject,
            ConstElement(ConstType::MVPMatrix)
        };

        static constexpr ConstLayout BasicPBRMaterial = {
            "pbrMaterial",
            UpdateFrequency::Static,
            ConstElement(ConstType::Vec4, "baseColorFactor"),
            ConstElement(ConstType::Float, "metallicFactor"),
            ConstElement(ConstType::Float, "roughnessFactor"),
            ConstElement(ConstType::Vec3, "emissiveFactor")
        };
    }

    class UniformBuffer {
    public:
        explicit UniformBuffer(const ConstLayout& layout)
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
        void setValue(ConstType type, const T& value) {
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

        template<typename T>
        T getValue(const char* name) const {
            T result{};
            int index = m_layout.findElementIndex(name);
            if (index >= 0) {
                size_t offset = m_layout.getElementOffset(index);
                size_t elementSize = m_layout.getElement(index).getSize();

                if (sizeof(T) <= elementSize) {
                    std::memcpy(&result, m_data.data() + offset, sizeof(T));
                }
            }
            return result;
        }

        const void* getData() const { return m_data.data(); }

        size_t getSize() const { return m_size; }

        const ConstLayout& getLayout() const { return m_layout; }

        bool isDirty() const { return m_dirty; }

        void clearDirty() { m_dirty = false; }
    private:
        ConstLayout m_layout;
        std::vector<uint8_t> m_data;
        size_t m_size;
        bool m_dirty;
    };
}