#include "stdafx.h"
#include "SendLoginThread.h"
#include "Define.h"
#include "../utility/Downloader.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

using namespace SOUI;


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


#ifndef _DEBUG  

	//����ģʽ�²����͵�¼��Ϣ

	//���͵�¼��Ϣ��¼
	pThread->SendLoginInfo(pThread->m_bAnonymity);

#endif

	return 0;
}

//���͵�¼��Ϣ��ip��ַ��
void SendLoginThread::SendLoginInfo(BOOL bAnonymity)
{
	//���ip��ַ
	//DownloadFile(L"https://whatismyipaddress.com/",L"E://git//BesLyric//Release//ip.txt");
	//DownloadFile(L"http://ip.qq.com/",L"E://git//BesLyric//Release//ip.txt");

	//�����ip�Ĵ���
	int nMaxCheckCount = 5;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	if(bAnonymity)
	{
		strIP = L"Anonymity";
		strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	}
	else
	{
		while(nMaxCheckCount)
		{
			strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
			if(FileHelper::CheckFileExist(strTempFile))
				_wremove(strTempFile.c_str());
	
			bool bRet = CDownloader::DownloadFile(L"http://www.ip138.com/ip2city.asp",strTempFile);
			if(bRet == false)
			{
				//����û���磬�������쳣��Ҳ���ܶ�ȡ�ļ�ʧ��
				//�ȴ�5���ټ��
				nMaxCheckCount--;
				Sleep(5000);
				continue;
			}

			vector<SStringW> vecLine;
			bRet = FileOperator::ReadAllLinesW(strTempFile,&vecLine);
			if(bRet == false)
			{
				//�ļ���ȡ�쳣��5������¼�����
				nMaxCheckCount--;
				Sleep(5000);
				continue;
			}

			for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
			{
				if(iter->Find(L'[') != -1)
				{
					auto beg = iter->Find(L'[')+1;
					auto end = iter->Find(L']');
					strIP = iter->Left(end);
					strIP = strIP.substr(beg);
					break;
				}
			}

			//�Ѿ����ip���˳�ѭ��
			break;
		}
	}

	//�������ӣ�����˸����¼��¼��Ϣ
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP;
	CDownloader::DownloadFile(strSendLink, strTempFile);
}
