// RmsProtector.cpp : Defines the entry point for the console application.
//

// ͷ�ļ�
#include "stdafx.h"
#include <Windows.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>

#define DW_WAIT_RESULT		60 * 1000

// �ص�����
HRESULT __stdcall
StatusCallback ( DRM_STATUS_MSG msg,
                 HRESULT hr,
                 void *pvParam,
                 void *pvContext )
{
    // HRESULT			hr				= S_OK;
    return S_OK;
}

int _tmain ( int argc, _TCHAR* argv[] )
{
    LPWSTR			wszGroupId		= NULL; // �û�ID
    HRESULT			hr				= S_OK;
    DRMHSESSION		hClient         = NULL;	// client session

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

    // ��������֤��
    hr = DRMIsActivated ( hClient, DRM_ACTIVATE_MACHINE, NULL );

    if ( FAILED ( hr ) )
    {
        if ( hr == E_DRM_NEEDS_MACHINE_ACTIVATION )
        {
            // δ����
            DRMActivate ( hClient, DRM_ACTIVATE_MACHINE, 0, NULL, NULL, NULL );
        }
        else
        {
            hr = GetLastError();
            wprintf ( L"Unexcepted error: 0x%x", hr );
            return hr;
        }
    }

    return 0;
}

