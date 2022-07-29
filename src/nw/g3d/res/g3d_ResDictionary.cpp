#include <nw/g3d/res/g3d_ResDictionary.h>
#include <algorithm>
#include <limits.h>

namespace nw { namespace g3d { namespace res {

namespace {

u32 GetBit(const char* str, size_t len, u32 refBit)
{
    u32 wd = (refBit >> 3);
    u32 pos = (refBit & 7);
    return wd < len ? (str[wd] >> pos) & 0x1 : 0;
}

u32 GetBit(const ResName* name, u32 refBit)
{
    return GetBit(name->GetName(), name->GetLength(), refBit);
}

}

void* ResDicPatricia::Find(const char* str) const
{
    if (this != NULL)
    {
        NW_G3D_ASSERT_NOT_NULL(str);
        Node* pNode = FindNode(str, strlen(str));
        if (pNode)
        {
            return pNode->ofsData.to_ptr();
        }
    }

    return NULL;
}

void* ResDicPatricia::Find(const ResName* name) const
{
    if (this != NULL)
    {
        NW_G3D_ASSERT_NOT_NULL(name);
        Node* pNode = FindNode(name);
        if (pNode)
        {
            return pNode->ofsData.to_ptr();
        }
    }

    return NULL;
}

int ResDicPatricia::FindIndex(const char* str) const
{
    if (this != NULL)
    {
        NW_G3D_ASSERT_NOT_NULL(str);
        const Node* pNode = FindNode(str, strlen(str));
        if (pNode)
        {
            return static_cast<int>(std::distance(ref().node + 1, pNode));
        }
    }

    return -1;
}

int ResDicPatricia::FindIndex(const ResName* name) const
{
    if (this != NULL)
    {
        NW_G3D_ASSERT_NOT_NULL(name);
        const Node* pNode = FindNode(name);
        if (pNode)
        {
            return static_cast<int>(std::distance(ref().node + 1, pNode));
        }
    }

    return -1;
}

ResDicPatricia::Node* ResDicPatricia::FindNode(const char* str, size_t len) const
{
    const Node* node = ref().node;
    const Node* parent = &node[0];
    const Node* child = &node[parent->idxLeft];

    while(parent->refBit > child->refBit)
    {
        parent = child;
        child = GetBit(str, len, child->refBit) ? &node[child->idxRight] : &node[child->idxLeft];
    }

    if (child->ofsName.offset && child->ofsName.GetResName()->Equals(str, len))
    {
        return const_cast<Node*>(child);
    }
    else
    {
        return NULL;
    }
}

ResDicPatricia::Node* ResDicPatricia::FindNode(const ResName* name) const
{
    const Node* node = ref().node;
    const Node* parent = &node[0];
    const Node* child = &node[parent->idxLeft];

    while(parent->refBit > child->refBit)
    {
        parent = child;
        child = GetBit(name, child->refBit) ? &node[child->idxRight] : &node[child->idxLeft];
    }

    if (child->ofsName.offset && child->ofsName.GetResName()->Equals(name))
    {
        return const_cast<Node*>(child);
    }
    else
    {
        return NULL;
    }
}

} } } // namespace nw::g3d::res
