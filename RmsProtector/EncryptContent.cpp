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
    HRESULT                 hr          = S_OK;
    DRMID         idNULL;
    DRMID         idContent;
    DRMID         idStandardEP;
    if ( NULL == hEnv || NULL == hLib || NULL == pwszRAC || NULL == pwszGUID ||
            NULL == hIssuanceLic || NULL == pwszSignedIL || NULL == ppbEncrypted )
        return E_INVALIDARG;
e_Exit:
    return hr;
}