#include "stdafx.h"
#include "functions.h"

HRESULT GetOnlineSignedIL ( DRMPUBHANDLE hIssuanceLic,
                            PWSTR wszLicensingSrv,
                            PWSTR *pwszSignedIL )
{
    HRESULT                 hr                  = S_OK;
    DRM_CONTEXT             context;
    DWORD                   dwWaitResult        = 0;
    SecureZeroMemory ( &context, sizeof ( DRM_CONTEXT ) );
    context.hEvent = CreateEvent ( NULL,
                                   FALSE,
                                   FALSE,
                                   NULL );
    if ( NULL == context.hEvent )
    {
        hr = GetLastError();
        goto e_Exit;
    }

    hr = DRMGetSignedIssuanceLicense ( NULL,
                                       hIssuanceLic,
                                       DRM_SIGN_ONLINE | DRM_AUTO_GENERATE_KEY,
                                       NULL,
                                       0,
                                       L"AES",
                                       NULL,
                                       &StatusCallback,
                                       wszLicensingSrv,
                                       &context );
    if ( FAILED ( hr ) ) goto e_Exit;

    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        hr = ERROR_TIMEOUT;
        goto e_Exit;
    }
    if ( FAILED ( context.hr ) )
    {
        hr = context.hr;
        goto e_Exit;
    }
    *pwszSignedIL = context.wszData;
    wprintf ( L"Online SignedIL: \n%s\n", *pwszSignedIL );


e_Exit:
    return hr;
}