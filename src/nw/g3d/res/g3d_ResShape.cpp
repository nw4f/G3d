#include <nw/g3d/res/g3d_ResShape.h>
#include <limits>
#include <nw/g3d/res/g3d_ResFile.h>

namespace nw { namespace g3d { namespace res {

void ResBuffer::Setup()
{
    GfxBuffer* vBuffer = this->GetGfxBuffer();
    vBuffer->SetData(this->GetData(), static_cast<u32>(this->GetSize()));
    vBuffer->Setup();
    vBuffer->DCFlush();
}

void ResBuffer::Cleanup()
{
    GfxBuffer* vBuffer = this->GetGfxBuffer();
    vBuffer->SetData(NULL, static_cast<u32>(this->GetSize()));
    vBuffer->Cleanup();
}

void ResVertex::Setup()
{
    for (int idxBuffer = 0, numBuffer = GetVtxBufferCount(); idxBuffer < numBuffer; ++idxBuffer)
    {
        this->GetVtxBuffer(idxBuffer)->Setup();
    }
}

void ResVertex::Cleanup()
{
    for (int idxBuffer = 0, numBuffer = GetVtxBufferCount(); idxBuffer < numBuffer; ++idxBuffer)
    {
        this->GetVtxBuffer(idxBuffer)->Cleanup();
    }
}

void ResShape::Setup()
{
    ResMesh* mesh = this->GetMesh();
    ResBuffer* buffer = mesh->GetIdxBuffer();
    GfxBuffer* idxBuffer = buffer->GetGfxBuffer();
    idxBuffer->SetData(buffer->GetData(), static_cast<u32>(buffer->GetSize()));
    idxBuffer->Setup();
    idxBuffer->DCFlush();
}

void ResShape::Cleanup()
{
    ResMesh* mesh = this->GetMesh();
    ResBuffer* buffer = mesh->GetIdxBuffer();
    GfxBuffer* idxBuffer = buffer->GetGfxBuffer();
    idxBuffer->SetData(NULL, static_cast<u32>(buffer->GetSize()));
    idxBuffer->Cleanup();
}

void ResMesh::DrawSubMesh(int submeshIndex, int submeshCount, int instances /*= 1*/) const
{
    NW_G3D_ASSERT(GX2_PRIMITIVE_POINTS <= ref().primType &&
        ref().primType <= GX2_PRIMITIVE_QUAD_STRIP);
    NW_G3D_ASSERT(submeshCount > 0);
    NW_G3D_ASSERT(instances > 0);

    SetPrimitiveRestartIndex((GetIndexFormat() & 0x1) * 0xffff0000 | 0x0000ffff);

    u32 strideShift = (GetIndexFormat() & 0x1) + 1;
    const ResSubMesh* pFirstSubMesh = GetSubMesh(submeshIndex);
    u32 offset = pFirstSubMesh->GetOffset();
    const ResSubMesh* pLastSubMesh = GetSubMesh(submeshIndex + submeshCount - 1);
    u32 count = ((pLastSubMesh->GetOffset() - offset) >> strideShift) + pLastSubMesh->GetCount();
    GX2PrimitiveType primType = GetPrimitiveType();
    GX2IndexFormat format = GetIndexFormat();
    const GfxBuffer* pIdxBuffer = GetIdxBuffer()->GetGfxBuffer();
    GX2DrawIndexedEx(primType, count, format,
        AddOffset(pIdxBuffer->GetData(), offset), 0, instances);
}

} } } // namespace nw::g3d::res
