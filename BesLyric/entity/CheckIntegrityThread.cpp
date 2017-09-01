#include "stdafx.h"
#include "CheckIntegrityThread.h"
#include "AutoUpdateThread.h"
#include "Define.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"


using namespace SOUI;

CCheckIntegrityThread* CCheckIntegrityThread::ms_Singleton = NULL;


//��ʼ�߳�
bool CCheckIntegrityThread::Start(bool bShowPassTip)
{
	if(m_bIsChecking)
	{
		_MessageBox(NULL,L"�Ѿ����ڼ�⵱��.....",L"�����ʾ", MB_OK|MB_ICONINFORMATION);

		if(m_handleThreadUI!=0)
			CloseHandle(m_handleThreadUI);

		m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);
		
		::WaitForSingleObject(m_EventWndInitDone, INFINITE); //�ȴ�����׼�����
		RestoreProcess();
		return true;
	}

	m_bShowPassTip = bShowPassTip;
	
	//�����¼����¼����ź�ʱ����ʾֹͣ�ȴ�
	// ManualReset = false�� ��ʾ WaitSingleObject �յ����źŵ�  m_EventWndInitDone ��m_EventWndInitDone�Զ���Ϊ���ź�
	m_EventWndInitDone = ::CreateEvent(NULL, FALSE, FALSE,NULL);


	//��������߳�
	m_handleThreadCheking = ::CreateThread(NULL, 0, ProcChecking, this, 0 ,NULL);
	m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);

	return (m_handleThreadCheking != NULL && m_handleThreadUI!=NULL);
}


//�����߳�
void CCheckIntegrityThread::Stop()
{
}

//��⣨�����أ� �߳�ִ�е�ַ
DWORD WINAPI CCheckIntegrityThread::ProcChecking(LPVOID pParam)
{
	CCheckIntegrityThread* pThread = static_cast<CCheckIntegrityThread*>(pParam);

	pThread->m_bIsChecking = true;

	::WaitForSingleObject(pThread->m_EventWndInitDone, INFINITE); //�ȴ�����׼�����

	pThread->CheckFFmpeg();
	pThread->m_bIsChecking = false;
	return 0;
}


//UI �߳�ִ�е�ַ
DWORD WINAPI CCheckIntegrityThread::ProcUI(LPVOID pParam)
{
	CCheckIntegrityThread* pThread = static_cast<CCheckIntegrityThread*>(pParam);
	
	//��һ�����ڣ�������ʾ������ �� �ṩȡ������
	DlgCheckIntegrity dlg(L"xml_check_integrity");

	dlg.SetEventHandleForWndHandle(pThread->m_EventWndInitDone, &pThread->m_hCheckWnd, pThread);

	int ret = dlg.DoModal(NULL);

	return 0;
}


//���ffmpeg.exe �Ƿ���ڣ�������������
bool CCheckIntegrityThread::CheckFFmpeg()
{
	//���ffpmeg�Ƿ����
	wstring strDir = FileHelper::GetCurrentDirectoryStr() + TEMP_WAV_FLODER_NAME ;
	wstring strFfmpeg = strDir + L"\\ffmpeg.exe";
	
	UpdateProgressUI(0, wstring(L"��� "+strFfmpeg + L"�Ƿ����...").c_str());

	if(!FileHelper::CheckFileExist(strFfmpeg)) 
	{
		UpdateProgressUI(5, wstring(L"��� "+strDir + L"�Ƿ����...").c_str());

		//������ffpmeg.exe�� �ȱ�֤�ļ�Ŀ¼����
		if(!FileHelper::CheckFolderExist(strDir))
		{
			if(RET_SUCCEEDED != _wmkdir(strDir.c_str()))
			{
				UpdateProgressUI(100, L"������(�޷�����Ŀ¼)");

				wstring strTip = L"�����޷�����Ŀ¼��\\n";
				strTip += strDir +L"\\n";
				strTip += L"�⽫���²���mp3�ļ������ܡ��޷���������\\n";
				
				_MessageBox(NULL, strTip.c_str(), L"��ʾ", MB_OK|MB_ICONWARNING);
				return false;
			}
		}
		
		UpdateProgressUI(60, wstring( L"����ת���� ffmpeg(34.84 MB)�������ĵȴ� ...").c_str());
		//�ȴ� LINK_DOWNLOAD_SERVER ���� ffmpeg
		bool bFirstTrySucceed = AutoUpdateThread::DownloadFile(LINK_DOWNLOAD_SERVER + L"ffmpeg.exe", strFfmpeg);
		if(!bFirstTrySucceed)//���ز��ɹ�
		{
			//�������ط���
			//��ʼ�ӷ���������ffmpeg
			wstring strNameExt = L"ffmpeg.ext"+ SERVER_FILE_EXTENTION_W;
			UpdateProgressUI(10,wstring( L"�������� "+ strNameExt +L" ...").c_str());

			wstring strFileExt = strDir +L"\\"+ strNameExt;
			wstring strLinkExt = LINK_SERVER_PATH + strNameExt;

			//���� ffmpeg.ext.zip
			bool bRet = AutoUpdateThread::DownloadFile(strLinkExt, strFileExt);
		

			//���� ffmpeg.1.zip - ffmpeg.4.zip 4���ļ�
			WCHAR szBuffer[MAX_BUFFER_SIZE/2];
			if(false != bRet)
				for(auto i=1; i<=4 ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					UpdateProgressUI(10 + i*(90-10)/4, wstring(L"�������� "+strNameSplited +L"...").c_str());

					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					wstring strLinkSplited = LINK_SERVER_PATH + strNameSplited;

					bRet = AutoUpdateThread::DownloadFile(strLinkSplited, strFileSplited);
					if(false == bRet)
						break;
				}

			//��� ffmpeg.ext.zip �� ffmpeg.1.zip - ffmpeg.4.zip 5���ļ��Ƿ��������
			bool bHaveDownload = true;
			if(!FileHelper::CheckFileExist(strFileExt))
				bHaveDownload = false;
		
			for(auto i=1; i<=4 ;i++)
			{
				wstring strFileSplited = strDir + wstring(L"\\ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
				if(!FileHelper::CheckFileExist(strFileExt))
				{
					bHaveDownload = false;
					break;
				}
			}

			if(false == bRet)
				bHaveDownload = false;
		
			if(!bHaveDownload)
			{
				UpdateProgressUI(100, L"������(ȱʧ�ļ���");

				//��ʾ�û�ȷʵ ת���ļ�������ʾ��δ���
				wstring strTip = L"ȱ���ļ���\\n";
				strTip+= strFfmpeg + L"\\n\\n";
			
				strTip+= wstring(L"����ԭ��\\n ");
				strTip+= wstring(L"1����������ʱ�������޷��������磬�Զ�����\\n");
				strTip+= wstring(L"2��ʹ�ù����У�ɾ�����ƶ���ffmpeg.exe �ļ�\\n");
				strTip+= wstring(L"3�����������쳣\n\n");

				strTip+= wstring(L"���������\\n");
				strTip+= wstring(L"1�����������������������,Ȼ������������ҳ����С������Լ�⡱\\n");
				strTip+= wstring(L"2���ֶ�����ffmpeg.exe �ļ������ʳ��򡰱������ҳ��Ĳ������ӣ��鿴��4С�ڡ��������⡱���ؽ̳�\\n");
				strTip+= wstring(L"");

				_MessageBox(NULL,strTip.c_str(), L"��������ʾ", MB_OK|MB_ICONWARNING);
				return false;
			}
			else
			{
				UpdateProgressUI(90, L"�ϲ����ص��ļ�,����ffmpeg.exe ...");
				//�Ѿ�������ϣ��ϲ��ļ��õ� ffmpeg.exe
				bRet = CSplitFile::MergeFile(strDir,L"ffmpeg", strDir);
				bool bFail = false;
				if(!bRet)
				{
					UpdateProgressUI(100, L"������(�����ļ� ������ ffmpeg.exe ʧ�ܣ�");

					wstring strTip = L"�޷����������ļ���\\n";
					strTip += strFfmpeg + L"\\n\n";			
				
					strTip+= wstring(L"����ԭ��\\n ");
					strTip+= wstring(L"1����������粻�ȶ�(�����������)�������ļ����ز�����\\n");
					strTip+= wstring(L"2���������粻�ȵ�\\n\\n");

					strTip+= wstring(L"���������\\n");
					strTip+= wstring(L"1������������ҳ����С������Լ�⡱\\n");
					strTip+= wstring(L"2���ֶ�����ffmpeg.exe �ļ������ʳ��򡰱������ҳ��Ĳ������ӣ��鿴��4С�ڡ��������⡱�ֶ����� ffmpeg.exe\\n");
					strTip+= wstring(L"");
					_MessageBox(NULL,strTip.c_str(), L"��ʾ", MB_OK|MB_ICONWARNING);
					bFail = true;
				}

				//ɾ����ʱ���ص��ļ�
				if(!bFail)
					UpdateProgressUI(95, L"ɾ����ʱ�ļ�...");

				_wremove(strFileExt.c_str());
				for(auto i=1; i<=4 ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					_wremove(strFileSplited.c_str());
				}

				if(bFail)
					return false;
			}
		}
	}
	
	UpdateProgressUI(100, L"������ɣ�");
	
	CloseProgressUI();

	if(m_bShowPassTip)
	{
		_MessageBox(NULL, L"��ǰ����������", L"��������ʾ", MB_OK|MB_ICONINFORMATION);
	}

	return true;
}


//����UI����
void CCheckIntegrityThread::UpdateProgressUI(int nPercentage, const WCHAR* pszTip)
{
	m_nPercentage = nPercentage;
	wcscpy_s(m_szTip, MAX_BUFFER_SIZE ,pszTip);
	::SendMessageW(m_hCheckWnd, MSG_USER_UPDATE_CHECK_PROGRESS, (WPARAM)nPercentage,(LPARAM)pszTip);
}

void CCheckIntegrityThread::RestoreProcess()
{
	::SendMessageW(m_hCheckWnd, MSG_USER_UPDATE_CHECK_PROGRESS, (WPARAM)m_nPercentage,(LPARAM)m_szTip);
}


void CCheckIntegrityThread::CloseProgressUI()
{
	::SendMessageW(m_hCheckWnd, MSG_USER_CLOSE_CHECK_PROGRESS, (WPARAM)m_nPercentage,(LPARAM)m_szTip);
}