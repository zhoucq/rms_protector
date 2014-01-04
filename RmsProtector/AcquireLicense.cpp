#include "stdafx.h"
#include "functions.h"

HRESULT
DoAcquireClientLicensorCertificate ( DRMHSESSION hClient,
                                     PWSTR wszLicensingSvr,
                                     PWSTR *wszClientLicensorCert )
{
    HRESULT         hr              = S_OK;
    PWSTR           wszLicensingSrv = NULL;
    DRM_CONTEXT     context;
    DWORD           dwWaitResult;
    hr = GetServiceLocation ( hClient,
                              DRM_SERVICE_TYPE_CLIENTLICENSOR,
                              DRM_SERVICE_LOCATION_ENTERPRISE,
                              &wszLicensingSrv );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Find service url failed." );
        goto e_Exit;
    }

    if ( NULL == ( context.hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL ) ) )
    {
        hr = GetLastHR();
        wprintf ( L"Create event failed with an unexcepted error: 0x%x", hr );
        goto e_Exit;
    }

    hr = DRMAcquireLicense ( hClient,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             wszLicensingSrv,
                             ( VOID* ) &context );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Acquire license failed.\n" );
        goto e_Exit;
    }
    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        hr = ERROR_TIMEOUT;
        wprintf ( L"Acquire license timed out.\n" );
        goto e_Exit;
    }
    if ( FAILED ( context.hr ) )
    {
        hr = context.hr;
        wprintf ( L"Acquire license failed. Callback function returned a failure code: 0x%x", hr );
        goto e_Exit;
    }
e_Exit:
    if ( NULL != context.hEvent )
    {
        CloseHandle ( context.hEvent );
    }
    return hr;
}