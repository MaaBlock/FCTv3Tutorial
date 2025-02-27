#include "../headers.h"

void FCT::VertexFactory::addAttribute(PipelineAttributeType type, const std::string& name, DataType dataType, bool flat)
{
    std::string defaultName;

    switch (type) {
    case PipelineAttributeType::Position2f:
        dataType = DataType::Vec2;
        defaultName = "position";
        break;
    case PipelineAttributeType::Position3f:
        dataType = DataType::Vec3;
        defaultName = "position";
        break;
    case PipelineAttributeType::Position4f:
        dataType = DataType::Vec4;
        defaultName = "position";
        break;
    case PipelineAttributeType::Color4f:
        dataType = DataType::Vec4;
        defaultName = "color";
        break;
    case PipelineAttributeType::TexCoord2f:
        dataType = DataType::Vec2;
        defaultName = "texCoord";
        break;
    case PipelineAttributeType::Normal3f:
        dataType = DataType::Vec3;
        defaultName = "normal";
        break;
    case PipelineAttributeType::Tangent3f:
        dataType = DataType::Vec3;
        defaultName = "tangent";
        break;
    case PipelineAttributeType::Bitangent3f:
        dataType = DataType::Vec3;
        defaultName = "bitangent";
        break;
    case PipelineAttributeType::TextureId:
        dataType = DataType::Float;
        defaultName = "textureId";
        break;
    case PipelineAttributeType::Custom:
        defaultName = "custom";
        break;
    }

    size_t offset = getStride();
    attributes.emplace_back(type, name.empty() ? defaultName : name, dataType, offset, flat);
}

size_t FCT::VertexFactory::getStride() const
{
    size_t stride = 0;
    for (const auto& attr : attributes) {
        stride += GetDataTypeSize(attr.dataType);
    }
    return stride;
}

FCT::VertexData* FCT::VertexFactory::createVertex() const
{
    return new VertexData(*this);
}

void FCT::VertexFactory::setVertexData(VertexData& vertexData, const void* data, size_t dataSize) const
{
    if (dataSize != getStride()) {
        throw std::runtime_error("数据大小与顶点步长不匹配");
    }
    std::memcpy(vertexData.data(), data, dataSize);
}

const FCT::VertexAttribute& FCT::VertexFactory::getAttribute(const std::string& name) const
{
    for (const auto& attr : attributes) {
        if (attr.name == name) {
            return attr;
        }
    }
    throw std::runtime_error("未找到属性");
}

FCT::VertexData::VertexData(const VertexFactory& factory)
{
    m_factory = &factory;
    m_dataSize = factory.getStride();
    m_data = new char[m_dataSize];
    if (!m_data) throw std::bad_alloc();
}