#pragma once
#include <string>
using namespace std;

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>

//#define __in  
//#define __out
 
#define MS_PROCESSOR_ARCHITECTURE_IA64             6
#define MS_PROCESSOR_ARCHITECTURE_AMD64          9
 
//�ο� https://www.cnblogs.com/huhu0013/p/4595702.html
class OsInfoHelper{

public:
	string GetOsBits()
	{
		// TODO: Add your control notification handler code here
		//char szOSVersion[40]={0};
		//	getOsVersion(szOSVersion);  //��ò���ϵͳ����
 
		//string bit =" " + GetSystemBits(); // ��ȡ����ϵͳλ��

		return GetSystemBits1();
	}

private:
	
	void getOsVersion(char* szOSName)
	{
		//https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoa
	    DWORD  dwMajorVersion;
	    DWORD   dwMinorVersion;
	    DWORD  dwBuildNumber;
	    DWORD  dwPlatformId;
	    OSVERSIONINFO osvi;//����OSVERSIONINFO���ݽṹ����
	    memset(&osvi, 0, sizeof(OSVERSIONINFO));//���ռ� 
	    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);//�����С 
	    GetVersionEx (&osvi);//��ð汾��Ϣ 
	    dwMajorVersion=osvi.dwMajorVersion;//���汾��
	    dwMinorVersion=osvi.dwMinorVersion;//���汾
	    dwBuildNumber=osvi.dwBuildNumber;//������
	    dwPlatformId=osvi.dwPlatformId;//ID��
	    char swVersion[10]={0};    
	    sprintf(swVersion,"%d.%d",dwMajorVersion,dwMinorVersion);
	    // dwVesion=atoi(swVersion);  
 
		 if (!strcmp(swVersion,"4.0")) strcpy( szOSName,"Win95");    //win95    
		 if (!strcmp(swVersion,"4.1")) strcpy( szOSName,"Win98");     //win98 
		 if (!strcmp(swVersion,"4.9")) strcpy( szOSName,"Win_me");     // win_me 
		 if (!strcmp(swVersion,"3.51")) strcpy( szOSName,"win_Nt_3_5");  //win_Nt_3_5    
		 if (!strcmp(swVersion,"5.0")) strcpy( szOSName,"Windows 2000");    //win2000   
	     if (!strcmp(swVersion,"5.1")) strcpy( szOSName,"Windows XP");    //win_xp 
	     if (!strcmp(swVersion,"5.2")) strcpy( szOSName,"win2003");    // win2003 
		 if (!strcmp(swVersion,"6.6")) strcpy(szOSName,"vista");    //vista
	     if (!strcmp(swVersion,"6.1")) strcpy( szOSName,"win7");     // win7 
	     if (!strcmp(swVersion,"6.1")) strcpy( szOSName,"win8");      // win8 
	}

	
	// ��ȫ��ȡ����ʵϵͳ��Ϣ
	void SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
	{
	 if (NULL==lpSystemInfo) return;
	 typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	 LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress( GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");;
	 if (NULL != fnGetNativeSystemInfo)
	 {
	  fnGetNativeSystemInfo(lpSystemInfo);
	 }
	 else
	 {
	  GetSystemInfo(lpSystemInfo);
	 }
	}

	// ��ȡ����ϵͳλ��
	string GetSystemBits1()
    {
       SYSTEM_INFO si;
	   SafeGetNativeSystemInfo(&si);
	   if (si.wProcessorArchitecture == MS_PROCESSOR_ARCHITECTURE_AMD64 ||
		  si.wProcessorArchitecture == MS_PROCESSOR_ARCHITECTURE_IA64 )
	   {
	      return "64-bit";
	   }
	   return "32-bit";
	}
	
	//http://msdn.microsoft.com/en-us/library/ms684139%28VS.85%29.aspx
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	//��ò���ϵͳλ��������2
	string GetSystemBits2()
	{
		// IsWow64Process �� Windows XP with SP2 �Լ� Windows Server 2003 with SP1 ��ʼ����

		//ע��IsWow64() ������ʮ��׼ȷ���жϲ���ϵͳ�Ƿ��� 32 λ�� 64 λ
		//ֻ�е���(1)���������Ϊ 32 λ��������Ϊ 64 λʱ
		//        (2)ͳһ��δ���� IsWow64Process �ӿ�ʱ�Ĳ���ϵͳ�϶�Ϊʱ 32 bit
		bool is64Bit = (IsWow64() == TRUE);
		if(is64Bit)
			return "64-bit";
		else
			return "32-bit";
	}

private:	

	//�жϳ����Ƿ������� 32bit ��ģ����������
	BOOL IsWow64()
	{
		BOOL bIsWow64 = FALSE;

		//IsWow64Process is not available on all supported versions of Windows.
		//Use GetModuleHandle to get a handle to the DLL that contains the function
		//and GetProcAddress to get a pointer to the function if available.

		LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
		
		// If the process is running under 32-bit Windows, the value is set to FALSE.
		// If the process is a 32-bit application running under 64-bit Windows 10 on ARM, the value is set to FALSE. 
		// If the process is a 64-bit application running under 64-bit Windows, the value is also set to FALSE.

		if(NULL != fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
			{
				//handle error
			}
		}
		return bIsWow64;
	}

int main( void )
{
    if(IsWow64())
        _tprintf(TEXT("The process is running under WOW64.\n"));
    else
        _tprintf(TEXT("The process is not running under WOW64.\n"));

    return 0;
}

};