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
    DRMHANDLE                   hBoundLicense               = NULL;
    PWSTR                       wszOwnerLicense             = NULL;
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

    // 注意：
    // 如果是Online Signed IL，需要使用DRMAcquireLicense方法获得EUL然后绑定
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
        wprintf ( L"Owner License NOT Found. \n" );
    }
    else
    {
        wszOwnerLicense = new WCHAR[ uiOwnerLicenseLength ];
        if ( NULL == wszOwnerLicense )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        hr = DRMGetOwnerLicense ( hIssuanceLic,
                                  &uiOwnerLicenseLength,
                                  wszOwnerLicense
                                );
        if ( FAILED ( hr ) ) goto e_Exit;
    }

    idContent.uVersion = 0;
    idContent.wszIDType = L"MS-GUID";
    idContent.wszID = pwszGUID;

    oParams.hEnablingPrincipal = hEP;
    oParams.hSecureStore = NULL;
    oParams.wszRightsRequested = L"OWNER";
    oParams.wszRightsGroup = L"Main-Rights";
    oParams.idResource = idContent;
    oParams.wszDefaultEnablingPrincipalCredentials = NULL;
    oParams.cAuthenticatorCount = 0;

    hr = DRMCreateBoundLicense ( hEnv,
                                 &oParams,
                                 wszOwnerLicense,
                                 &hBoundLicense,
                                 NULL );
    if ( FAILED ( hr ) ) goto e_Exit;

e_Exit:
    return hr;
}