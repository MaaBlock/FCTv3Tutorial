//
// Created by Administrator on 2025/4/5.
//
#include "./Format.h"
#ifndef VERTEX_H
#define VERTEX_H
namespace FCT
{
     enum class ElementType {
        Position2f,
        Position3f,
        Normal3f,
        Tangent3f,
        Bitangent3f,
        TexCoord2f,
        Color3f,
        Color4f,
        Custom
    };

    constexpr bool isPositionType(ElementType type) noexcept
    {
        return type == ElementType::Position2f || type == ElementType::Position3f;
    }

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

    constexpr Format GetDefaultFormat(ElementType type) noexcept {
        switch (type) {
            case ElementType::Position2f:
                return Format::R32G32_SFLOAT;
            case ElementType::Position3f:
                return Format::R32G32B32_SFLOAT;
            case ElementType::Normal3f:
                return Format::R32G32B32_SFLOAT;
            case ElementType::Tangent3f:
                return Format::R32G32B32_SFLOAT;
            case ElementType::Bitangent3f:
                return Format::R32G32B32_SFLOAT;
            case ElementType::TexCoord2f:
                return Format::R32G32_SFLOAT;
            case ElementType::Color3f:
                return Format::R32G32B32_SFLOAT;
            case ElementType::Color4f:
                return Format::R32G32B32A32_SFLOAT;
                return Format::R32G32B32A32_SFLOAT;
            case ElementType::Custom:
            default:
                return Format::UNDEFINED;
        }
    }

    constexpr const char* GetDefaultSemantic(ElementType type) noexcept {
        switch (type) {
            case ElementType::Position2f:
            case ElementType::Position3f:
                return "pos";
            case ElementType::Normal3f:
                return "normal";
            case ElementType::Tangent3f:
                return "tangent";
            case ElementType::Bitangent3f:
                return "bitangent";
            case ElementType::TexCoord2f:
                return "texcoord";
            case ElementType::Color3f:
            case ElementType::Color4f:
                return "color";
            case ElementType::Custom:
            default:
                return "Ccstom";
        }
    }

    class VertexElement {
    public:
        constexpr VertexElement() noexcept
       : m_type(ElementType::Custom), m_format(Format::UNDEFINED), m_semantic("") {}

        constexpr VertexElement(ElementType type) noexcept
            : m_type(type), m_format(GetDefaultFormat(type)), m_semantic(GetDefaultSemantic(type)) {}

        constexpr VertexElement(ElementType type, const char* semantic) noexcept
            : m_type(type), m_format(GetDefaultFormat(type)), m_semantic(semantic) {}

        constexpr VertexElement(ElementType type, Format format, const char* semantic) noexcept
            : m_type(type), m_format(format), m_semantic(semantic) {}

        constexpr VertexElement(Format format, const char* semantic) noexcept
            : m_type(ElementType::Custom), m_format(format), m_semantic(semantic) {}

        constexpr ElementType getType() const noexcept { return m_type; }

        constexpr Format getFormat() const noexcept { return m_format; }

        constexpr const char* getSemantic() const noexcept { return m_semantic; }

        constexpr size_t getSize() const noexcept {
            return FormatSize(m_format);
        }

    private:
        ElementType m_type;
        Format m_format;
        const char* m_semantic;
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

        constexpr int getElementIndexByType(ElementType type) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() == type) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr const VertexElement* getElementByType(ElementType type) const noexcept {
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

        constexpr size_t getElementOffsetByType(ElementType type) const noexcept {
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

        constexpr int getElementIndexByType(ElementType type) const noexcept {
            for (size_t i = 0; i < m_elementCount; ++i) {
                if (m_elements[i].getType() == type) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        constexpr const VertexElement* getElementByType(ElementType type) const noexcept {
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

        constexpr size_t getElementOffsetByType(ElementType type) const noexcept {
            int index = getElementIndexByType(type);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

        constexpr size_t getElementOffsetBySemantic(const char* semantic) const noexcept {
            int index = getElementIndexBySemantic(semantic);
            return (index >= 0) ? getElementOffset(index) : 0;
        }

        constexpr void ensurePositionFirst() noexcept {
            int posIndex = getElementIndexByType(ElementType::Position3f);
            if (posIndex < 0) {
                posIndex = getElementIndexByType(ElementType::Position2f);
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
        T& getAttribute(ElementType type) noexcept {
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
        void setAttribute(ElementType type, const T& value) noexcept {
            const auto* element = m_layout.getElementByType(type);
            if (element) {
                size_t offset = m_layout.getElementOffsetByType(type);
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
        const T& getAttribute(ElementType type) const noexcept {
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

        ConstVertex operator[](size_t index) const noexcept {
            assert(index < getVertexCount() && "Vertex index out of range");
            return ConstVertex(Vertex(const_cast<uint8_t*>(m_data.data()) + (index * m_stride), m_layout));
        }

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
                const auto& element = m_layout.getElement(elementIndex);
                vertex.setAttribute(element.getType(), std::forward<T>(value));
            }
        }

        template<typename T, typename... Rest>
        void setAttributes(Vertex& vertex, size_t elementIndex, T&& first, Rest&&... rest) {
            if (elementIndex < m_layout.getElementCount()) {
                const auto& element = m_layout.getElement(elementIndex);
                vertex.setAttribute(element.getType(), std::forward<T>(first));
                setAttributes(vertex, elementIndex + 1, std::forward<Rest>(rest)...);
            }
        }

        std::vector<uint8_t> m_data;
        VertexLayout m_layout;
        size_t m_stride;
    };
}
#endif //VERTEX_H