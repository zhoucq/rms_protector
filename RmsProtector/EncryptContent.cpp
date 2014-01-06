#include "stdafx.h"
#include "functions.h"

HRESULT EncryptContent ( DRMENVHANDLE hEnv,
                         DRMHANDLE hLib,
                         PWSTR pwszRAC,
                         PWSTR pwszGUID,
                         DRMHANDLE hIssuanceLic,
                         PWSTR pwszSignedIL,
                         BYTE** ppbEncrypted )
{
    HRESULT                     hr                          = S_OK;
    DRMHANDLE                   hEP                         = NULL;
    DRMID                       idNULL;
    DRMID                       idContent;
    DRMID                       idStandardEP;
    UINT                        uiOwnerLicenseLength        = 0;
    DRMBOUNDLICENSEPARAMS       oParams;
    DRMENCODINGTYPE             eType;
    if ( NULL == hEnv || NULL == hLib || NULL == pwszRAC || NULL == pwszGUID ||
            NULL == hIssuanceLic || NULL == pwszSignedIL )
    {
        return E_INVALIDARG;
    }

    SecureZeroMemory ( &idNULL, sizeof ( DRMID ) );
    SecureZeroMemory ( &idStandardEP, sizeof ( DRMID ) );
    idStandardEP.wszID = L"ASCII Tag";
    idStandardEP.wszIDType = L"UDStdPlg Enabling Principal";

    hr = DRMCreateEnablingPrincipal ( hEnv,
                                      hLib,
                                      idStandardEP.wszIDType,
                                      &idNULL,
                                      pwszRAC,
                                      &hEP );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

    hr = DRMGetOwnerLicense ( hIssuanceLic,
                              &uiOwnerLicenseLength,
                              NULL
                            );

    if ( FAILED ( hr )  && E_DRM_OWNER_LICENSE_NOT_FOUND != hr )
    {
        goto e_Exit;
    }
    if ( E_DRM_OWNER_LICENSE_NOT_FOUND == hr )
    {
        // No owner license found, do nothing
    }
    else
    {
        // todo: test owner lincense
    }

    idContent.uVersion = 0;
    idContent.wszID = L"";

e_Exit:
    return hr;
}