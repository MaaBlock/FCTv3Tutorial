//
// Created by Administrator on 2025/4/5.
//
#include "./Format.h"
#include "./DataTypes.h"
#include "../Base/string.h"
#ifndef VERTEX_H
#define VERTEX_H
namespace FCT
{
     enum class VtxType {
        Position2f,
        Position3f,
        Position4f,
        Normal3f,
        Tangent3f,
        Bitangent3f,
        TexCoord2f,
        Color3f,
        Color4f,
        Custom
    };

    enum class ModelVertexAttribute {
        Position,       // 对应 position
        Normal,         // 对应 normal
        TexCoord0,      // 对应 texCoords[0]
        TexCoord1,      // 对应 texCoords[1]
        TexCoord2,
        TexCoord3,
        TexCoord4,
        TexCoord5,
        TexCoord6,
        TexCoord7,
        Color0,         // 对应 colors[0]
        Color1,         // 对应 colors[1]
        Color2,
        Color3,
        Color4,
        Color5,
        Color6,
        Color7,
        Tangent,        // 对应 tangent
        Bitangent,      // 对应 bitangent
        None            // 不对应任何ModelVertex属性
    };
    constexpr ModelVertexAttribute GetDefaultModelAttribute(VtxType type) noexcept {
        switch (type) {
        case VtxType::Position2f:
        case VtxType::Position3f:
        case VtxType::Position4f:
            return ModelVertexAttribute::Position;

        case VtxType::Normal3f:
            return ModelVertexAttribute::Normal;

        case VtxType::Tangent3f:
            return ModelVertexAttribute::Tangent;

        case VtxType::Bitangent3f:
            return ModelVertexAttribute::Bitangent;

        case VtxType::TexCoord2f:
            return ModelVertexAttribute::TexCoord0;

        case VtxType::Color3f:
        case VtxType::Color4f:
            return ModelVertexAttribute::Color0;

        case VtxType::Custom:
        default:
            return ModelVertexAttribute::None;
        }
    }

    constexpr bool isPositionType(VtxType type) noexcept
    {
        return type == VtxType::Position2f || type == VtxType::Position3f || type == VtxType::Position4f;
    }

    constexpr Format GetDefaultFormat(VtxType type) noexcept {
        switch (type) {
            case VtxType::Position2f:
                return Format::R32G32_SFLOAT;
            case VtxType::Position3f:
                return Format::R32G32B32_SFLOAT;
            case VtxType::Position4f:
                return Format::R32G32B32A32_SFLOAT;
            case VtxType::Normal3f:
                return Format::R32G32B32_SFLOAT;
            case VtxType::Tangent3f:
                return Format::R32G32B32_SFLOAT;
            case VtxType::Bitangent3f:
                return Format::R32G32B32_SFLOAT;
            case VtxType::TexCoord2f:
                return Format::R32G32_SFLOAT;
            case VtxType::Color3f:
                return Format::R32G32B32_SFLOAT;
            case VtxType::Color4f:
                return Format::R32G32B32A32_SFLOAT;
                return Format::R32G32B32A32_SFLOAT;
            case VtxType::Custom:
            default:
                return Format::UNDEFINED;
        }
    }

    constexpr const char* GetDefaultSemantic(VtxType type) noexcept {
        switch (type) {
            case VtxType::Position2f:
            case VtxType::Position3f:
            case VtxType::Position4f:
                return "pos";
            case VtxType::Normal3f:
                return "normal";
            case VtxType::Tangent3f:
                return "tangent";
            case VtxType::Bitangent3f:
                return "bitangent";
            case VtxType::TexCoord2f:
                return "texcoord";
            case VtxType::Color3f:
            case VtxType::Color4f:
                return "color";
            case VtxType::Custom:
            default:
                return "Ccstom";
        }
    }

    class VertexElement {
    public:
        constexpr VertexElement() noexcept
       : m_type(VtxType::Custom), m_format(Format::UNDEFINED), m_semantic(""),m_modelAttribute(ModelVertexAttribute::None) {}
        template<typename... Args>
        constexpr VertexElement(Args&&... args) noexcept
           : m_type(VtxType::Custom), m_format(Format::UNDEFINED), m_semantic(""),
             m_modelAttribute(ModelVertexAttribute::None) {
            processArgs(std::forward<Args>(args)...);
        }
        constexpr ModelVertexAttribute getModelAttribute() const noexcept { return m_modelAttribute; }

        constexpr VtxType getType() const noexcept { return m_type; }

        constexpr Format getFormat() const noexcept { return m_format; }

        constexpr const char* getSemantic() const noexcept { return m_semantic; }

        constexpr size_t getSize() const noexcept {
            return FormatSize(m_format);
        }

    private:
        constexpr void processArgs() noexcept {}
        constexpr void processArgs(VtxType type) noexcept {
            m_type = type;
            if (m_format == Format::UNDEFINED) {
                m_format = GetDefaultFormat(type);
            }
            if (StringEquals(m_semantic, "")) {
                m_semantic = GetDefaultSemantic(type);
            }
            if (m_modelAttribute == ModelVertexAttribute::None) {
                m_modelAttribute = GetDefaultModelAttribute(type);
            }
        }

        constexpr void processArgs(Format format) noexcept {
            m_format = format;
        }

        constexpr void processArgs(const char* semantic) noexcept {
            m_semantic = semantic;
        }

        constexpr void processArgs(ModelVertexAttribute attr) noexcept {
            m_modelAttribute = attr;
        }

        template<typename First, typename... Rest>
        constexpr void processArgs(First&& first, Rest&&... rest) noexcept {
            processArgs(std::forward<First>(first));
            processArgs(std::forward<Rest>(rest)...);
        }
        VtxType m_type;
        Format m_format;
        const char* m_semantic;
        ModelVertexAttribute m_modelAttribute;
    };

    class VertexLayout
    {
    public:
        template<typename... Elements>
        constexpr VertexLayout(const Elements&... elements) noexcept {
            (addElement(elements), ...);
        }
        constexpr void addElement(const VertexElement& element) noexcept {
            if (m_elementCount < MaxElements) {
                m_elements[m_elementCount++] = element;
                m_stride += element.getSize();
            }
        }

        constexpr size_t getElementCount() const noexcept {
            return m_elementCount;
        }

        constexpr const VertexElement& getElement(size_t index) const noexcept {
            return (index < m_elementCount) ? m_elements[index] : m_elements[0];
        }

        constexpr size_t getStride() const noexcept {
            return m_stride;
        }

        constexpr int getElementIndexBySemantic(const char* semantic) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (StringEquals(m_elements[i].getSemantic(), semantic)) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr int getElementIndexByType(VtxType type) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() == type) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr const VertexElement* getElementByType(VtxType type) const noexcept {
            int index = getElementIndexByType(type);
            return (index >= 0) ? &m_elements[index] : nullptr;
        }

        constexpr const VertexElement* getElementBySemantic(const char* semantic) const noexcept {
            int index = getElementIndexBySemantic(semantic);
            return (index >= 0) ? &m_elements[index] : nullptr;
        }

        constexpr size_t getElementOffset(size_t index) const noexcept {
            size_t offset = 0;
            for (size_t i = 0; i < index && i < m_elementCount; ++i) {
                offset += m_elements[i].getSize();
            }
            return offset;
        }

        constexpr size_t getElementOffsetByType(VtxType type) const noexcept {
            int index = getElementIndexByType(type);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

        constexpr size_t getElementOffsetBySemantic(const char* semantic) const noexcept {
            int index = getElementIndexBySemantic(semantic);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

    private:
        static constexpr size_t MaxElements = 64;
        VertexElement m_elements[MaxElements]{};
        size_t m_elementCount = 0;
        size_t m_stride = 0;
    };
    class PixelLayout
    {
    public:
        //constexpr PixelLayout() noexcept : m_elementCount(0), m_stride(0) {}

        template<typename... Args>
        constexpr PixelLayout(Args&&... args) noexcept : m_elementCount(0), m_stride(0) {
            addElements(std::forward<Args>(args)...);
        }

        constexpr void addElement(const VertexElement& element) noexcept {
            if (m_elementCount < MaxElements) {
                m_elements[m_elementCount++] = element;
                m_stride += element.getSize();
            }
        }

        constexpr void addElements(const VertexLayout& layout) noexcept {
            for (size_t i = 0; i < layout.getElementCount() && m_elementCount < MaxElements; ++i) {
                addElement(layout.getElement(i));
            }
        }

        constexpr void addElements(const PixelLayout& layout) noexcept {
            for (size_t i = 0; i < layout.getElementCount() && m_elementCount < MaxElements; ++i) {
                addElement(layout.getElement(i));
            }
        }

        constexpr size_t getElementCount() const noexcept {
            return m_elementCount;
        }

        constexpr const VertexElement& getElement(size_t index) const noexcept {
            return (index < m_elementCount) ? m_elements[index] : m_elements[0];
        }

        constexpr size_t getStride() const noexcept {
            return m_stride;
        }

        constexpr int getElementIndexBySemantic(const char* semantic) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (StringEquals(m_elements[i].getSemantic(), semantic)) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr int getElementIndexByType(VtxType type) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() == type) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr const VertexElement* getElementByType(VtxType type) const noexcept {
            int index = getElementIndexByType(type);
            return (index >= 0) ? &m_elements[index] : nullptr;
        }

        constexpr const VertexElement* getElementBySemantic(const char* semantic) const noexcept {
            int index = getElementIndexBySemantic(semantic);
            return (index >= 0) ? &m_elements[index] : nullptr;
        }

        constexpr size_t getElementOffset(size_t index) const noexcept {
            size_t offset = 0;
            for (size_t i = 0; i < index && i < m_elementCount; ++i) {
                offset += m_elements[i].getSize();
            }
            return offset;
        }

        constexpr size_t getElementOffsetByType(VtxType type) const noexcept {
            int index = getElementIndexByType(type);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

        constexpr size_t getElementOffsetBySemantic(const char* semantic) const noexcept {
            int index = getElementIndexBySemantic(semantic);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

        constexpr void ensurePositionFirst() noexcept {
            int posIndex = getElementIndexByType(VtxType::Position3f);
            if (posIndex < 0) {
                posIndex = getElementIndexByType(VtxType::Position2f);
            }

            if (posIndex > 0) {
                VertexElement posElement = m_elements[posIndex];

                for (int i = posIndex; i > 0; --i) {
                    m_elements[i] = m_elements[i - 1];
                }

                m_elements[0] = posElement;

            }
        }

    private:
        constexpr void addElements() noexcept {}

        template<typename... Rest>
        constexpr void addElements(const VertexElement& element, Rest&&... rest) noexcept {
            addElement(element);
            addElements(std::forward<Rest>(rest)...);
        }

        template<typename... Rest>
        constexpr void addElements(const VertexLayout& layout, Rest&&... rest) noexcept {
            addElements(layout);
            addElements(std::forward<Rest>(rest)...);
        }

        static constexpr size_t MaxElements = 64;
        VertexElement m_elements[MaxElements];
        size_t m_elementCount;
        size_t m_stride;
    };
    class VertexBuffer;

    class Vertex {
        friend class VertexBuffer;
    public:
        template<typename T>
        T& getAttribute(VtxType type) noexcept {
            const auto* element = m_layout.getElementByType(type);
            if (element) {
                size_t offset = m_layout.getElementOffsetByType(type);
                return *reinterpret_cast<T*>(m_data + offset);
            }
            static T dummy{};
            return dummy;
        }

        template<typename T>
        T& getAttribute(const char* semantic) noexcept {
            const auto* element = m_layout.getElementBySemantic(semantic);
            if (element) {
                size_t offset = m_layout.getElementOffsetBySemantic(semantic);
                return *reinterpret_cast<T*>(m_data + offset);
            }
            static T dummy{};
            return dummy;
        }

        template<typename T>
        void setAttribute(VtxType type, const T& value) noexcept {
            const auto* element = m_layout.getElementByType(type);
            if (element) {
                size_t offset = m_layout.getElementOffsetByType(type);
                *reinterpret_cast<T*>(m_data + offset) = value;
            }
        }
        template<typename T>
        void setAttribute(size_t elementIndex, const T& value) noexcept {
            if (elementIndex < m_layout.getElementCount()) {
                size_t offset = m_layout.getElementOffset(elementIndex);
                *reinterpret_cast<T*>(m_data + offset) = value;
            }
        }

        template<typename T>
        void setAttribute(const char* semantic, const T& value) noexcept {
            const auto* element = m_layout.getElementBySemantic(semantic);
            if (element) {
                size_t offset = m_layout.getElementOffsetBySemantic(semantic);
                *reinterpret_cast<T*>(m_data + offset) = value;
            }
        }

        uint8_t* getData() noexcept {
            return m_data;
        }

        const uint8_t* getData() const noexcept {
            return m_data;
        }

        const VertexLayout& getLayout() const noexcept {
            return m_layout;
        }

    private:
        Vertex(uint8_t* data, const VertexLayout& layout) noexcept
            : m_data(data), m_layout(layout) {}

        uint8_t* m_data;
        const VertexLayout& m_layout;
    };

    class ConstVertex {
    public:
        ConstVertex(const Vertex& vertex) noexcept
            : m_data(vertex.getData()), m_layout(vertex.getLayout()) {}

        template<typename T>
        const T& getAttribute(VtxType type) const noexcept {
            const auto* element = m_layout.getElementByType(type);
            if (element) {
                size_t offset = m_layout.getElementOffsetByType(type);
                return *reinterpret_cast<const T*>(m_data + offset);
            }
            static const T dummy{};
            return dummy;
        }

        template<typename T>
        const T& getAttribute(const char* semantic) const noexcept {
            const auto* element = m_layout.getElementBySemantic(semantic);
            if (element) {
                size_t offset = m_layout.getElementOffsetBySemantic(semantic);
                return *reinterpret_cast<const T*>(m_data + offset);
            }
            static const T dummy{};
            return dummy;
        }

        const uint8_t* getData() const noexcept {
            return m_data;
        }

        const VertexLayout& getLayout() const noexcept {
            return m_layout;
        }

    private:
        const uint8_t* m_data;
        const VertexLayout& m_layout;
    };
    class VertexBuffer {
    public:
        explicit VertexBuffer(const VertexLayout& layout) noexcept
            : m_layout(layout), m_stride(layout.getStride()) {}

        const VertexLayout& getLayout() const noexcept {
            return m_layout;
        }

        size_t getVertexCount() const noexcept {
            return m_data.size() / m_stride;
        }

        size_t getStride() const noexcept {
            return m_stride;
        }
        void* getData()
        {
            return m_data.data();
        }

        const void* getData() const noexcept {
            return m_data.data();
        }

        size_t getDataSize() const noexcept {
            return m_data.size();
        }

        void reserve(size_t vertexCount) {
            m_data.reserve(vertexCount * m_stride);
        }

        void resize(size_t vertexCount) {
            m_data.resize(vertexCount * m_stride);
        }

        void clear() noexcept {
            m_data.clear();
        }

        size_t addVertex() {
            size_t index = getVertexCount();
            m_data.resize((index + 1) * m_stride);
            return index;
        }

        Vertex operator[](size_t index) noexcept {
            assert(index < getVertexCount() && "Vertex index out of range");
            return Vertex(m_data.data() + (index * m_stride), m_layout);
        }
/*
        ConstVertex operator[](size_t index) const noexcept {
            assert(index < getVertexCount() && "Vertex index out of range");
            return ConstVertex(Vertex(const_cast<uint8_t*>(m_data.data()) + (index * m_stride), m_layout));
        }*/

        Vertex front() noexcept {
            assert(!m_data.empty() && "Vertex buffer is empty");
            return Vertex(m_data.data(), m_layout);
        }

        Vertex back() noexcept {
            assert(!m_data.empty() && "Vertex buffer is empty");
            return Vertex(m_data.data() + ((getVertexCount() - 1) * m_stride), m_layout);
        }

        ConstVertex front() const noexcept {
            assert(!m_data.empty() && "Vertex buffer is empty");
            return ConstVertex(Vertex(const_cast<uint8_t*>(m_data.data()), m_layout));
        }

        ConstVertex back() const noexcept {
            assert(!m_data.empty() && "Vertex buffer is empty");
            return ConstVertex(Vertex(const_cast<uint8_t*>(m_data.data() + ((getVertexCount() - 1) * m_stride)), m_layout));
        }

        template<typename... Args>
        void emplaceBack(Args&&... args) {
            static_assert(sizeof...(args) > 0, "At least one attribute must be provided");

            size_t index = addVertex();
            Vertex vertex = (*this)[index];

            setAttributes(vertex, 0, std::forward<Args>(args)...);
        }


    private:
        template<typename T>
        void setAttributes(Vertex& vertex, size_t elementIndex, T&& value) {
            if (elementIndex < m_layout.getElementCount()) {
                vertex.setAttribute(elementIndex, std::forward<T>(value));
            }
        }

        template<typename T, typename... Rest>
        void setAttributes(Vertex& vertex, size_t elementIndex, T&& first, Rest&&... rest) {
            if (elementIndex < m_layout.getElementCount()) {
                vertex.setAttribute(elementIndex, std::forward<T>(first));
                setAttributes(vertex, elementIndex + 1, std::forward<Rest>(rest)...);
            }
        }

        std::vector<uint8_t> m_data;
        VertexLayout m_layout;
        size_t m_stride;
    };
}
#endif //VERTEX_H