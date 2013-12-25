#ifndef __FUNCTIONS_H_
#define __FUNCTIONS_H_

#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>

// 定义
#define     DW_WAIT_RESULT      60 * 1000

// 类型定义
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

// 回调函数
HRESULT __stdcall
StatusCallback ( DRM_STATUS_MSG msg,
                 HRESULT hr,
                 void *pvParam,
                 void *pvContext );

// 以双字节方式读取文件
HRESULT ReadManifestToWideString ( PWSTR *pwszFileString );

// 服务地址发现
HRESULT FindServiceURL ( DRMHSESSION hClient,
                         UINT uiServiceType,
                         UINT uiServiceLocation,
                         PWSTR *pwszServiceUrl );

// Activate machine
HRESULT DoMachineActivation ( DRMHSESSION hClient );

// Activate User
HRESULT DoUserActivation ( DRMHSESSION hClient );
#endif