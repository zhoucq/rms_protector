// RmsProtector.cpp : Defines the entry point for the console application.
//

// 头文件
#include "stdafx.h"
#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <strsafe.h>
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

HRESULT DoAcquireClientLicensorCertificate (
    DRMHSESSION hClient,
    PWSTR wszLicensingSvr,
    PWSTR *wszClientLicensorCert )
{
    HRESULT         hr              = S_OK;
    PWSTR           wszLicensingSrv = NULL;
    DRM_CONTEXT     context;
    DWORD           dwWaitResult;
    hr = FindServiceURL ( hClient,
                          DRM_SERVICE_TYPE_CLIENTLICENSOR,
                          DRM_SERVICE_LOCATION_ENTERPRISE,
                          &wszLicensingSrv );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Find service url failed." );
        goto e_Exit;
    }

    if ( NULL == ( context.hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL ) ) )
    {
        hr = GetLastError();
        wprintf ( L"Create event failed with an unexcepted error: 0x%x", hr );
        goto e_Exit;
    }

    hr = DRMAcquireLicense ( hClient,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             wszLicensingSrv,
                             ( VOID* ) &context );
    if ( FAILED ( hr ) )
    {
        wprintf ( L"Acquire license failed.\n" );
        goto e_Exit;
    }
    dwWaitResult = WaitForSingleObject ( context.hEvent, DW_WAIT_RESULT );
    if ( dwWaitResult == DW_WAIT_RESULT )
    {
        hr = ERROR_TIMEOUT;
        wprintf ( L"Acquire license timed out.\n" );
        goto e_Exit;
    }
    if ( FAILED ( context.hr ) )
    {
        hr = context.hr;
        wprintf ( L"Acquire license failed. Callback function returned a failure code: 0x%x", hr );
        goto e_Exit;
    }
e_Exit:
    if ( NULL != context.hEvent )
    {
        CloseHandle ( context.hEvent );
    }
    return hr;
}

int _tmain ( int argc, _TCHAR* argv[] )
{

    LPWSTR          wszGroupId                      = L"user1@doc.lab"; // 用户ID
    // LPWSTR          wszGroupId      = NULL; // 用户ID
    HRESULT         hr                              = S_OK;
    DRMHSESSION     hClient                         = NULL; // client session
    PWSTR           wszMachineCertificate           = NULL; // machine cert
    PWSTR           wszRAC                          = NULL; // RAC
    PWSTR           wszClientLicensorCert           = NULL; // Client licensor cert
    PWSTR           wszCLCServiceUrl                = NULL;
    PWSTR           wszManifest                     = NULL; // manifest文件
    UINT            uiSecurityProviderTypeLength    = 0;
    UINT            uiSecurityProviderPathLength    = 0;
    PWSTR           wszSecurityProviderType         = NULL;
    PWSTR           wszSecurityProviderPath         = NULL;
    DRMENVHANDLE    hEnv                            = NULL;
    DRMHANDLE       hLib                            = NULL;
    DRMHANDLE       hIssuanceLicense                = NULL;
    hr = DRMCreateClientSession ( &StatusCallback,
                                  0,
                                  DRM_DEFAULTGROUPIDTYPE_WINDOWSAUTH,
                                  wszGroupId,
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
        }
        else
        {
            hr = GetLastError();
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
        }
        else
        {
            hr = GetLastError();
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
            hr = FindServiceURL ( hClient,
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
    hr = DRMGetSecurityProvider ( 0, &uiSecurityProviderTypeLength, NULL, &uiSecurityProviderPathLength, NULL );
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

    // create an unsigned IL from scratch
    hr = DRMCreateIssuanceLicense ( NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &hIssuanceLicense );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

e_Exit:
    return hr;
}

