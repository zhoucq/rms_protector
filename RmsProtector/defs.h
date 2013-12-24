#ifndef __DEFS_H_
#define __DEFS_H_

#include <windows.h>
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

#endif