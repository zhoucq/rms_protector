#include "stdafx.h"
#include "functions.h"

// �����ַ����
HRESULT GetServiceLocation ( DRMHSESSION hClient,
                             UINT uiServiceType,
                             UINT uiServiceLocation,
                             PWSTR *pwszServiceUrl )
{
    HRESULT         hr              = S_OK;
    UINT            uiURLLen        = 0;
    hr = DRMGetServiceLocation ( hClient, uiServiceType, uiServiceLocation, NULL, &uiURLLen, NULL );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

    *pwszServiceUrl = new WCHAR[uiURLLen];
    hr = DRMGetServiceLocation ( hClient, uiServiceType, uiServiceLocation, NULL, &uiURLLen, *pwszServiceUrl );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

e_Exit:
    return hr;
}