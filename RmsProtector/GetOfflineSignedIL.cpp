#include "stdafx.h"
#include "functions.h"

HRESULT GetOfflineSignedIL ( DRMHANDLE *phIssuanceLicense,
                             PWSTR pwszCLC,
                             PWSTR *ppwszSignedIL )
{
    HRESULT             hr                  = S_OK;
    DRM_CONTEXT         context;
    DWORD               dwWaitResult        = 0;

    SecureZeroMemory ( &context, sizeof ( DRM_CONTEXT ) );
    context.hEvent = CreateEvent ( NULL,
                                   FALSE,
                                   FALSE,
                                   NULL );
    if ( NULL == context.hEvent )
    {
        hr = GetLastHR();
        goto e_Exit;
    }

    hr = DRMGetSignedIssuanceLicense ( NULL,
                                       *phIssuanceLicense,
                                       DRM_SIGN_OFFLINE | DRM_AUTO_GENERATE_KEY,
                                       NULL,
                                       0,
                                       L"AES",
                                       pwszCLC,
                                       &StatusCallback,
                                       NULL,
                                       &context );
    if ( FAILED ( hr ) ) goto e_Exit;
    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        hr = HRESULT_FROM_WIN32 ( ERROR_TIMEOUT );
        goto e_Exit;
    }

e_Exit:
    return hr;
}


HRESULT GetOfflineSignedIL_ ( DRMENVHANDLE hEnv,
                              DRMHANDLE hLib,
                              PWSTR pwszUserID,
                              PWSTR pwszMachineCert,
                              PWSTR pwszCLC,
                              PWSTR pwszManifest,
                              PWSTR *ppwszGUID,
                              DRMPUBHANDLE *phIssuanceLic,
                              PWSTR *ppwszSignedIL )
{
    HRESULT                 hr              = S_OK;
    SYSTEMTIME              stTimeFrom;
    SYSTEMTIME              stTimeUntil;
    DRMPUBHANDLE            hIssuanceLic;
    GUID                    guid;
    UINT                    uiGUIDLength;
    DRMPUBHANDLE            hUser           = NULL;
    DRMPUBHANDLE            hRight          = NULL;
    DRM_CONTEXT             context;
    DWORD                   dwWaitResult    = 0;
    PWSTR                   pwszLicensingSvr    = NULL;

    SecureZeroMemory ( &context, sizeof ( DRM_CONTEXT ) );

    /* if ( NULL != pwszTimeFrom )
    {
        ConvertStringToSystemTime ( pwszTimeFrom, &stTimeFrom );
    }
    else
    {
        ZeroMemory ( &stTimeFrom, sizeof ( SYSTEMTIME ) );
    }

    if ( NULL != pwszTimeUntil )
    {
        ConvertStringToSystemTime ( pwszTimeUntil, &stTimeUntil );
    }
    else
    {
        ZeroMemory ( &stTimeUntil, sizeof ( SYSTEMTIME ) );
    }
    */
    hr = DRMCreateIssuanceLicense ( NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &hIssuanceLic );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"DRMCreateIssuanceLicense failed: 0x%x\n", hr );
        goto e_Exit;
    }

    hr = CoCreateGuid ( &guid );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }
    *ppwszGUID = new WCHAR[GUID_LENGTH];
    uiGUIDLength = StringFromGUID2 ( guid, *ppwszGUID, GUID_LENGTH );
    if ( 0 == uiGUIDLength )
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    hr = DRMSetMetaData ( hIssuanceLic,
                          *ppwszGUID,
                          L"MSGUID",
                          NULL,     // SKU ID
                          NULL,     // SKU ID TYPE
                          NULL,     // Content Type
                          NULL      // Content Name
                        );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"DRMSetMetaData failed: 0x%x\n", hr );
        goto e_Exit;
    }

    /* ∂ÓÕ‚…Ë÷√ */
    /*
    hr = DRMSetApplicationSpecificData ( hIssuanceLic,
                                         false,
                                         L"",
                                         L"" );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }
    */

    // Set the usage policy
    hr = DRMSetUsagePolicy (
             hIssuanceLic,                      // Issuance license handle
             DRM_USAGEPOLICY_TYPE_BYNAME,       // Policy type
             false,                             // Add the policy
             true,                              // Rights are prohibited
             L"ApplicationName",                // Application name
             L"1.0.0.0",                        // Minimum version number
             L"3.0.0.0",                        // Maximum version number
             NULL,                              // Public key not required
             NULL,                              // Digest algorithm
             NULL,                              // Pointer to the digest
             NULL );                            // Digest length
    if ( FAILED ( hr ) ) goto e_Exit;

    hr = DRMCreateUser ( pwszUserID, NULL, L"Windows", &hUser );
    if ( FAILED ( hr ) ) goto e_Exit;

    hr = DRMCreateRight ( L"Owner",
                          NULL,
                          NULL,
                          0,
                          NULL,
                          NULL,
                          &hRight );
    if ( FAILED ( hr ) ) goto e_Exit;

    hr = DRMAddRightWithUser ( hIssuanceLic, hRight, hUser );
    if ( FAILED ( hr ) ) goto e_Exit;



    context.hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL );
    hr = DRMGetSignedIssuanceLicense ( hEnv,
                                       hIssuanceLic,
                                       DRM_SIGN_OFFLINE |
                                       DRM_AUTO_GENERATE_KEY,
                                       NULL,
                                       0,
                                       L"AES",
                                       NULL,
                                       &StatusCallback,
                                       pwszCLC,
                                       &context );
    if ( FAILED ( hr ) ) goto e_Exit;

    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( WAIT_TIMEOUT == dwWaitResult || FAILED ( context.hr ) ) goto e_Exit;

    *ppwszSignedIL = context.wszData;

e_Exit:
    if ( NULL != hUser )
    {
        CloseHandle ( &hUser );
    }
    if ( NULL != hRight )
    {
        CloseHandle ( &hRight );
    }
    if ( NULL != hIssuanceLic )
    {
        CloseHandle ( &hIssuanceLic );
    }
    if ( NULL != context.hEvent )
    {
        CloseHandle ( context.hEvent );
        SecureZeroMemory ( &context, sizeof ( context ) );
    }
    return hr;
}