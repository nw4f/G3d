#ifndef NW_G3D_RES_BINDING_H_
#define NW_G3D_RES_BINDING_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace res {

enum BindTarget
{
    BIND_TEXTURE                    = 0x1 << 0,

    BIND_FUNCTION                   = 0x1 << 0
};

class BindResult
{
public:
    BindResult() : flag(0) {}

    static BindResult Bound(bit32 mask = MASK_ALL)
    {
        NW_G3D_ASSERT(mask <= MASK_ALL);
        return BindResult(mask << SHIFT_SUCCESS);
    }

    static BindResult NotBound(bit32 mask = MASK_ALL)
    {
        NW_G3D_ASSERT(mask <= MASK_ALL);
        return BindResult(mask << SHIFT_FAILURE);
    }

    bool IsComplete(bit32 mask = MASK_ALL)
    {
        NW_G3D_ASSERT(mask <= MASK_ALL);
        return 0 == ((flag >> SHIFT_FAILURE) & mask);
    }

    bool IsMissed(bit32 mask = MASK_ALL)
    {
        NW_G3D_ASSERT(mask <= MASK_ALL);
        return 0 == ((flag >> SHIFT_SUCCESS) & mask);
    }

    const BindResult operator|(BindResult rhs) const { return BindResult(*this) |= rhs; }

    BindResult& operator|=(BindResult rhs)
    {
        flag |= rhs.flag;
        return *this;
    }

private:
    enum
    {
        SHIFT_FAILURE   = 0,
        SHIFT_SUCCESS   = 16,
        MASK_ALL        = 0xFFFF
    };

    explicit BindResult(bit32 flag) : flag(flag) {}

    bit32 flag;
};

} } } // namespace nw::g3d

#endif // NW_G3D_RES_BINDING_H_
