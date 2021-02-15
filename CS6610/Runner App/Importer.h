#pragma once
#include "Mesh3D.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glm\vec3.hpp>
#include <vector>

namespace Importer
{
	namespace Mesh
	{
		static Mesh3D* ImportByFileName(const char* i_fileName)
        {
            Assimp::Importer importer;
            std::string meshFilePath(ASSET_DIR);
            meshFilePath.append("Models\\");
            meshFilePath.append(i_fileName);

            const auto pModel = importer.ReadFile(meshFilePath,
                aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
            printf(importer.GetErrorString());
            //Select the first mesh for now
            const auto pMesh = pModel->mMeshes[0];
            std::vector<Vertex> vertices;
            vertices.reserve(pMesh->mNumVertices);

            for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
            {
                vertices.push_back(
                    {
                        //Push position data
                        {
                            pMesh->mVertices[i].x,
                            pMesh->mVertices[i].y,
                            pMesh->mVertices[i].z
                        }
                    }
                );
            }

            std::vector<unsigned short> indices;
            indices.reserve(pMesh->mNumFaces * 3);

            for (unsigned int i = 0; i < pMesh->mNumFaces; ++i)
            {
                const auto& face = pMesh->mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            return new Mesh3D(vertices,indices);
        }

	}
}

