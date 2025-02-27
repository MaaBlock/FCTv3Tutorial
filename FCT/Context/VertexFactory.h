#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include "../MutilThreadBase/RefCount.h"
#include "./DataTypes.h"


namespace FCT {


    struct VertexAttribute {
        PipelineAttributeType type;
        std::string name;
        DataType dataType;
        size_t offset;
        bool flat;

        VertexAttribute(PipelineAttributeType t, const std::string& n, DataType dt, size_t o)
            : type(t), name(n), dataType(dt), offset(o) {
            flat = false;
        }
        VertexAttribute(PipelineAttributeType t, const std::string& n, DataType dt, size_t o,bool f)
            : type(t), name(n), dataType(dt), offset(o),flat(f) {

        }
    };

    class VertexFactory;
    class VertexData;
    class VertexArray;

    class VertexData {
    public:
        VertexData(const VertexFactory& factory);

        ~VertexData() {
            delete[] m_data;
        }

        VertexData(const VertexData&) = delete;
        VertexData& operator=(const VertexData&) = delete;
        VertexData(VertexData&&) = delete;
        VertexData& operator=(VertexData&&) = delete;

        template<typename T>
        void setAttribute(const std::string& name, const T& value);
        template<typename T>
        T getAttribute(const std::string& name) const;

        void* data() { return m_data; }
        const void* data() const { return m_data; }
        size_t getSize() const { return m_dataSize; }

    private:
        const VertexFactory* m_factory;
        char* m_data;
        size_t m_dataSize;

        friend class VertexFactory;
    };

    class VertexFactory : public RefCount {
    public:
        VertexFactory() = default;

        void addAttribute(PipelineAttributeType type, const std::string& name = "", DataType dataType = DataType::Float,bool flat = false);

        const std::vector<VertexAttribute>& getAttributes() const {
            return attributes;
        }

        size_t getStride() const;

        VertexData* createVertex() const;

        template<typename T>
        void setAttributeData(VertexData& vertexData, const std::string& name, const T& value) const;

        void setVertexData(VertexData& vertexData, const void* data, size_t dataSize) const;

        const VertexAttribute& getAttribute(const std::string& name) const;
    private:
        std::vector<VertexAttribute> attributes;
    };
    template<typename T>
    void VertexData::setAttribute(const std::string& name, const T& value) {
        const auto& attr = m_factory->getAttribute(name);
        if (GetDataTypeSize(attr.dataType) != sizeof(T)) {
            throw std::runtime_error("Attribute size mismatch");
        }
        std::memcpy(static_cast<char*>(m_data) + attr.offset, &value, GetDataTypeSize(attr.dataType));
    }

    template<typename T>
    T VertexData::getAttribute(const std::string& name) const {
        const auto& attr = m_factory->getAttribute(name);
        if (GetDataTypeSize(attr.dataType) != sizeof(T)) {
            throw std::runtime_error("Attribute size mismatch");
        }
        T result;
        std::memcpy(&result, static_cast<const char*>(m_data) + attr.offset, GetDataTypeSize(attr.dataType));
        return result;
    }

    template<typename T>
    void VertexFactory::setAttributeData(VertexData& vertexData, const std::string& name, const T& value) const {
        for (const auto& attr : attributes) {
            if (attr.name == name) {
                if (GetDataTypeSize(attr.dataType) != sizeof(T)) {
                    throw std::runtime_error("属性大小不匹配");
                }
                std::memcpy((char*)vertexData.data() + attr.offset, &value, GetDataTypeSize(attr.dataType));
                return;
            }
        }
        throw std::runtime_error("未找到属性");
    }

} // namespace FCT