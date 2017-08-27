#include "stdafx.h"
#include "AutoUpdateThread.h"
#include "Define.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 
#define MAXBLOCKSIZE 1024  

using namespace SOUI;

AutoUpdateThread* AutoUpdateThread::ms_Singleton = NULL;


//��ʼ�߳�
bool AutoUpdateThread::Start()
{
	//�����߳�
	if(m_handleThread != NULL)
		return true;

	//����ʱ�䣬�¼����ź�ʱ����ʾֹͣ�ȴ�
	// ManualReset = false�� ��ʾ WaitSingleObject �յ����źŵ�  m_EventStopWaiting ��m_EventStopWaiting�Զ���Ϊ���ź�
	m_EventStopWaiting = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}

//ֹͣ�߳�
void AutoUpdateThread::Stop()
{
	if(m_handleThread == NULL)
		return;

	m_bLoop = false;

	//���� m_EventStopWaiting Ϊ���źţ��Խ��� ThreadProc �е�ѭ���ĵȴ�
	SetEvent(m_EventStopWaiting);

	::WaitForSingleObject(m_handleThread, INFINITE);

	CloseHandle(m_handleThread);
	CloseHandle(m_EventStopWaiting);
}

//�߳�ִ�е�ַ
DWORD WINAPI AutoUpdateThread::ThreadProc(LPVOID pParam)
{
	AutoUpdateThread* pThread = static_cast<AutoUpdateThread*>(pParam);
	
	
#ifdef _DEBUG
	//����ģʽ����¼��¼��Ϣ
	pThread->m_bFirstEnter = false;
#endif

	if(pThread->m_bFirstEnter)
	{
		//���͵�¼��Ϣ��¼
		pThread->SendLoginInfo();

		pThread->m_bFirstEnter = false;
	}


#if UPDATE_LOOP == 0
	pThread->m_bLoop = false;
#endif

	while(pThread->m_bLoop)
	{
		if(pThread->m_bKeepUpdate && pThread->IfUpdateAvailable())
			pThread->AutoUpdate();
		
		WaitForSingleObject(pThread->m_EventStopWaiting, pThread->m_nDelay);
		//Sleep(pThread->m_nDelay);
	}

	return 0;
}

//�Զ�����ִ�к���
bool AutoUpdateThread::AutoUpdate()
{
	wstring strVersionFile = FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_VERSION_INFO;
	wstring strLastExe =  FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_EXE_TEMP;

	/*���°汾����ֱ���������µİ汾 ִ���ļ� */
	//�����µİ汾��־�ļ� versionLog.txt ��������������Ϊ versionLog.zip��
	wstring strVersionLog =  FileHelper::GetCurrentDirectoryStr()+ L"versionLog.txt";
	if(FileHelper::CheckFileExist(strVersionLog))
		_wremove(strVersionLog.c_str());
	bool bRet = DownloadFile( LINK_VERSION_LOG, strVersionLog );
	if(bRet == false)
		return false;

	//�������µ�ִ���ļ�  BesLyric.exe��������������Ϊ BesLyricExe.zip�� �� strLastExe ��BesLyric����
	bRet = DownloadFile( LINK_LAST_EXE , strLastExe);
	if(bRet == false)
		return false;

	/*�޸��ļ����ƣ��ﵽ�滻�ɰ汾Ŀ�� */
	wstring strCurrentExe = strLastExe + L".exe";
	wstring strBackupExe =  strLastExe+ L"."+ VERSION_NUMBER ;//+ ".exe";

	if(FileHelper::CheckFileExist(strBackupExe))//ɾ�����ܴ��ڵı����ļ�
		_wremove(strBackupExe.c_str());
	_wrename(strCurrentExe.c_str(),strBackupExe.c_str());
	_wrename(strLastExe.c_str(),strCurrentExe.c_str());
	

	/*������Ϣ����ʾ�û�������ʹ���°汾 */
	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	bRet = FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	
	//��ȡ���µ����ݼ�¼
	SStringW strUpdateInfo = L"";

	if(bRet == false)
		strUpdateInfo = L"������Ϣ�����ˣ���Ҫ������Щϸ�� ��)";
	else
	{
		if(vecLineW.size() == 1 || vecLineW.size() == 2)
			strUpdateInfo = L"������Ϣ�����ˣ���Ҫ������Щϸ�� ��)";
		else
		{
			int nLine = 0;
			for(auto iter = vecLineW.begin(); iter!= vecLineW.end(); iter++,nLine++)
			{
				if(nLine >= 2)
				{
					strUpdateInfo += *iter;
					strUpdateInfo += L"\\n";
				}
			}
		}
	}
	
	SStringW strShownInfo = L"����и��£����½�����һ��������Ч  ��) \\n\\n���°汾�������ݣ�\\n";
	strShownInfo += strUpdateInfo;
	_MessageBox(NULL, strShownInfo, _T("���������ʾ"),MB_OK| MB_ICONINFORMATION);

	//��ʾһ��֮����ʱ���ټ�����
	m_bKeepUpdate = false;

	return true;
}

//����Ƿ��и���
bool AutoUpdateThread::IfUpdateAvailable()
{
	wstring strVersionFile = FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_VERSION_INFO;
	wstring strLastExe =  FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_EXE_TEMP;

	/*�������°汾������Ϣ */
	bool bRet = DownloadFile( LINK_LAST_VERSION_INFO, strVersionFile);
	if(bRet == false)
		return false;//���ʳ���

	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	if(vecLineW.begin() != vecLineW.end())
		strVersion = * vecLineW.begin();

	//ʶ������ļ����������
	if(vecLineW.size() >= 2)
	{
		SStringW secondLine = *(vecLineW.begin()+1);
		if(secondLine.GetLength()!=0)
		{
			if(secondLine.GetLength() == 6 && secondLine == L"<html>")
			{
				//Ϊҳ�治���ڵ���ʾ�����汾�ļ�������, �򲻸���
				return false;
			}
		}
	}

	if(VersionCompare(SStringW(VERSION_NUMBER.c_str()), strVersion) >= 0 ) // �汾һ�� ���� ��ǰ�汾���ڷ������汾�����ڿ���״̬��
	{
		return false;
	}

	//�и��¿���
	return true;
}

//�Ƚ�2���ַ����汾�ŵĴ�С��
int AutoUpdateThread::VersionCompare(const SStringW v1, const SStringW v2)
{
	int nMainNum1 = 0, nSubNum1= 0, nModifidNum1= 0;
	int nMainNum2 = 0, nSubNum2= 0, nModifidNum2= 0;
	swscanf(v1,L"%d.%d.%d",&nMainNum1, &nSubNum1, &nModifidNum1);
	swscanf(v2,L"%d.%d.%d",&nMainNum2, &nSubNum2, &nModifidNum2);

	if(nMainNum1 > nMainNum2)
		return 1;
	else if(nMainNum1 < nMainNum2)
		return -1;
	else//���汾����ͬ
	{
		if(nSubNum1 > nSubNum2)
		return 1;
		else if(nSubNum1 < nSubNum2)
			return -1;
		else//�ΰ汾����ͬ
		{
			if(nModifidNum1 > nModifidNum2)
				return 1;
			else if(nModifidNum1 < nModifidNum2)
				return -1;
			else
				return 0;
		}
	}

}


/*��Urlָ��ĵ�ַ���ļ����ص�save_asָ��ı����ļ�*/
bool AutoUpdateThread::DownloadFile(const wstring strUrl, const wstring strSaveAs)
{
	byte Temp[MAXBLOCKSIZE];
	ULONG Number = 1;

	FILE *stream;
	HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET handle2 = InternetOpenUrl(hSession, strUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (handle2 != NULL)
		{
			if ((_wfopen_s(&stream, strSaveAs.c_str(), L"wb")) == 0)
			{
				while (Number > 0)
				{
					InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);

					fwrite(Temp, sizeof (char), Number, stream);
				}
				fclose(stream);
			}
			else
				return false;

			InternetCloseHandle(handle2);
			handle2 = NULL;
		}
		else
			return false;
		InternetCloseHandle(hSession);
		hSession = NULL;
	}

	return true;
}




//���͵�¼��Ϣ��ip��ַ��
void AutoUpdateThread::SendLoginInfo()
{
	//���ip��ַ
	//DownloadFile(L"https://whatismyipaddress.com/",L"E://git//BesLyric//Release//ip.txt");
	//DownloadFile(L"http://ip.qq.com/",L"E://git//BesLyric//Release//ip.txt");

	//�����ip�Ĵ���
	int nMaxCheckCount = 5;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	while(nMaxCheckCount)
	{
		strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	
		bool bRet = DownloadFile(L"http://www.ip138.com/ip2city.asp",strTempFile);
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
				strIP = iter->Right(beg).Left(end);
				break;
			}
		}

		//�Ѿ����ip���˳�ѭ��
		break;
	}

	//�������ӣ�����˸����¼��¼��Ϣ
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP;
	DownloadFile(strSendLink, strTempFile);
}
