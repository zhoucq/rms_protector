#ifndef __DEFS_H_
#define __DEFS_H_

#define     DW_WAIT_RESULT      60 * 1000

const UINT  GUID_LENGTH         = 128;  // GUID长度

// 类型定义
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

#endif