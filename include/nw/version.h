#ifndef NW_VERSION_H_
#define NW_VERSION_H_

#define NW_VERSION_MAJOR          1
#define NW_VERSION_MINOR          5
#define NW_VERSION_MICRO          0
#define NW_VERSION_RELSTEP        0

#ifndef NW_VERSION_NUMBER
#define NW_VERSION_NUMBER(major, minor, micro, relstep) (((major) << 24) | ((minor) << 16) | ((micro) << 8) | ((relstep) << 0))

#define NW_CURRENT_VERSION_NUMBER NW_VERSION_NUMBER(NW_VERSION_MAJOR, NW_VERSION_MINOR, NW_VERSION_MICRO, NW_VERSION_RELSTEP )
#endif

#endif // NW_VERSION_H_
