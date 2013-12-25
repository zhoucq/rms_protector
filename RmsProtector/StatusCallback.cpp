#include "stdafx.h"
#include "functions.h"

// »Øµ÷º¯Êı
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