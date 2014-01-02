#include "stdafx.h"
#include "functions.h"

HRESULT GetOfflineSignedIL ( DRMENVHANDLE hEnv,
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

    // TODO: 这里需要测试
    hr = DRMSetMetaData ( hIssuanceLic,
                          *ppwszGUID,
                          L"",
                          L"",
                          L"",
                          L"",
                          L"" );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

    /* 额外设置 */
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
e_Exit:
    return hr;
}