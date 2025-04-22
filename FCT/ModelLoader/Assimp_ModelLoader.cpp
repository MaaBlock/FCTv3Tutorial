#include "../FCTAPI.h"
namespace FCT
{
    std::unique_ptr<ModelData> Assimp_ModelLoader::loadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const auto aiScene = importer.ReadFile(path,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        auto ret = std::make_unique<ModelData>();
        for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
        {
            const auto aiMesh = aiScene->mMeshes[i];
            auto mesh = new ModelMesh();
            mesh->name = aiMesh->mName.C_Str();
            mesh->material = nullptr;

            mesh->vertices.resize(aiMesh->mNumVertices);

            if (aiMesh->HasPositions())
            {
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    const auto& aiVertex = aiMesh->mVertices[j];
                    mesh->vertices[j].position = { aiVertex.x, aiVertex.y, aiVertex.z };
                }
            }

            if (aiMesh->HasNormals())
            {
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    const auto& aiNormal = aiMesh->mNormals[j];
                    mesh->vertices[j].normal = { aiNormal.x, aiNormal.y, aiNormal.z };
                }
            }

            for (unsigned int texCoordSet = 0; texCoordSet < 8; texCoordSet++)
            {
                if (aiMesh->HasTextureCoords(texCoordSet))
                {
                    for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                        const auto& aiTexCoord = aiMesh->mTextureCoords[texCoordSet][j];
                        mesh->vertices[j].texCoords[texCoordSet] = { aiTexCoord.x, aiTexCoord.y };
                    }
                }
            }

            if (aiMesh->HasTangentsAndBitangents())
            {
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    const auto& aiTangent = aiMesh->mTangents[j];
                    const auto& aiBitangent = aiMesh->mBitangents[j];
                    mesh->vertices[j].tangent = { aiTangent.x, aiTangent.y, aiTangent.z };
                    mesh->vertices[j].bitangent = { aiBitangent.x, aiBitangent.y, aiBitangent.z };
                }
            }

            for (unsigned int colorSet = 0; colorSet < 8; colorSet++)
            {
                if (aiMesh->HasVertexColors(colorSet))
                {
                    for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                        const auto& aiColor = aiMesh->mColors[colorSet][j];
                        mesh->vertices[j].colors[colorSet] = { aiColor.r, aiColor.g, aiColor.b, aiColor.a };
                    }
                }
            }

            if (aiMesh->HasFaces())
            {
                mesh->indices.reserve(aiMesh->mNumFaces * 3);
                for (unsigned int j = 0; j < aiMesh->mNumFaces; j++)
                {
                    const aiFace& face = aiMesh->mFaces[j];
                    for (unsigned int k = 0; k < face.mNumIndices; k++)
                    {
                        mesh->indices.push_back(face.mIndices[k]);
                    }
                }
            }

            ret->meshes.push_back(mesh);
        }
        return ret;
    }
}