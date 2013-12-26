#include "stdafx.h"
#include "functions.h"

HRESULT GetOfflineSignedIL ( DRMENVHANDLE hEnv,
                             DRMHANDLE hLib,
                             PWSTR pwszUserID,
                             PWSTR pwszMachineCert,
                             PWSTR pwszCLC,
                             PWSTR pwszManifest,
                             PWSTR pwszTimeFrom,
                             PWSTR pwszTimeUntil,
                             PWSTR pwszUsersRights,
                             PWSTR *ppwszGUID,
                             // DRMPUBHANDLE *phIssuanceLic,
                             PWSTR *ppwszSignedIL )
{
    HRESULT                 hr              = S_OK;
    SYSTEMTIME              stTimeFrom;
    SYSTEMTIME              stTimeUntil;
    DRMPUBHANDLE            hIssuanceLic;
    if ( NULL != pwszTimeFrom )
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

    
e_Exit:
    return hr;
}