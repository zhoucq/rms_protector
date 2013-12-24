// RmsProtector.cpp : Defines the entry point for the console application.
//

// 头文件
#include "stdafx.h"
#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>
#include "defs.h"


#define     DW_WAIT_RESULT      60 * 1000

// 回调函数
HRESULT __stdcall
StatusCallback ( DRM_STATUS_MSG msg,
                 HRESULT hr,
                 void *pvParam,
                 void *pvContext )
{
    // HRESULT          hr              = S_OK;
    UINT            uiSignedILLength    = 0;
    PDRM_CONTEXT    pContext            = ( PDRM_CONTEXT ) pvContext;
    if ( pContext )
    {
        pContext->hr = hr;
    }
    else
    {
        return E_FAIL;
    }
    if ( FAILED ( pContext->hr ) && pContext->hEvent )
    {
        SetEvent ( pContext->hEvent );
        return S_OK;
    }
    switch ( pContext->hr )
    {
        case S_DRM_ALREADY_ACTIVATED:
            wprintf ( L"Callback hr = S_DRM_ALREADY_ACTIVATED\n" );
            break;
        case S_DRM_CONNECTING:
            wprintf ( L"Callback hr = S_DRM_CONNECTING\n" );
            break;
        case S_DRM_CONNECTED:
            wprintf ( L"Callback hr = S_DRM_CONNECTED\n" );
            break;
        case S_DRM_INPROGRESS:
            wprintf ( L"Callback hr = S_DRM_INPROGRESS\n" );
            break;
        case S_DRM_COMPLETED:
            wprintf ( L"Callback hr = S_DRM_COMPLETED\n" );
            pContext->hr = S_OK;
            if ( ( msg == DRM_MSG_SIGN_ISSUANCE_LICENSE ) && pvParam )
            {
                if ( NULL != pvParam )
                {
                    //
                    // Retrieve the length of the signed issuance license
                    //
                    hr = StringCchLengthW ( ( PWSTR ) pvParam,
                                            STRSAFE_MAX_CCH,
                                            &uiSignedILLength
                                          );
                    if ( FAILED ( hr ) )
                    {
                        wprintf ( L"\nStringCchLengthW failed.  hr = 0x%x\n", hr );
                        break;
                    }
                    //
                    // Allocate memory for the signed issuance license
                    //
                    pContext->wszData = new WCHAR[ uiSignedILLength + 1 ];
                    if ( NULL == pContext->wszData )
                    {
                        wprintf ( L"\nFailed to allocate memory "\
                                  L"for the signed issuance license.\n" );
                        hr = E_OUTOFMEMORY;
                        break;
                    }
                    //
                    // Copy the signed issuance license into the
                    // pContext->wszData buffer
                    //
                    hr = StringCchCopyW ( ( wchar_t* ) pContext->wszData,
                                          uiSignedILLength + 1 ,
                                          ( PWSTR ) pvParam
                                        );
                    if ( FAILED ( hr ) )
                    {
                        wprintf ( L"\nStringCchCopyW failed.  hr = 0x%x\n", hr );
                        break;
                    }
                }
            }
            if ( pContext->hEvent )
            {
                SetEvent ( ( HANDLE ) pContext->hEvent );
            }
            break;
        case E_DRM_ACTIVATIONFAILED:
            wprintf ( L"Callback hr = E_DRM_ACTIVATIONFAILED\n" );
            break;
        case E_DRM_HID_CORRUPTED:
            wprintf ( L"Callback hr = E_DRM_HID_CORRUPTED\n" );
            break;
        case E_DRM_INSTALLATION_FAILED:
            wprintf ( L"Callback hr = E_DRM_INSTALLATION_FAILED\n" );
            break;
        case E_DRM_ALREADY_IN_PROGRESS:
            wprintf ( L"Callback hr = E_DRM_ALREADY_IN_PROGRESS\n" );
            break;
        case E_DRM_NO_CONNECT:
            wprintf ( L"Callback hr = E_DRM_NO_CONNECT\n" );
            break;
        case E_DRM_ABORTED:
            wprintf ( L"Callback hr = E_DRM_ABORTED\n" );
            break;
        case E_DRM_SERVER_ERROR:
            wprintf ( L"Callback hr = E_DRM_SERVER_ERROR\n" );
            break;
        case E_DRM_INVALID_SERVER_RESPONSE:
            wprintf ( L"Callback hr = E_DRM_INVALID_SERVER_RESPONSE\n" );
            break;
        case E_DRM_SERVER_NOT_FOUND:
            wprintf ( L"Callback hr = E_DRM_SERVER_NOT_FOUND\n" );
            break;
        case E_DRM_AUTHENTICATION_FAILED:
            wprintf ( L"Callback hr = E_DRM_AUTHENTICATION_FAILED\n" );
            break;
        case E_DRM_EMAIL_NOT_VERIFIED:
            wprintf ( L"Callback hr = E_DRM_EMAIL_NOT_VERIFIED\n" );
            break;
        case E_DRM_AD_ENTRY_NOT_FOUND:
            wprintf ( L"Callback hr = E_DRM_AD_ENTRY_NOT_FOUND\n" );
            break;
        case E_DRM_NEEDS_MACHINE_ACTIVATION:
            wprintf ( L"Callback hr = E_DRM_NEEDS_MACHINE_ACTIVATION\n" );
            break;
        case E_DRM_NEEDS_GROUPIDENTITY_ACTIVATION:
            wprintf ( L"Callback hr = E_DRM_NEEDS_GROUPIDENTITY_ACTIVATION\n" );
            break;
        case E_DRM_REQUEST_DENIED:
            wprintf ( L"Callback hr = E_DRM_REQUEST_DENIED\n" );
            break;
        case E_DRM_INVALID_EMAIL:
            wprintf ( L"Callback hr = E_DRM_INVALID_EMAIL\n" );
            break;
        case E_DRM_SERVICE_GONE:
            wprintf ( L"Callback hr = E_DRM_SERVICE_GONE\n" );
            break;
        case E_DRM_SERVICE_MOVED:
            wprintf ( L"Callback hr = E_DRM_SERVICE_MOVED\n" );
            break;
        case E_DRM_VALIDITYTIME_VIOLATION:
            wprintf ( L"Callback hr = E_DRM_VALIDITYTIME_VIOLATION\n" );
            break;
        case S_DRM_REQUEST_PREPARED:
            wprintf ( L"Callback hr = S_DRM_REQUEST_PREPARED\n" );
            break;
        case E_DRM_NO_LICENSE:
            wprintf ( L"Callback hr = E_DRM_NO_LICENSE\n" );
            break;
        case E_DRM_SERVICE_NOT_FOUND:
            wprintf ( L"Callback hr = E_DRM_SERVICE_NOT_FOUND\n" );
            break;
        case E_DRM_LICENSEACQUISITIONFAILED:
            wprintf ( L"Callback hr = E_DRM_LICENSEACQUISITIONFAILED\n" );
            break;
        default:
            wprintf ( L"Default callback hr = 0x%x\n", hr );
            if ( pContext->hEvent )
            {
                SetEvent ( ( HANDLE ) pContext->hEvent );
            }
            break;
    }
    return S_OK;
}

HRESULT FindServiceURL ( DRMHSESSION hClient,
                         UINT uiServiceType,
                         UINT uiServiceLocation,
                         PWSTR *pwszServiceUrl )
{
    HRESULT         hr              = S_OK;
    UINT            uiURLLen        = 0;
    hr = DRMGetServiceLocation ( hClient, uiServiceType, uiServiceLocation, NULL, &uiURLLen, NULL );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Get service location failed. Return code: 0x%x", hr );
        goto e_Exit;
    }

    *pwszServiceUrl = new WCHAR[uiURLLen];
    hr = DRMGetServiceLocation ( hClient, uiServiceType, uiServiceLocation, NULL, &uiURLLen, *pwszServiceUrl );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Get service location failed. Return code: 0x%x", hr );
        goto e_Exit;
    }

e_Exit:
    return hr;
}

// Activate machine
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
    wprintf ( L"The machine is now activated.\n" );

e_Exit:
    if ( NULL != context.hEvent )
    {
        CloseHandle ( context.hEvent );
    }
    return hr;
}

// Activate User
HRESULT DoUserActivation ( DRMHSESSION hClient )
{
    HRESULT         hr              = S_OK;
    PWSTR           wszServiceUrl   = NULL;
    DRM_ACTSERV_INFO *pdasi         = NULL;
    DRM_CONTEXT     context;
    DWORD           dwWaitResult    = 0;
    hr = FindServiceURL ( hClient,
                          DRM_SERVICE_TYPE_ACTIVATION,
                          DRM_SERVICE_LOCATION_ENTERPRISE,
                          &wszServiceUrl );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }
    pdasi = new DRM_ACTSERV_INFO;
    pdasi->wszURL = wszServiceUrl;

    SecureZeroMemory ( &context, sizeof ( DRM_CONTEXT ) );
    if ( NULL == ( context.hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL ) ) )
    {
        hr = GetLastError();
        wprintf ( L"Create event failed with an unexcepted error: 0x%x\n", hr );
        goto e_Exit;
    }

    hr = DRMActivate ( hClient,
                       DRM_ACTIVATE_GROUPIDENTITY,
                       0,
                       pdasi,
                       ( VOID* ) &context,
                       NULL );
    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        hr = ERROR_TIMEOUT;
        wprintf ( L"Do user activation timed out.\n" );
        goto e_Exit;
    }
    if ( FAILED ( context.hr ) )
    {
        hr = context.hr;
        wprintf ( L"Activate user failed. Callback function returned a failure code: 0x%x\n", hr );
        goto e_Exit;
    }
e_Exit:
    return hr;
}

int _tmain ( int argc, _TCHAR* argv[] )
{
    LPWSTR          wszGroupId      = L"user1@doc.lab"; // 用户ID
    // LPWSTR          wszGroupId      = NULL; // 用户ID
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
            hr = DoMachineActivation ( hClient );
        }
        else
        {
            hr = GetLastError();
            wprintf ( L"Activate machine failed with an unexcepted error: 0x%x", hr );
            goto e_Exit;
        }
    }
    else
    {
        wprintf ( L"The machine is already activated.\n" );
    }
    // 激活用户RAC证书
    hr = DRMIsActivated ( hClient, DRM_ACTIVATE_GROUPIDENTITY, NULL );
    if ( FAILED ( hr ) )
    {
        if ( E_DRM_NEEDS_GROUPIDENTITY_ACTIVATION == hr )
        {
            hr = DoUserActivation ( hClient );
        }
        else
        {
            hr = GetLastError();
            wprintf ( L"Activate user failed with an unexcepted error: 0x%x\n", hr );
            goto e_Exit;
        }
    }
    else
    {
        wprintf ( L"The user is already activated.\n" );
    }

e_Exit:
    return hr;
}

