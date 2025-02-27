#pragma once
#include "./VertexFactory.h"
namespace FCT
{
    class VertexArray
    {
    public:
        VertexArray(const VertexFactory *factory, size_t vertexCount)
            : factory(factory),
              vertexCount(vertexCount),
              dataVec(factory->getStride() * vertexCount)
        {
        }

        virtual ~VertexArray() {};

        VertexArray(const VertexArray &) = delete;
        VertexArray &operator=(const VertexArray &) = delete;
        VertexArray(VertexArray &&) = delete;
        VertexArray &operator=(VertexArray &&) = delete;

        template <typename T>
        void setAttribute(size_t vertexIndex, const std::string &name, const T &value);
        template <typename T>
        T getAttribute(size_t vertexIndex, const std::string &name) const;
        size_t getAttributeOffset(const std::string &name) const
        {
            const auto &attr = factory->getAttribute(name);
            return attr.offset;
        } 
        size_t getAttributeOffsetBytype(PipelineAttributeType type) const
        {
            auto attrs = factory->getAttributes();
            for (auto attr : attrs) {
                if (attr.type == type) {
                    return attr.offset;
                }
            }
        }

        template <typename T>
        void setAttribute(size_t vertexIndex, size_t offset, const T &value)
        {
            const size_t dataOffset = factory->getStride() * vertexIndex + offset;
            std::memcpy(dataVec.data() + dataOffset, &value, sizeof(T));
        }
        // template <>
        // void setAttribute<int>(size_t vertexIndex, const std::string& name, const int& value);
        void *data() { return dataVec.data(); }
        const void *data() const { return dataVec.data(); }
        size_t getByteSize() const { return dataVec.size(); }
        size_t getVertexCount() const { return vertexCount; }

        const VertexFactory *getFactory() const { return factory; }

        void setPosition(size_t vertexIndex, const Vec2 &position)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Position2f, position);
        }

        void setPosition(size_t vertexIndex, const Vec3 &position)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Position3f, position);
        }

        void setPosition(size_t vertexIndex, const Vec4 &position)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Position4f, position);
        }

        void setColor(size_t vertexIndex, const Vec4 &color)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Color4f, color);
        }

        void setTexCoord(size_t vertexIndex, const Vec2 &texCoord)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::TexCoord2f, texCoord);
        }

        void setNormal(size_t vertexIndex, const Vec3 &normal)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Normal3f, normal);
        }

        void setTangent(size_t vertexIndex, const Vec3 &tangent)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Tangent3f, tangent);
        }

        void setBitangent(size_t vertexIndex, const Vec3 &bitangent)
        {
            setAttributeByType(vertexIndex, PipelineAttributeType::Bitangent3f, bitangent);
        }
        void addVertex(int size = 1)
        {
            const size_t stride = factory->getStride();
            const size_t newSize = dataVec.size() + size * stride;

            dataVec.resize(newSize);
            vertexCount += size;
        }
        void clear()
        {
            dataVec.clear();
            vertexCount = 0;
        }

    private:
        void handlePositionMismatch(size_t vertexIndex, PipelineAttributeType type, const Vec2 &value)
        {
            if (type == PipelineAttributeType::Position3f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position3f, Vec3(value.x, value.y, 0.0f));
            }
            else if (type == PipelineAttributeType::Position4f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position4f, Vec4(value.x, value.y, 0.0f, 1.0f));
            }
        }

        void handlePositionMismatch(size_t vertexIndex, PipelineAttributeType type, const Vec3 &value)
        {
            if (type == PipelineAttributeType::Position2f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position2f, Vec2(value.x, value.y));
            }
            else if (type == PipelineAttributeType::Position4f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position4f, Vec4(value.x, value.y, value.z, 1.0f));
            }
        }

        void handlePositionMismatch(size_t vertexIndex, PipelineAttributeType type, const Vec4 &value)
        {
            if (type == PipelineAttributeType::Position2f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position2f, Vec2(value.x, value.y));
            }
            else if (type == PipelineAttributeType::Position3f)
            {
                setAttributeByType(vertexIndex, PipelineAttributeType::Position3f, Vec3(value.x, value.y, value.z));
            }
        }

        template <typename T>
        void handleMismatch(size_t vertexIndex, PipelineAttributeType type, const T &value)
        {
            switch (type)
            {
            case PipelineAttributeType::Position2f:
            case PipelineAttributeType::Position3f:
            case PipelineAttributeType::Position4f:
                handlePositionMismatch(vertexIndex, type, value);
                break;
            default:
                break;
            }
        }

        bool isCompatibleType(PipelineAttributeType attrType, PipelineAttributeType valueType)
        {
            if (attrType == valueType)
                return true;
            bool isAttrPosition = (attrType == PipelineAttributeType::Position2f ||
                                   attrType == PipelineAttributeType::Position3f ||
                                   attrType == PipelineAttributeType::Position4f);
            bool isValuePosition = (valueType == PipelineAttributeType::Position2f ||
                                    valueType == PipelineAttributeType::Position3f ||
                                    valueType == PipelineAttributeType::Position4f);

            return isAttrPosition && isValuePosition;
        }

        template <typename T>
        void setAttributeByType(size_t vertexIndex, PipelineAttributeType type, const T &value)
        {
            for (const auto &attr : factory->getAttributes())
            {
                if (isCompatibleType(attr.type, type))
                {
                    if (GetDataTypeSize(attr.dataType) == sizeof(T))
                    {
                        setAttribute(vertexIndex, attr.name, value);
                    }
                    else
                    {
                        handleMismatch(vertexIndex, attr.type, value);
                    }
                    return;
                }
            }
            // throw std::runtime_error("No compatible attribute found for the given type");
        }

        const VertexFactory *factory;
        std::vector<char> dataVec;
        size_t vertexCount;
    };
    inline uint32_t swapEndian(uint32_t value)
    {
        return ((value >> 24) & 0x000000FF) |
               ((value >> 8) & 0x0000FF00) |
               ((value << 8) & 0x00FF0000) |
               ((value << 24) & 0xFF000000);
    }
    template <typename T>
    void VertexArray::setAttribute(size_t vertexIndex, const std::string &name, const T &value)
    {
        const auto &attr = factory->getAttribute(name);
        if (GetDataTypeSize(attr.dataType) != sizeof(T))
        {
            throw std::runtime_error("Attribute size mismatch");
        }
        if (vertexIndex >= vertexCount)
        {
            throw std::out_of_range("Vertex index out of range");
        }
        const size_t offset = factory->getStride() * vertexIndex + attr.offset;
        std::memcpy(dataVec.data() + offset, &value, GetDataTypeSize(attr.dataType));
    }
    /*
    template <>
    inline void VertexArray::setAttribute<int>(size_t vertexIndex, const std::string& name, const int& value) {

        const auto& attr = factory->getAttribute(name);
        if (GetDataTypeSize(attr.dataType) != sizeof(int))
        {
            throw std::runtime_error("Attribute size mismatch");
        }
        if (vertexIndex >= vertexCount)
        {
            throw std::out_of_range("Vertex index out of range");
        }
        const size_t offset = factory->getStride() * vertexIndex + attr.offset;
        //int newvalue = swapEndian(value);
        std::memcpy(dataVec.data() + offset, &newvalue, GetDataTypeSize(attr.dataType));
    }
    */
    template <typename T>
    T VertexArray::getAttribute(size_t vertexIndex, const std::string &name) const
    {
        const auto &attr = factory->getAttribute(name);
        if (GetDataTypeSize(attr.dataType) != sizeof(T))
        {
            throw std::runtime_error("Attribute size mismatch");
        }
        if (vertexIndex >= vertexCount)
        {
            throw std::out_of_range("Vertex index out of range");
        }
        const size_t offset = factory->getStride() * vertexIndex + attr.offset;
        T result;
        std::memcpy(&result, dataVec.data() + offset, GetDataTypeSize(attr.dataType));
        return result;
    }
}