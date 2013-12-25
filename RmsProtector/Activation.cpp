#include "stdafx.h"
#include "functions.h"

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