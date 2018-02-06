//==============================================================================
// Joe Schutte
//==============================================================================

#include "ImportScene.h"

#include <MathLib/FloatFuncs.h>
#include <SystemLib/MemoryAllocation.h>

// -- middleware
#include "assimp/Importer.hpp"
#include "assimp/Scene.h"
#include "assimp/postprocess.h"

namespace Shooty {

    #define ReturnFailure_(x) if(!x) { return false; }

    //==============================================================================
    bool CountMeshes(const aiScene* aiscene, const struct aiNode* node, uint& count);
    bool ExtractMeshes(const aiScene* aiscene, const struct aiNode* node, ImportedScene* mesh);
    bool ExtractMeshes(const aiScene* aiscene, const aiNode* node, ImportedScene* scene, uint& mesh_index);

    //==============================================================================
    bool ImportScene(const char* filename, ImportedScene* scene) {
        Assimp::Importer importer;
        const aiScene* aiscene = importer.ReadFile(filename, aiProcess_GenNormals
                                                   | aiProcess_CalcTangentSpace
                                                   | aiProcess_GenUVCoords
                                                   | aiProcess_ConvertToLeftHanded
                                                   | aiProcess_Triangulate
                                                   | aiProcess_JoinIdenticalVertices
                                                   | aiProcess_PreTransformVertices
                                                   | aiProcess_TransformUVCoords
                                                   | aiProcess_ImproveCacheLocality
                                                   | aiProcess_OptimizeMeshes
                                                   | aiProcess_OptimizeGraph);
        if (!aiscene) {
            const char* errstr = importer.GetErrorString();
            (void)errstr; // JSTODO - Error reporting
            return false;
        }

        uint meshcount = 0;
        ReturnFailure_(CountMeshes(aiscene, aiscene->mRootNode, meshcount));

        scene->meshes.Resize((uint32)meshcount);
        for (uint scan = 0; scan < meshcount; ++scan) {
            scene->meshes[scan] = New_(ImportedMesh);
        }

        uint mesh_index = 0;
        ReturnFailure_(ExtractMeshes(aiscene, aiscene->mRootNode, scene, mesh_index));

        // aiscene is cleaned up by the importer's destructor
        return true;
    }

    //==============================================================================
    void ShutdownImportedScene(ImportedScene* scene) {
        for (uint scan = 0, meshcount = scene->meshes.Length(); scan < meshcount; ++scan) {
            Delete_(scene->meshes[scan]);
        }

        scene->meshes.Close();
    }

    //==============================================================================
    bool CountMeshes(const aiScene* aiscene, const aiNode* node, uint& count) {
        count += node->mNumMeshes;

        for (uint scan = 0; scan < node->mNumChildren; ++scan) {
            ReturnFailure_(CountMeshes(aiscene, node->mChildren[scan], count));
        }

        return true;
    }

    //==============================================================================
    static void ExtractTriangles(const aiFace* face, CArray<uint32>& indices) {
        Assert_(face->mNumIndices == 3);
        indices.Add(face->mIndices[0]);
        indices.Add(face->mIndices[1]);
        indices.Add(face->mIndices[2]);
    }

    //==============================================================================
    static void ExtractQuad(const aiFace* face, CArray<uint32>& indices) {
        Assert_(face->mNumIndices == 4);

        Error_("Not tested");
        indices.Add(face->mIndices[0]);
        indices.Add(face->mIndices[1]);
        indices.Add(face->mIndices[2]);
        indices.Add(face->mIndices[0]);
        indices.Add(face->mIndices[2]);
        indices.Add(face->mIndices[3]);
    }

    //==============================================================================
    bool ExtractMeshes(const aiScene* aiscene, const aiNode* node, ImportedScene* scene, uint& meshIndex) {
        for (uint meshscan = 0, meshcount = node->mNumMeshes; meshscan < meshcount; ++meshscan) {
            const struct aiMesh* aimesh = aiscene->mMeshes[node->mMeshes[meshscan]];

            ImportedMesh* mesh = scene->meshes[meshIndex++];

            // -- extract vertices
            uint vertexcount = aimesh->mNumVertices;
            mesh->positions.Resize((uint32)vertexcount);
            for (uint scan = 0; scan < vertexcount; ++scan) {
                mesh->positions[scan].x = aimesh->mVertices[scan].x;
                mesh->positions[scan].y = aimesh->mVertices[scan].y;
                mesh->positions[scan].z = aimesh->mVertices[scan].z;
                mesh->positions[scan].w = 1.0f;
            }

            // -- extract normals
            if (aimesh->HasNormals()) {
                mesh->normals.Resize((uint32)vertexcount);
                for (uint scan = 0; scan < vertexcount; ++scan) {
                    mesh->normals[scan].x = aimesh->mNormals[scan].x;
                    mesh->normals[scan].y = aimesh->mNormals[scan].y;
                    mesh->normals[scan].z = aimesh->mNormals[scan].z;
                }
            }

            // -- extract uvs
            if (aimesh->HasTextureCoords(0)) {
                mesh->uv0.Resize((uint32)vertexcount);
                for (uint scan = 0; scan < vertexcount; ++scan) {
                    mesh->uv0[scan].x = aimesh->mTextureCoords[0][scan].x;
                    mesh->uv0[scan].y = aimesh->mTextureCoords[0][scan].y;
                }
            }

            // -- extract indices
            mesh->indices.Reserve((uint32)aimesh->mNumFaces * 3);
            for (uint facescan = 0, facecount = aimesh->mNumFaces; facescan < facecount; ++facescan) {
                const struct aiFace* face = &aimesh->mFaces[facescan];

                if (face->mNumIndices == 3) {
                    ExtractTriangles(face, mesh->indices);
                }
                else if (face->mNumIndices == 4) {
                    ExtractQuad(face, mesh->indices);
                }
                else {
                    return false;
                }
            }
        }

        for (uint scan = 0; scan < node->mNumChildren; ++scan) {
            ReturnFailure_(ExtractMeshes(aiscene, node->mChildren[scan], scene, meshIndex));
        }

        return true;
    }
}