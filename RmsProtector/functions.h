#ifndef __FUNCTIONS_H_
#define __FUNCTIONS_H_

#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>
#include "defs.h"

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
HRESULT GetServiceLocation ( DRMHSESSION hClient,
                             UINT uiServiceType,
                             UINT uiServiceLocation,
                             PWSTR *pwszServiceUrl );

// Activate machine
HRESULT DoMachineActivation ( DRMHSESSION hClient );

// Activate User
HRESULT DoUserActivation ( DRMHSESSION hClient );

// Get certificate
HRESULT GetCertificate ( DRMHSESSION hClient,
                         UINT uiCertFlag,
                         PWSTR *pwszCertificate );

// TODO: 合并依赖DRMAcquire的方法
// Acquire CLC
HRESULT
DoAcquireClientLicensorCertificate ( DRMHSESSION hClient,
                                     PWSTR wszLicensingSvr,
                                     PWSTR *wszClientLicensorCert );

HRESULT GetOfflineSignedIL_ ( DRMENVHANDLE hEnv,
                              DRMHANDLE hLib,
                              PWSTR pwszUserID,
                              PWSTR pwszMachineCert,
                              PWSTR pwszCLC,
                              PWSTR pwszManifest,
                              PWSTR *ppwszGUID,
                              DRMPUBHANDLE *phIssuanceLic,
                              PWSTR *ppwszSignedIL );

// 生成Unsigned IL
HRESULT GetUnsignedIL ( PWSTR wszOwner,
                        PWSTR wszUserName,
                        PWSTR *ppwszGUID,
                        DRMPUBHANDLE *phIssuanceLic );

// 离线签署IL
HRESULT GetOfflineSignedIL ( DRMHANDLE *phIssuanceLicense,
                             PWSTR pwszCLC,
                             PWSTR *ppwszSignedIL );

// 连线签署IL
HRESULT GetOnlineSignedIL ( DRMPUBHANDLE *phIssuanceLic,
                            PWSTR wszLicensingSrv ,
                            PWSTR *pwszSignedIL );

// 文档加密
HRESULT EncryptContent ( DRMENVHANDLE hEnv,
                         DRMHANDLE hLib,
                         PWSTR pwszRAC,
                         PWSTR pwszGUID,
                         DRMHANDLE hIssuanceLic,
                         PWSTR pwszSignedIL,
                         BYTE** ppbEncrypted );
#endif