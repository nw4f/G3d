#include <nw/g3d/res/g3d_ResSkeleton.h>

namespace nw { namespace g3d { namespace res {

int ResSkeleton::GetBranchEndIndex(int boneIndex) const
{
    int numBone = GetBoneCount();
    int branchParentIndex = GetBone(boneIndex)->GetParentIndex();
    if (boneIndex == 0)
    {
        branchParentIndex = -1;
    }
    for (++boneIndex; boneIndex < numBone; ++boneIndex)
    {
        if (GetBone(boneIndex)->GetParentIndex() <= branchParentIndex)
        {
            return boneIndex;
        }
    }
    return boneIndex;
}

} } } // namespace nw::g3d::res
