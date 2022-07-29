#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_ResFile.h>

namespace nw { namespace g3d { namespace res {

BindResult ResModel::Bind(const ResFile* pFile)
{
    NW_G3D_ASSERT_NOT_NULL(pFile);

    BindResult result;

    for (int idxMat = 0, numMat = GetMaterialCount(); idxMat < numMat; ++idxMat)
    {
        ResMaterial* pMaterial = GetMaterial(idxMat);
        result |= pMaterial->Bind(pFile);
    }

    return result;
}

void ResModel::Setup()
{
    int numMaterial = this->GetMaterialCount();
    for (int i = 0; i < numMaterial; ++i)
    {
        this->GetMaterial(i)->Setup();
    }

    int numShape = this->GetShapeCount();
    for (int i = 0; i < numShape; ++i)
    {
        this->GetShape(i)->Setup();
    }

    int numVertex = this->GetVertexCount();
    for (int i = 0; i < numVertex; ++i)
    {
        this->GetVertex(i)->Setup();
    }
}

void ResModel::Cleanup()
{
    int numMaterial = this->GetMaterialCount();
    for (int i = 0; i < numMaterial; ++i)
    {
        this->GetMaterial(i)->Cleanup();
    }

    int numShape = this->GetShapeCount();
    for (int i = 0; i < numShape; ++i)
    {
        this->GetShape(i)->Cleanup();
    }

    int numVertex = this->GetVertexCount();
    for (int i = 0; i < numVertex; ++i)
    {
        this->GetVertex(i)->Cleanup();
    }
}

} } } // namespace nw::g3d::res
