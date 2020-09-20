#include "stdafx.h"
#include "SendLoginThread.h"
#include "Define.h"
#include "../utility/Downloader.h"
#include "LinkHelper.h"
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
		auto& helper = LinkHelper::getSingleton();

		//���ip��ַ����ҳԴ
		helper.UpdateLinkFromServer();
		vector<LinkValue> ipSrc = helper.GetAllLinksIp();
		//vector<LinkValue> ipSrc;
		//ipSrc.push_back(LinkValue(L">(\\d+\\.\\d+\\.\\d+\\.\\d+)<",L"https://whatismyipaddress.com/"));
		//ipSrc.push_back(LinkValue(L"value=\"(\\d+\\.\\d+\\.\\d+\\.\\d+)\"",L"http://ip.chacuo.net/"));
		//ipSrc.push_back(LinkValue(L"����IP��(\\d+\\.\\d+\\.\\d+\\.\\d+)&nbsp;",L"http://www.zzsky.cn/code/ip/index.asp"));
		
		//����Դ�����ip�Ĵ���
		int nMaxSingleCheckCount = 3;
	
		int nLeftCheckCount = nMaxSingleCheckCount * ipSrc.size(); // �����ܹ�������
		while(nLeftCheckCount-1)
		{
			strTempFile =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_TEMP;
			if(FileHelper::CheckFileExist(strTempFile))
				_wremove(strTempFile.c_str());
	
			LinkValue linkValue = ipSrc[(nLeftCheckCount-1) / nMaxSingleCheckCount];

			bool bRet = CDownloader::DownloadFile(linkValue.link,strTempFile);
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
				wstring ipStr = L"Invalid IP query source";
				
				if(CatchIPStr(iter->GetBuffer(1),linkValue.value, ipStr))
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
bool SendLoginThread::CatchIPStr(const wstring &line,const wstring &rule, OUT wstring& ip)
{
	std::locale loc("");    
    std::wcout.imbue(loc);    
    std::wstring regString(rule); 

	// �ο� https://blog.csdn.net/effective_coder/article/details/9010337

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

