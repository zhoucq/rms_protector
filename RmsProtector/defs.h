#ifndef __DEFS_H_
#define __DEFS_H_

#define     DW_WAIT_RESULT      60 * 1000

// ���Ͷ���
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

#endif