//=================================================================================================================================
// Joe Schutte
//=================================================================================================================================

#include "BuildCommon/BakeModel.h"
#include "BuildCore/BuildContext.h"
#include "SceneLib/ModelResource.h"

namespace Selas
{
    //=============================================================================================================================
    Error BakeModel(BuildProcessorContext* context, const BuiltModel& model)
    {
        ModelResourceData data;
        data.aaBox                = model.aaBox;
        data.boundingSphere       = model.boundingSphere;
        data.cameraCount          = (uint32)model.cameras.Count();
        data.meshCount            = (uint32)model.meshes.Count();
        data.totalVertexCount     = (uint32)model.positions.Count();
        data.indexCount           = (uint32)model.indices.Count();
        data.textureCount         = (uint32)model.textures.Count();
        data.materialCount        = (uint32)model.materials.Count();
        data.cameras              = (CameraSettings*)model.cameras.DataPointer();
        data.textureResourceNames = (FilePathString*)model.textures.DataPointer();
        data.materials            = (Material*)model.materials.DataPointer();
        data.materialHashes       = (Hash32*)model.materialHashes.DataPointer();
        data.meshData             = (MeshMetaData*)model.meshes.DataPointer();
        
        context->CreateOutput(ModelResource::kDataType, ModelResource::kDataVersion, context->source.name.Ascii(), data);

        ModelGeometryData geometry;
        geometry.indexSize       = model.indices.DataSize();
        geometry.faceIndexSize   = model.faceIndexCounts.DataSize();
        geometry.positionSize    = model.positions.DataSize();
        geometry.normalsSize     = model.normals.DataSize();
        geometry.tangentsSize    = model.tangents.DataSize();
        geometry.uvsSize         = model.uvs.DataSize();
        geometry.indices         = (uint32*)model.indices.DataPointer();
        geometry.faceIndexCounts = (uint32*)model.faceIndexCounts.DataPointer();
        geometry.positions       = (float3*)model.positions.DataPointer();
        geometry.normals         = (float3*)model.normals.DataPointer();
        geometry.tangents        = (float4*)model.tangents.DataPointer();
        geometry.uvs             = (float2*)model.uvs.DataPointer();

        context->CreateOutput(ModelResource::kGeometryDataType, ModelResource::kDataVersion, context->source.name.Ascii(),
                              geometry);

        return Success_;
    }
}