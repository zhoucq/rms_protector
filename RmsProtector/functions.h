#ifndef __FUNCTIONS_H_
#define __FUNCTIONS_H_

#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>

// 类型定义
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

// UTILS
// Convert String To SYSTEMTIME
BOOL ConvertStringToSystemTime ( PWSTR pwszTimeString,
                                 SYSTEMTIME *stTime );

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

// 首先创建Unsigned IL，添加用户权限后，使用CLC进行签署
HRESULT GetOfflineSignedIL ( DRMENVHANDLE hEnv,
                             DRMHANDLE hLib,
                             PWSTR pwszUserID,
                             PWSTR pwszMachineCert,
                             PWSTR pwszCLC,
                             PWSTR pwszManifest,
                             PWSTR *ppwszGUID,
                             DRMPUBHANDLE *phIssuanceLic,
                             PWSTR *ppwszSignedIL );
#endif