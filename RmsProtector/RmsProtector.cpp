// RmsProtector.cpp : Defines the entry point for the console application.
//

// 头文件
#include "stdafx.h"
#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include "defs.h"

#define DW_WAIT_RESULT      60 * 1000

// 回调函数
HRESULT __stdcall
StatusCallback ( DRM_STATUS_MSG msg,
                 HRESULT hr,
                 void *pvParam,
                 void *pvContext )
{
    // HRESULT          hr              = S_OK;
    PDRM_CONTEXT pContext = ( PDRM_CONTEXT ) pvContext;
    SetEvent ( pContext->hEvent );
    return S_OK;
}

HRESULT DoMachineActivation ( DRMHSESSION hClient )
{
    HRESULT         hr              = S_OK;
    DRM_CONTEXT     context;
    DWORD           dwWaitResult    = 0;

    if ( NULL == hClient )
    {
        wprintf ( L"The client session was NULL.\n" );
        hr = E_INVALIDARG;
        goto e_Exit;
    }

    // create callback event
    SecureZeroMemory ( &context, sizeof ( DRM_CONTEXT ) );
    if ( NULL == ( context.hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL ) ) )
    {
        wprintf ( L"Create event failed.\n" );
        hr = GetLastError();
        goto e_Exit;
    }

    // do machine activation
    hr = DRMActivate ( hClient,
                       DRM_ACTIVATE_MACHINE | DRM_ACTIVATE_SILENT,
                       0,
                       NULL,
                       ( VOID* ) &context,
                       NULL );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Activate machine failed. Error code: 0x%x\n", hr );
        goto e_Exit;
    }
    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        wprintf ( L"Do machine activation timed out.\n" );
        hr = ERROR_TIMEOUT;
        goto e_Exit;
    }
    if ( FAILED ( context.hr ) )
    {
        hr = context.hr;
        wprintf ( L"Activate machine failed. Callback function returned a failure code: 0x%x\n", hr );
        goto e_Exit;
    }
	wprintf(L"The machine is now activated.\n");
e_Exit:
    if ( NULL != context.hEvent )
    {
        CloseHandle ( context.hEvent );
    }
    return hr;
}

int _tmain ( int argc, _TCHAR* argv[] )
{
    // LPWSTR          wszGroupId      = L"user1@doc.lab"; // 用户ID
	LPWSTR          wszGroupId      = NULL; // 用户ID
    HRESULT         hr              = S_OK;
    DRMHSESSION     hClient         = NULL; // client session
    hr = DRMCreateClientSession ( &StatusCallback,
                                  0,
                                  DRM_DEFAULTGROUPIDTYPE_WINDOWSAUTH,
                                  wszGroupId,
                                  &hClient );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Create client session failed." );
        return hr;
    }
    // 激活计算机证书
    hr = DRMIsActivated ( hClient, DRM_ACTIVATE_MACHINE, NULL );
    if ( FAILED ( hr ) )
    {
        if ( hr == E_DRM_NEEDS_MACHINE_ACTIVATION )
        {
            DoMachineActivation ( hClient );
        }
        else
        {
            hr = GetLastError();
            wprintf ( L"Unexcepted error: 0x%x", hr );
            return hr;
        }
    }
    else
    {
        wprintf ( L"The machine is already activated.\n" );
    }
    return 0;
}

