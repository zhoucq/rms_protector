#include "stdafx.h"
#include "functions.h"

BOOL ConvertStringToSystemTime ( PWSTR pwszTimeString,
                                 SYSTEMTIME *pstTime )
{
    UINT wYear, wMonth, wDay, wHour, wMinute, wSecond;
    SecureZeroMemory ( pstTime, sizeof ( SYSTEMTIME ) );
    int ret = swscanf_s ( pwszTimeString,
                          L"%d-%d-%d %d:%d:%d",
                          &wYear,
                          &wMonth,
                          &wDay,
                          &wHour,
                          &wMinute,
                          &wSecond );
    pstTime->wYear = wYear;
    pstTime->wMonth = wMonth;
    pstTime->wDay = wDay;
    pstTime->wHour = wHour;
    pstTime->wMinute = wMinute;
    pstTime->wSecond = wSecond;
    return ret;
}

// 以Unicode编码读取manifest文件
HRESULT ReadManifestToWideString ( PWSTR *pwszFileString )
{
    HRESULT                     hr              = S_OK;
    HANDLE                      hFile           = INVALID_HANDLE_VALUE;
    DWORD                       dwBytesRead     = 0;
    BY_HANDLE_FILE_INFORMATION  fileInfo;
    WCHAR                       wcFilePath[MAX_PATH];
    // 获取Manifest文件名
    if ( !GetModuleFileName ( NULL , wcFilePath, MAX_PATH ) )
    {
        hr = GetLastError();
        goto e_Exit;
    }
    hr = StringCchCat ( wcFilePath, MAX_PATH, L".xml" );
    if ( FAILED ( hr ) )
    {
        goto e_Exit;
    }

    hFile = CreateFileW ( wcFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if ( INVALID_HANDLE_VALUE == hFile )
    {
        hr = GetLastError();
        goto e_Exit;
    }

    if ( 0 == GetFileInformationByHandle ( hFile, &fileInfo ) )
    {
        hr = GetLastError();
        goto e_Exit;
    }
    *pwszFileString = new WCHAR[fileInfo.nFileIndexLow + sizeof ( WCHAR )];
    if ( 0 == ReadFile ( hFile, *pwszFileString, fileInfo.nFileSizeLow, &dwBytesRead, 0 ) )
    {
        hr = GetLastError();
        goto e_Exit;
    }

e_Exit:
    if ( NULL != hFile )
    {
        CloseHandle ( hFile );
    }
    return hr;
}