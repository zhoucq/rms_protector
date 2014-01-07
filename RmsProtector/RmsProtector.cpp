// RmsProtector.cpp : Defines the entry point for the console application.
//

// 头文件
#include "stdafx.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>
#include "functions.h"

using namespace std;


WCHAR* getCmdOption ( WCHAR** begin, WCHAR** end, const std::wstring & option )
{
    WCHAR ** itr = std::find ( begin, end, option );
    if ( itr != end && ++itr != end )
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists ( WCHAR** begin, WCHAR** end, const std::wstring & option )
{
    WCHAR ** itr =  std::find ( begin, end, option );
    return std::find ( begin, end, option ) != end;
}


int _tmain ( int argc, _TCHAR* argv[] )
{
    // TODO: 需要一个ParseArguments方法
    // parse arguments
    PWSTR           pwszOptTimeStart                = NULL;
    PWSTR           pwszOptTimeUntil                = NULL;
    PWSTR           pwszOptRights                   = NULL;
    PWSTR           pwszOptInput                    = NULL;
    PWSTR           pwszOptOutput                   = NULL;

    pwszOptTimeStart = getCmdOption ( argv, argv + argc, L"-start" );
    pwszOptTimeUntil = getCmdOption ( argv, argv + argc, L"-until" );
    pwszOptRights = getCmdOption ( argv, argv + argc, L"-rights" );
    pwszOptInput = getCmdOption ( argv, argv + argc, L"-input" );
    pwszOptOutput = getCmdOption ( argv, argv + argc, L"-output" );

    // TODO: arguments check
    //if ( NULL != pwszOptTimeStart )
    //{
    //}


    LPWSTR          wszUserName                     = L"user1@doc.lab"; // 用户ID
    //LPWSTR          wszUserName                   = NULL; // 用户ID
    HRESULT         hr                              = S_OK;
    DRMHSESSION     hClient                         = NULL; // client session
    PWSTR           wszMachineCertificate           = NULL; // machine cert
    PWSTR           wszRAC                          = NULL; // RAC
    PWSTR           wszClientLicensorCert           = NULL; // Client licensor cert
    PWSTR           wszCLCServiceUrl                = NULL;
    PWSTR           pwszLicensingSrv                 = NULL;
    PWSTR           wszManifest                     = NULL; // manifest文件
    UINT            uiSecurityProviderTypeLength    = 0;
    UINT            uiSecurityProviderPathLength    = 0;
    PWSTR           wszSecurityProviderType         = NULL;
    PWSTR           wszSecurityProviderPath         = NULL;
    DRMENVHANDLE    hEnv                            = NULL;
    DRMHANDLE       hLib                            = NULL;
    DRMHANDLE       hIssuanceLicense                = NULL;
    PWSTR           pwszGuid                        = NULL;
    PWSTR           pwszSignedIL                    = NULL;
    hr = DRMCreateClientSession ( &StatusCallback,
                                  0,
                                  DRM_DEFAULTGROUPIDTYPE_WINDOWSAUTH,
                                  wszUserName,
                                  &hClient );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Create client session failed." );
        return hr;
    }

    // 1. 激活并获取计算机证书
    hr = DRMIsActivated ( hClient, DRM_ACTIVATE_MACHINE, NULL );
    if ( FAILED ( hr ) )
    {
        if ( hr == E_DRM_NEEDS_MACHINE_ACTIVATION )
        {
            hr = DoMachineActivation ( hClient );
            if ( FAILED ( hr ) )
            {
                goto e_Exit;
            }
        }
        else
        {
            hr = GetLastHR();
            wprintf ( L"Activate machine failed with an unexcepted error: 0x%x", hr );
            goto e_Exit;
        }
    }
    else
    {
        wprintf ( L"The machine is already activated.\n" );
    }

    hr = GetCertificate ( hClient, DRM_EL_MACHINE, &wszMachineCertificate );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

    // 2. 激活/获取用户RAC证书
    hr = DRMIsActivated ( hClient, DRM_ACTIVATE_GROUPIDENTITY, NULL );
    if ( FAILED ( hr ) )
    {
        if ( E_DRM_NEEDS_GROUPIDENTITY_ACTIVATION == hr )
        {
            hr = DoUserActivation ( hClient );
            if ( FAILED ( hr ) )
            {
                goto e_Exit;
            }
        }
        else
        {
            hr = GetLastHR();
            wprintf ( L"Activate user failed with an unexcepted error: 0x%x\n", hr );
            goto e_Exit;
        }
    }
    else
    {
        wprintf ( L"The user is already activated.\n" );
    }
    hr = GetCertificate ( hClient, DRM_EL_SPECIFIED_GROUPIDENTITY, &wszRAC );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Get RAC failed.\n" );
        goto e_Exit;
    }

    // 3. 生成/获取CLC
    hr = GetCertificate ( hClient,
                          DRM_EL_SPECIFIED_CLIENTLICENSOR,
                          &wszClientLicensorCert );
    if ( FAILED ( hr ) )
    {
        if ( E_DRM_NO_MORE_DATA != hr )
        {
            wprintf ( L"Get CLC failed with an unexcepted error: 0x%x", hr );
            goto e_Exit;
        }
        else
        {
            hr = GetServiceLocation ( hClient,
                                      DRM_SERVICE_TYPE_CLIENTLICENSOR,
                                      DRM_SERVICE_LOCATION_ENTERPRISE,
                                      &wszCLCServiceUrl );
            if ( FAILED ( hr ) )
            {
                wprintf ( L"Find CLC service url failed. Error code: 0x%x", hr );
                goto e_Exit;
            }
            hr = DoAcquireClientLicensorCertificate ( hClient,
                    wszCLCServiceUrl,
                    &wszClientLicensorCert );
            if ( FAILED ( hr ) )
            {
                wprintf ( L"Acquire client licensor certificate failed. Error code: 0x%x", hr );
                goto e_Exit;
            }

        }
    }

    hr = ReadManifestToWideString ( &wszManifest );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Read manifest file failed. Error code: 0x%x", hr );
        goto e_Exit;
    }

    // Get the security provider
    hr = DRMGetSecurityProvider ( 0,
                                  &uiSecurityProviderTypeLength,
                                  NULL,
                                  &uiSecurityProviderPathLength,
                                  NULL );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"\nDRMGetSecurityProvider failed. hr = 0x%x\n", hr );
        goto e_Exit;
    }
    wszSecurityProviderType = new WCHAR[ uiSecurityProviderTypeLength ];
    if ( NULL == wszSecurityProviderType )
    {
        wprintf ( L"\nMemory allocation for wszSecurityProviderType "\
                  L"failed.\n" );
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    wszSecurityProviderPath = new WCHAR[ uiSecurityProviderPathLength ];
    if ( NULL == wszSecurityProviderPath )
    {
        wprintf ( L"\nMemory allocation for wszSecurityProviderPath "\
                  L"failed." );
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = DRMGetSecurityProvider ( 0,
                                  &uiSecurityProviderTypeLength,
                                  wszSecurityProviderType,
                                  &uiSecurityProviderPathLength,
                                  wszSecurityProviderPath );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"\nDRMGetSecurityProvider failed. hr = 0x%x\n", hr );
        goto e_Exit;
    }

    // Initialize an environment
    hr = DRMInitEnvironment ( DRMSECURITYPROVIDERTYPE_SOFTWARESECREP,
                              DRMSPECTYPE_FILENAME,
                              wszSecurityProviderPath,
                              wszManifest,
                              wszMachineCertificate,
                              &hEnv,
                              &hLib );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"\nDRMInitEnvironment failed. hr = 0x%x.\n", hr );
        goto e_Exit;
    }

    hr = GetUnsignedIL ( wszUserName, wszUserName, &pwszGuid, &hIssuanceLicense );
    if ( FAILED ( hr ) ) goto e_Exit;


    // 注释掉的代码使用Online方式签署IL
    /*
    hr = GetServiceLocation (
             hClient,                            // Client handle
             DRM_SERVICE_TYPE_PUBLISHING,        // Type of service
             DRM_SERVICE_LOCATION_ENTERPRISE,    // Location of service
             &pwszLicensingSrv );                // Service URL
    if ( FAILED ( hr ) ) goto e_Exit;

    hr = GetOnlineSignedIL ( &hIssuanceLicense, pwszLicensingSrv , &pwszSignedIL );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Online sign IL failed, hr = 0x%x.\n", hr );
        goto e_Exit;
    }*/

    hr = GetOfflineSignedIL ( hEnv,
                              &hIssuanceLicense,
                              wszClientLicensorCert,
                              &pwszSignedIL );

    hr = EncryptContent ( hEnv,
                          hLib,
                          wszRAC,
                          pwszGuid,
                          hIssuanceLicense,
                          pwszSignedIL,
                          NULL );
e_Exit:
    wprintf ( L"Return Code:0x%x", hr );
    return hr;
}

