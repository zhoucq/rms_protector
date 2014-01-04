#include "stdafx.h"
#include "functions.h"

HRESULT GetUnsignedIL ( PWSTR wszUserName,
                        PWSTR *ppwszGUID,
                        DRMPUBHANDLE *phIssuanceLic )
{
    HRESULT             hr              = NULL;
    GUID                guid;
    UINT                uiGUIDLength    = 0;
    DRMPUBHANDLE        hUser           = NULL;
    DRMPUBHANDLE        hRight          = NULL;

    hr = DRMCreateIssuanceLicense ( NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    phIssuanceLic    // 创建IL句柄
                                  );
    if ( FAILED ( hr ) ) goto e_Exit;

    // 生成GUID
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

    // 设置MetaData
    hr = DRMSetMetaData ( *phIssuanceLic,
                          *ppwszGUID,
                          L"MS-GUID",
                          NULL,     // SKU ID
                          NULL,     // SKU ID TYPE
                          NULL,     // Content Type
                          NULL );   // Content Name

    if ( FAILED ( hr ) ) goto e_Exit;

    // 创建User
    hr = DRMCreateUser ( wszUserName, NULL, L"Unspecified", &hUser );
    if ( FAILED ( hr ) ) goto e_Exit;

    // 创建Right
    hr = DRMCreateRight ( L"EDIT", NULL, NULL, 0, NULL, NULL, &hRight );
    if ( FAILED ( hr ) ) goto e_Exit;

    // 将User和Right加到IL中
    hr = DRMAddRightWithUser ( *phIssuanceLic, hRight, hUser );
    if ( FAILED ( hr ) ) goto e_Exit;

e_Exit:
    if ( NULL != hUser )
    {
        CloseHandle ( &hUser );
    }
    if ( NULL != hRight )
    {
        CloseHandle ( &hRight );
    }
    return hr;
}