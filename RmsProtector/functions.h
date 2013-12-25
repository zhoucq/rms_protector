#ifndef __FUNCTIONS_H_
#define __FUNCTIONS_H_

#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>

// ����
#define     DW_WAIT_RESULT      60 * 1000

// ���Ͷ���
typedef struct Drm_Context
{
    HANDLE  hEvent;
    HRESULT hr;
    PWSTR   wszData;
} DRM_CONTEXT, *PDRM_CONTEXT;

// �ص�����
HRESULT __stdcall
StatusCallback ( DRM_STATUS_MSG msg,
                 HRESULT hr,
                 void *pvParam,
                 void *pvContext );

// ��˫�ֽڷ�ʽ��ȡ�ļ�
HRESULT ReadManifestToWideString ( PWSTR *pwszFileString );

// �����ַ����
HRESULT FindServiceURL ( DRMHSESSION hClient,
                         UINT uiServiceType,
                         UINT uiServiceLocation,
                         PWSTR *pwszServiceUrl );

// Activate machine
HRESULT DoMachineActivation ( DRMHSESSION hClient );

// Activate User
HRESULT DoUserActivation ( DRMHSESSION hClient );
#endif