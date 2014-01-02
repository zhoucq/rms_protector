#include "stdafx.h"
#include "functions.h"

// Get certificate
HRESULT GetCertificate ( DRMHSESSION hClient,
                         UINT uiCertFlag,
                         PWSTR *pwszCertificate )
{
    HRESULT             hr                      = S_OK;
    UINT                uiCertificateLength     = 0;
    BOOL                fShared                 = true;
    hr = DRMEnumerateLicense ( hClient,
                               uiCertFlag,
                               0,
                               &fShared,
                               &uiCertificateLength,
                               NULL );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }
    *pwszCertificate = new WCHAR[uiCertificateLength];
    hr = DRMEnumerateLicense ( hClient,
                               uiCertFlag,
                               0,
                               &fShared,
                               &uiCertificateLength,
                               *pwszCertificate );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }
e_Exit:
    return hr;
}