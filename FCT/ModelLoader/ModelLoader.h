//
// Created by Administrator on 2025/4/21.
//
#include "../Context/Vertex.h"
#ifndef MODELLOADER_H
#define MODELLOADER_H
namespace FCT
{
    struct ModelVertex
    {
        ModelVertex() :  position(0.0f, 0.0f, 0.0f),
                         normal(0.0f, 0.0f, 1.0f),
                         tangent(1.0f, 0.0f, 0.0f),
                         bitangent(0.0f, 1.0f, 0.0f)
        {
            for (int i = 0; i < 8; i++) {
                texCoords[i] = Vec2(0.0f, 0.0f);
            }

            for (int i = 0; i < 8; i++) {
                colors[i] = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }
        Vec3 position;
        Vec3 normal;
        Vec2 texCoords[8];
        Vec3 tangent;
        Vec3 bitangent;
        Vec4 colors[8];
    };
    struct ModelMaterial
    {

    };
    struct ModelMesh
    {
        ModelMaterial* material;
        std::vector<ModelVertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;
    };
    struct ModelObject
    {
        std::vector<ModelObject*> children;
        ModelMesh* mesh;
    };
    struct ModelData
    {
        std::vector<ModelMaterial*> materials;
        std::vector<ModelMesh*> meshes;
        std::vector<ModelObject*> objects;
        ModelMesh* findMesh(const std::string &name)
        {
            auto it = std::find_if(meshes.begin(), meshes.end(), [&name](const ModelMesh* mesh)
            {
                return mesh->name == name;
            });

            return (it != meshes.end()) ? *it : nullptr;
        }
    };

    inline void setVertexAttributeFromModel(Vertex& vertex, size_t elementIndex,
                                    const VertexElement& element,
                                    const ModelVertex& modelVertex) {
        ModelVertexAttribute modelAttr = element.getModelAttribute();
        Format format = element.getFormat();

        if (modelAttr == ModelVertexAttribute::Position) {
            switch (format) {
            case Format::R32G32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec2(modelVertex.position.x, modelVertex.position.y));
                break;
            case Format::R32G32B32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec3(modelVertex.position.x, modelVertex.position.y, modelVertex.position.z));
                break;
            case Format::R32G32B32A32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec4(modelVertex.position.x, modelVertex.position.y, modelVertex.position.z, 1.0f));
                break;
            default:
                    break;
            }
            return;
        }

        if (modelAttr == ModelVertexAttribute::Normal) {
            switch (format) {
            case Format::R32G32B32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec3(modelVertex.normal.x, modelVertex.normal.y, modelVertex.normal.z));
                break;
            default:
                    break;
            }
            return;
        }

        if (modelAttr == ModelVertexAttribute::Tangent) {
            switch (format) {
            case Format::R32G32B32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec3(modelVertex.tangent.x, modelVertex.tangent.y, modelVertex.tangent.z));
                break;
            default:
                    break;
            }
            return;
        }

        if (modelAttr == ModelVertexAttribute::Bitangent) {
            switch (format) {
            case Format::R32G32B32_SFLOAT:
                vertex.setAttribute(elementIndex, Vec3(modelVertex.bitangent.x, modelVertex.bitangent.y, modelVertex.bitangent.z));
                break;
            default:
                    break;
            }
            return;
        }

        if (modelAttr >= ModelVertexAttribute::TexCoord0 && modelAttr <= ModelVertexAttribute::TexCoord7) {
            size_t texCoordIndex = static_cast<size_t>(modelAttr) - static_cast<size_t>(ModelVertexAttribute::TexCoord0);
            if (texCoordIndex < 8) {
                switch (format) {
                case Format::R32G32_SFLOAT:
                    vertex.setAttribute(elementIndex, Vec2(modelVertex.texCoords[texCoordIndex].x, modelVertex.texCoords[texCoordIndex].y));
                    break;
                default:
                        break;
                }
            }
            return;
        }

        if (modelAttr >= ModelVertexAttribute::Color0 && modelAttr <= ModelVertexAttribute::Color7) {
            size_t colorIndex = static_cast<size_t>(modelAttr) - static_cast<size_t>(ModelVertexAttribute::Color0);
            if (colorIndex < 8) {
                switch (format) {
                case Format::R32G32B32_SFLOAT:
                    vertex.setAttribute(elementIndex, Vec3(modelVertex.colors[colorIndex].x, modelVertex.colors[colorIndex].y, modelVertex.colors[colorIndex].z));
                    break;
                case Format::R32G32B32A32_SFLOAT:
                    vertex.setAttribute(elementIndex, Vec4(modelVertex.colors[colorIndex].x, modelVertex.colors[colorIndex].y, modelVertex.colors[colorIndex].z, modelVertex.colors[colorIndex].w));
                    break;
                case Format::R8G8B8A8_UNORM:
                    {
                        Vector4<uint8_t> colorData(
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].x * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].y * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].z * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].w * 255.0f)
                        );
                        vertex.setAttribute(elementIndex, colorData);
                    }
                    break;
                case Format::B8G8R8A8_UNORM:
                    {
                        Vector4<uint8_t> colorData(
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].z * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].y * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].x * 255.0f),
                            static_cast<uint8_t>(modelVertex.colors[colorIndex].w * 255.0f)
                        );
                        vertex.setAttribute(elementIndex, colorData);
                    }
                    break;
                default:
                        break;
                }
            }
            return;
        }
    }

    // 转换时，用一个map存ModelMaterial
    // 转换时，用一个map存ModelMesh

    class ModelLoader {
    public:
        virtual std::unique_ptr<ModelData> loadModel(const std::string &path) = 0;
    protected:

    };
}
#endif //MODELLOADER_H
