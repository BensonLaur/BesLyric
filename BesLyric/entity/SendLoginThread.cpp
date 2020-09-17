#include "stdafx.h"
#include "SendLoginThread.h"
#include "Define.h"
#include "../utility/Downloader.h"
#include "OsInfoHelper.hpp"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

using namespace SOUI;

#include <regex>		//ʹ��������ʽƥ��IP

//��ʼ�߳�
bool SendLoginThread::Start(BOOL bAnonymity)
{
	//�����߳�
	if(m_handleThread != NULL)
		return true;

	m_bAnonymity = bAnonymity;
	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}

//�߳�ִ�е�ַ
DWORD WINAPI SendLoginThread::ThreadProc(LPVOID pParam)
{
	SendLoginThread* pThread = static_cast<SendLoginThread*>(pParam);


#ifdef _DEBUG  

	//����ģʽ�²����͵�¼��Ϣ

	//���͵�¼��Ϣ��¼
	pThread->SendLoginInfo(pThread->m_bAnonymity);

#endif

	return 0;
}

//���͵�¼��Ϣ��ip��ַ��
void SendLoginThread::SendLoginInfo(BOOL bAnonymity)
{
	//���ip��ַ����ҳԴ��3������
	const int SrcCount = 3;
	vector<wstring> ipSrc;
	ipSrc.push_back(L"http://ip.chacuo.net/");
	ipSrc.push_back(L"http://www.zzsky.cn/code/ip/index.asp");
	ipSrc.push_back(L"https://whatismyipaddress.com/");

	//����Դ�����ip�Ĵ���
	int nMaxSingleCheckCount = 3;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	if(bAnonymity)
	{
		strIP = L"Anonymity";
		strTempFile =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_TEMP;
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	}
	else
	{
		int nLeftCheckCount = nMaxSingleCheckCount * SrcCount; // �����ܹ�������
		while(nLeftCheckCount-1)
		{
			strTempFile =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_TEMP;
			if(FileHelper::CheckFileExist(strTempFile))
				_wremove(strTempFile.c_str());
	
			wstring url = ipSrc[(nLeftCheckCount-1) / nMaxSingleCheckCount];
			bool bRet = CDownloader::DownloadFile(url,strTempFile);
			if(bRet == false)
			{
				//����û���磬�������쳣��Ҳ���ܶ�ȡ�ļ�ʧ��
				//�ȴ�5���ټ��
				nLeftCheckCount--;
				Sleep(5000);
				continue;
			}

			vector<SStringW> vecLine;
			bRet = FileOperator::ReadAllLinesW(strTempFile,&vecLine);
			if(bRet == false)
			{
				//�ļ���ȡ�쳣��5������¼�����
				nLeftCheckCount--;
				Sleep(5000);
				continue;
			}

			bool bFound = false;
			for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
			{
				// https://whatismyipaddress.com/ �� http://ip.qq.com/ ����ҳ��IP���� >ddd.ddd.ddd.ddd< ���������Ҹ��������ַ���
				wstring ipStr = L"Invalid IP query source";
				
				if(CatchIPStr(iter->GetBuffer(1), ipStr))
				{
					strIP = ipStr;
					bFound = true;
					break;
				}
			}

			if(bFound)
				break;//�Ѿ����ip���˳�ѭ��
			
			nLeftCheckCount--;
		}
	}

	wstring systemArch = S_CA2W(OsInfoHelper().GetOsBits().c_str()).GetBuffer(64);

	//�������ӣ�����˸����¼��¼��Ϣ
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP + L"&version=beslyric-soui&vernum="+VERSION_NUMBER 
		+ L"&systemArchitecture=" + systemArch;
	CDownloader::DownloadFile(strSendLink, strTempFile);
}


//�������������IP�ַ���
bool SendLoginThread::CatchIPStr(const wstring &line, OUT wstring& ip)
{
	//(http://ip.chacuo.net/)
	//value="..."

	// (https://whatismyipaddress.com/)
	// > . . . <

	//�� (http://www.zzsky.cn/code/ip/index.asp)
	//   ����IP��...&nbsp;

	// �ο� https://blog.csdn.net/effective_coder/article/details/9010337
	std::locale loc("");    
    std::wcout.imbue(loc);    
        
    std::wstring regString1(_T("value=\"(\\d+\\.\\d+\\.\\d+\\.\\d+)\"")); 

    std::wstring regString2(_T(">(\\d+\\.\\d+\\.\\d+\\.\\d+)<")); 

    std::wstring regString3(_T("����IP��(\\d+\\.\\d+\\.\\d+\\.\\d+)&nbsp;"));   
    
	//�Ȳ��ҹ���1
    if(GetIpByRegString(line, regString1, ip))
		return true;

	//�Ȳ��ҹ���2
    if(GetIpByRegString(line, regString2, ip))
		return true;
	
	//���ҹ���3
	if( GetIpByRegString(line, regString3, ip))
		return true;
	
	return false;
}


bool SendLoginThread::GetIpByRegString(const wstring &line, std::wstring regString, OUT wstring& ip)
{
	// ���ʽѡ�� - ���Դ�Сд     
    std::regex_constants::syntax_option_type fl = std::regex_constants::icase;    
     
    // ����һ��������ʽ���     
    std::wregex regExpress(regString, fl);    

	// ������ҵĽ��     
    std::wsmatch ms;    
    
    // ����     
    if(std::regex_search(line, ms, regExpress))    
    {    
		if(ms.size() == 2)
		{
            ip = ms.str(1); 
			return true;
		}
		else
			return false;
    }    
    else    
    {    
        return false;
    }    
}
