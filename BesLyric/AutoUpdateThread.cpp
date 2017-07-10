#include "stdafx.h"
#include "AutoUpdateThread.h"

#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 
#define MAXBLOCKSIZE 1024  

using namespace SOUI;
extern int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType);

AutoUpdateThread* AutoUpdateThread::ms_Singleton = NULL;

//��ʼ�߳�
bool AutoUpdateThread::Start()
{
#ifdef _DEBUG
	//����ģʽ����¼��¼��Ϣ
	m_bFirstEnter = false;
#endif

	if(m_bFirstEnter)
	{
		//���͵�¼��Ϣ��¼
		SendLoginInfo();

		m_bFirstEnter = false;
	}

	//�����߳�
	if(m_handleThread != NULL)
		return true;

	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}


//�����߳�
DWORD AutoUpdateThread::Pause()
{
	return ::SuspendThread(m_handleThread);
}

//�ָ��߳�
DWORD AutoUpdateThread::Resume()
{
	return ::ResumeThread(m_handleThread);
}

//ֹͣ�߳�
void AutoUpdateThread::Stop()
{
	if(m_handleThread == NULL)
		return;

	m_bKeepUpdate = false;


	::WaitForSingleObject(m_handleThread, INFINITE);

	CloseHandle(m_handleThread);
}

//�߳�ִ�е�ַ
DWORD WINAPI AutoUpdateThread::ThreadProc(LPVOID pParam)
{
	AutoUpdateThread* pThread = static_cast<AutoUpdateThread*>(pParam);
	
#if _KEEP_UPDATE == 0
	pThread->m_bKeepUpdate = false;
#endif

	while(pThread->m_bKeepUpdate)
	{
		//�����û����
		if(!pThread->m_bHasUpdate)
			pThread->AutoUpdate();
		
		Sleep(pThread->m_nDelay);
	}

	return 0;
}

//�Զ�����ִ�к���
bool AutoUpdateThread::AutoUpdate()
{
	bool bRet;
	string strVersionFile = FileHelper::GetCurrentDirectoryStr()+ "version";
	string strLastExe =  FileHelper::GetCurrentDirectoryStr()+ "BesLyric";

	/*�������°汾������Ϣ */
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/lastVersion.zip", wstring(S_CA2W( SStringA(strVersionFile.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	if(vecLineW.begin() != vecLineW.end())
		strVersion = * vecLineW.begin();

	//ʶ���ļ����������
	if(vecLineW.size() >= 2)
	{
		SStringW secondLine = *(vecLineW.begin()+1);
		if(secondLine.GetLength()!=0)
		{
			if(secondLine.GetLength() == 6 && secondLine == L"<html>")
			{
				//Ϊҳ�治���ڵ���ʾ�����汾�ļ������ڣ�����Ϊ�Ѹ���
				m_bHasUpdate = true;

				return false;
			}
		}
	}

	if(VersionCompare(VERSION_NUMBER, strVersion) >= 0 ) // �汾һ�� ���� ��ǰ�汾���ڷ������汾�����ڿ���״̬��
	{
		m_bHasUpdate = true;
		return true;
	}
	
	/*���°汾����ֱ���������µİ汾 ִ���ļ� */
	//�����µİ汾��־�ļ� versionLog.txt ��������������Ϊ versionLog.zip��
	string strVersionLog =  FileHelper::GetCurrentDirectoryStr()+ "versionLog.txt";
	if(FileHelper::CheckFileExist(strVersionLog))
		remove(strVersionLog.c_str());
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/versionLog.zip",wstring(S_CA2W( SStringA(strVersionLog.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	//�������µ�ִ���ļ�  BesLyric.exe��������������Ϊ BesLyricExe.zip�� �� strLastExe ��BesLyric����
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/BesLyricExe.zip",wstring(S_CA2W( SStringA(strLastExe.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	/*�޸��ļ����ƣ��ﵽ�滻�ɰ汾Ŀ�� */
	string strCurrentExe = strLastExe + ".exe";
	string strBackupExe =  strLastExe+ "."+ S_CW2A(VERSION_NUMBER).GetBuffer(1) ;//+ ".exe";

	if(FileHelper::CheckFileExist(strBackupExe))//ɾ�����ܴ��ڵı����ļ�
		remove(strBackupExe.c_str());
	rename(strCurrentExe.c_str(),strBackupExe.c_str());
	rename(strLastExe.c_str(),strCurrentExe.c_str());
	

	/*������Ϣ����ʾ�û�������ʹ���°汾 */
	//��ȡ���µ����ݼ�¼
	SStringW strUpdateInfo = L"";
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
	
	SStringW strShownInfo = L"����и��£����½�����һ��������Ч  ��) \\n\\n���°汾�������ݣ�\\n";
	strShownInfo += strUpdateInfo;
	_MessageBox(NULL, strShownInfo, _T("���������ʾ"),MB_OK| MB_ICONINFORMATION);



	//����Ϊ�Ѹ���
	m_bHasUpdate = true;

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

	string strTempFile =  FileHelper::GetCurrentDirectoryStr()+ "temp";
	if(FileHelper::CheckFileExist(strTempFile))
		remove(strTempFile.c_str());
	DownloadFile(L"http://www.ip138.com/ip2city.asp",wstring(S_CA2W( SStringA(strTempFile.c_str()).GetBuffer(1) )));
	
	string strIP="";
	vector<string>	vecLine;
	FileOperator::ReadAllLines(strTempFile,&vecLine);
	for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
	{
		if(iter->find_first_of('[') != string::npos)
		{
			auto beg = iter->find_first_of('[')+1;
			auto end = iter->find_first_of(']');
			strIP = iter->substr(beg, end - beg);
			break;
		}
	}

	//�������ӣ�����˸����¼��¼��Ϣ
	string strSendLink = "http://bensonlaur.vicp.io/BesBlog/beslyric/login.action?ip=" + strIP;
	DownloadFile(wstring(S_CA2W( SStringA(strSendLink.c_str()).GetBuffer(1) )),wstring(S_CA2W( SStringA(strTempFile.c_str()).GetBuffer(1) )));
}
