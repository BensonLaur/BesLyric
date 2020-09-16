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

		return GetSystemBits();
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
	string GetSystemBits()
    {
       SYSTEM_INFO si;
	   SafeGetNativeSystemInfo(&si);
	   if (si.wProcessorArchitecture == MS_PROCESSOR_ARCHITECTURE_AMD64 ||
		  si.wProcessorArchitecture == MS_PROCESSOR_ARCHITECTURE_IA64 )
	   {
	      return "64 bit";
	   }
	   return "32 bit";
	}

 

};