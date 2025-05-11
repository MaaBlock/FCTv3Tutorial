//
// Created by Administrator on 2025/5/11.
//

#ifndef PRIMITIVES_H
#define PRIMITIVES_H
namespace FCT
{
    namespace Primitives
    {
        enum class VertexMode {
            Shared,
            PerFace,
            Auto
        };

        enum class TextureMode {
            Single,
            PerFace,
            Auto
        };
        template<typename IndexType = uint16_t>
static StaticMesh<IndexType>* createCube(Context* ctx, float size = 1.0f,
    const VertexLayout& layout = VertexLayout{
        VertexElement{VtxType::Position3f},
        VertexElement{VtxType::Color4f},
        VertexElement{VtxType::TexCoord2f}
    })
        {
            StaticMesh<IndexType>* mesh = new StaticMesh<IndexType>(ctx, layout);

            float halfSize = size * 0.5f;

            Vec3 vertices[8] = {
                Vec3(-halfSize, -halfSize, -halfSize),
                Vec3(halfSize, -halfSize, -halfSize),
                Vec3(halfSize, halfSize, -halfSize),
                Vec3(-halfSize, halfSize, -halfSize),
                Vec3(-halfSize, -halfSize, halfSize),
                Vec3(halfSize, -halfSize, halfSize),
                Vec3(halfSize, halfSize, halfSize),
                Vec3(-halfSize, halfSize, halfSize)
            };

            Vec4 colors[6] = {
                Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                Vec4(0.0f, 1.0f, 0.0f, 1.0f),
                Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                Vec4(1.0f, 1.0f, 0.0f, 1.0f),
                Vec4(1.0f, 0.0f, 1.0f, 1.0f),
                Vec4(0.0f, 1.0f, 1.0f, 1.0f)
            };

            Vec2 texCoords[4] = {
                Vec2(0.0f, 1.0f),
                Vec2(1.0f, 1.0f),
                Vec2(1.0f, 0.0f),
                Vec2(0.0f, 0.0f)
            };

            mesh->addVertex(vertices[4], colors[0], texCoords[0]);
            mesh->addVertex(vertices[5], colors[0], texCoords[1]);
            mesh->addVertex(vertices[6], colors[0], texCoords[2]);
            mesh->addVertex(vertices[7], colors[0], texCoords[3]);

            mesh->addVertex(vertices[0], colors[1], texCoords[1]);
            mesh->addVertex(vertices[1], colors[1], texCoords[0]);
            mesh->addVertex(vertices[2], colors[1], texCoords[3]);
            mesh->addVertex(vertices[3], colors[1], texCoords[2]);

            mesh->addVertex(vertices[3], colors[2], texCoords[0]);
            mesh->addVertex(vertices[2], colors[2], texCoords[1]);
            mesh->addVertex(vertices[6], colors[2], texCoords[2]);
            mesh->addVertex(vertices[7], colors[2], texCoords[3]);

            mesh->addVertex(vertices[0], colors[3], texCoords[0]);
            mesh->addVertex(vertices[1], colors[3], texCoords[1]);
            mesh->addVertex(vertices[5], colors[3], texCoords[2]);
            mesh->addVertex(vertices[4], colors[3], texCoords[3]);

            mesh->addVertex(vertices[1], colors[4], texCoords[0]);
            mesh->addVertex(vertices[2], colors[4], texCoords[1]);
            mesh->addVertex(vertices[6], colors[4], texCoords[2]);
            mesh->addVertex(vertices[5], colors[4], texCoords[3]);

            mesh->addVertex(vertices[0], colors[5], texCoords[0]);
            mesh->addVertex(vertices[3], colors[5], texCoords[1]);
            mesh->addVertex(vertices[7], colors[5], texCoords[2]);
            mesh->addVertex(vertices[4], colors[5], texCoords[3]);

            mesh->setIndices({
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                8, 9, 10, 10, 11, 8,
                12, 13, 14, 14, 15, 12,
                16, 17, 18, 18, 19, 16,
                20, 21, 22, 22, 23, 20
            });

            mesh->create();

            return mesh;
        }

        template<typename IndexType = uint16_t>
        static StaticMesh<IndexType>* createSphere(Context* ctx, float radius = 1.0f, int segments = 16,
            const VertexLayout& layout = VertexLayout{
                VertexElement{VtxType::Position3f},
                VertexElement{VtxType::Color4f},
                VertexElement{VtxType::TexCoord2f}
            })
        {
            StaticMesh<IndexType>* mesh = new StaticMesh<IndexType>(ctx, layout);

            segments = std::max(3, segments);

            std::vector<Vec3> positions;
            std::vector<Vec4> colors;
            std::vector<Vec2> texCoords;
            std::vector<IndexType> indices;

            for (int y = 0; y <= segments; y++) {
                for (int x = 0; x <= segments; x++) {
                    float xSegment = (float)x / (float)segments;
                    float ySegment = (float)y / (float)segments;
                    float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
                    float yPos = std::cos(ySegment * M_PI);
                    float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

                    positions.push_back(Vec3(xPos, yPos, zPos) * radius);
                    colors.push_back(Vec4(xSegment, ySegment, 1.0f - ySegment, 1.0f));
                    texCoords.push_back(Vec2(xSegment, ySegment));
                }
            }

            for (int y = 0; y < segments; y++) {
                for (int x = 0; x < segments; x++) {
                    IndexType first = (y * (segments + 1)) + x;
                    IndexType second = first + segments + 1;

                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);

                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
            }

            for (size_t i = 0; i < positions.size(); i++) {
                mesh->addVertex(positions[i], colors[i], texCoords[i]);
            }

            mesh->setIndices(indices);
            mesh->create();

            return mesh;
        }
    }
}
#endif //PRIMITIVES_H
