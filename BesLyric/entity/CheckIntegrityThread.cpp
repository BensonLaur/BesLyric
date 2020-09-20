#include "stdafx.h"
#include "CheckIntegrityThread.h"
#include "AutoUpdateThread.h"
#include "Define.h"

#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"
#include "LinkHelper.h"

#include <fstream> 
using namespace std;

using namespace SOUI;

//��ʼ�߳�
bool CCheckIntegrityThread::Start(bool bShowPassTip)
{
	if(m_bIsChecking)
	{
		if(m_bIsProcUIClose) //֮ǰ�Ѿ��ر���
		{
			if(m_handleThreadUI!=0)
				CloseHandle(m_handleThreadUI);

			m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);
		
			::WaitForSingleObject(m_EventWndInitDone, INFINITE); //�ȴ�����׼�����

			RestoreProcess();
		}

		SwitchToThisWindow(m_hCheckWnd,TRUE);  // �ڶ���������bRestore �� �������С������ָ�����

		return true;
	}

	m_bShowPassTip = bShowPassTip;
	
	//�����¼����¼����ź�ʱ����ʾֹͣ�ȴ�
	// ManualReset = false�� ��ʾ WaitSingleObject �յ����źŵ�  m_EventWndInitDone ��m_EventWndInitDone�Զ���Ϊ���ź�
	m_EventWndInitDone = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	m_EventUpdateDownloadDone = ::CreateEvent(NULL, TRUE, FALSE,NULL);

	//��������߳�
	m_handleThreadCheking = ::CreateThread(NULL, 0, ProcChecking, this, 0 ,NULL);

	//����UI�߳�
	m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);

	return (m_handleThreadCheking != NULL);
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

	//������update ������Ƿ���Ҫ����
	pThread->DownloadUpdateFileAndMark();
	
	//�����ضԱ��꽫�������temp������������
	::SetEvent(pThread->m_EventUpdateDownloadDone);
	
	::WaitForSingleObject(pThread->m_EventWndInitDone, INFINITE); //�ȴ�����׼����ϣ����ܽ��н������ĸ��¹���

	//�ӷ�����������������
	LinkHelper::getSingleton().UpdateLinkFromServer();

	pThread->CheckUpdateFile();	//������е�dll�Ƿ�Ϊ����
	pThread->CheckFFmpeg();		//��� FFmpeg.exe �Ƿ��Ѵ���

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

	
	::WaitForSingleObject(pThread->m_EventUpdateDownloadDone, INFINITE); //�ȴ�update �ļ��������,�ŵ�������
	
	pThread->m_bIsProcUIClose = false;  //���δ�ر�
	
	if(pThread->PreCheckWhetherNeedToShowCheckDialog())
		dlg.DoModal(NULL);
	else
		::SetEvent(pThread->m_EventWndInitDone);  //����Ҫ���򣬵��� ProcChecking ��ִ�м��ǰ����Ҫ����� �¼���������������һ���¼���Ч

	pThread->m_bIsProcUIClose = true;  //����Ѿ��ر�
	return 0;
}

 //Ԥ�ȼ���Ƿ���Ҫ��ʾ���dialog
bool CCheckIntegrityThread::PreCheckWhetherNeedToShowCheckDialog()
{
	bool bNeed = false;

	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	wstring buffer;
	if(!FileOperator::ReadAllText(strTemp, buffer))
		return false;
	
	if(buffer[0] == L'1')		//�и��±��
		bNeed = true;
	else
	{
		//û�и��±�־���ٿ���ffmpeg
		//���ffpmeg�Ƿ����
		wstring strDir = FileHelper::GetCurrentDirectoryStr() + TEMP_WAV_FLODER_NAME ;
		wstring strFfmpeg = strDir + L"\\ffmpeg.exe";
	
		bool bFileExist =  FileHelper::CheckFileExist(strFfmpeg);
		string strMd5;
		bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
		if(!bFileExist || !bRet || 
			(bFileExist && !updateHelper.IsValidFFmpegMd5(strMd5))) //��ⷢ�ֲ�һ�£�������Ҫ����
			bNeed = true;
	}	
	return bNeed;
}

//��������ļ��Ƿ�Ϊ����
bool CCheckIntegrityThread::CheckUpdateFile()
{
	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	wstring buffer;
	if(!FileOperator::ReadAllText(strTemp, buffer))
		return false;
	
	if(buffer[0] == L'1')
	{		
		//Ϊ�˷�ֹ������;�쳣�˳����������´μ�������Ϊ��һ������ɸ��£�ͨ�� lastUpdateDone �������Ƿ���ĸ������
		//д�� lastUpdateDone �ļ��� 0 ��ʾδ������һ�εĸ���
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"lastUpdateDone",L"0");  

		//���汾 v2.1.11 ������ĸ��´���
		wstring strLastExe = L"";
		if(AutoUpdateThread::VersionCompare( VERSION_NUMBER.c_str(), L"2.1.11") == 0)
		{
			UpdateProgressUI(2, wstring( L"����Ϊ �汾 v2.1.11 ������ĸ��´���...").c_str());
			
			//�汾 2.1.11 ���� ���� soui 2.2 �汾�ķ�֧�����һ���汾������÷�֧ ��Sun Apr 22 14:20:12 2018 +0800�����ύ��Ϣ
			
			//������ʵʩ�����������֮ǰ����Ҫ�ȸ��� ��ǰ�� exe �ļ�Ϊ soui 2.6 ��������� exe
			strLastExe =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_LAST_EXE_TEMP;
	
			//�������µ�soui 2.6 ��������� exe,  BesLyric.exe��������������Ϊ BesLyricExe.rar�� �� strLastExe ��BesLyric����
			bool bRet = CDownloader::DownloadFile( LINK_LAST_EXE_2 , strLastExe);
			if(bRet == false)
			{
				UpdateProgressUI(100, wstring( L"�����ļ�ʧ�ܣ����������쳣...").c_str());
				return false;
			}
		}

		UpdateProgressUI(5, wstring( L"��ȡ������Ŀ���ݣ������ĵȴ� ...").c_str());

		//��ȡ�����ļ��õ����д�У����ļ���Ϣ
		vector<UpdateItem> updateItems;
		if(!updateHelper.GetUpdateItem(updateHelper.GetEtcDir() + L"update", updateItems))			
		{
			UpdateProgressUI(100, wstring( L"��ȡ��������ʧ��...").c_str());
			return false;
		}

		//����Աȣ������������ش������������������������滻

		//�ռ���Ҫ���µ���
		vector<UpdateItem> updateList;
		for(auto iter = updateItems.begin(); iter != updateItems.end(); iter++)
		{
			wstring taget = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			
			//���㱾��md5 
			string md5;
			updateHelper.GetFileMd5(taget, md5);

			if(md5 != iter->md5)
			{
				//��Ҫ��¼���ļ�
				updateList.push_back(*iter);
			}
		}
		
		UpdateProgressUI(30, wstring( L"����������Ҫ���µ��� ...").c_str());
		//�����tempDir��

		wstring strTempDir = updateHelper.GetEtcDir() + L"tempDir\\" ;

		int nCount = 0;
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			nCount += (90-30) / int(updateList.size());
			UpdateProgressUI(35+nCount, (wstring( L"��������") + iter->fileName).c_str());

			//��ȷ�� ��ʱĿ¼ ��iter->local Ŀ¼����
			if(!updateHelper.MakeSureRelativeLocalExist(strTempDir, iter->local))
			{
				UpdateProgressUI(100, wstring( L"������ʱĿ¼ʧ�ܣ��������ʧ��") .c_str());
				return false;
			}

			//���ص����λ�� iter->local ��
			wstring strTarget = strTempDir + iter->local + iter->fileName;

			bool fail = false;
			if(!CDownloader::DownloadFile(iter->link, strTarget))
				fail = true;
			else
			{
				//���������������ļ��� md5
				string md5;
				updateHelper.GetFileMd5(strTarget, md5);
				if(md5 != iter->md5)
					fail = true;
			}

			if(fail)
			{
				_MessageBox(NULL, L"�޷����ش������ļ�������ʧ��", L"��ʾ", MB_OK|MB_ICONWARNING);
				UpdateProgressUI(100, wstring( L"�������ʧ��") .c_str());
				return false;
			}
		}

		UpdateProgressUI(95, wstring( L"�����滻���...") .c_str());
		vector<wstring> oldFileToDelete;

		//���������еľ��ļ�
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			wstring target = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			wstring targetTemp = target + L".temp";
			_wrename(target.c_str(), targetTemp.c_str());
			oldFileToDelete.push_back(iter->local + iter->fileName +  L".temp");
		}

		//�����صĵ�tempDir���ļ����Ƶ�Ŀ��λ��
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			if(!updateHelper.MakeSureRelativeLocalExist(FileHelper::GetCurrentDirectoryStr(), iter->local))
			{
				UpdateProgressUI(100, wstring( L"����Ŀ¼ʧ�ܣ��������ʧ��") .c_str());
				return false;
			}
			wstring target = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			wstring targetTemp =  updateHelper.GetEtcDir() + L"tempDir\\" + iter->fileName;
			CopyFileW(targetTemp.c_str(), target.c_str(), FALSE);
		}

		//���������´�������ɾ��
		wstring strContent;
		for(auto iterOld = oldFileToDelete.begin(); iterOld != oldFileToDelete.end(); iterOld++)
		{
			strContent += *iterOld + L"\n";
		}
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"fileToDelete",strContent);  


		//���汾 v2.1.11 ������ĸ��´���//�����Ӧ�Ĳ���Ϊ���أ�Ϊ�˸�����ʵ�ʵ��ȫ���滻����������������������ڴ��滻exe�ļ�
		if(AutoUpdateThread::VersionCompare( VERSION_NUMBER.c_str(), L"2.1.11") == 0)
		{
			/*�޸��ļ����ƣ��ﵽ�滻�ɰ汾Ŀ�� */
			wstring strCurrentExe = FileHelper::GetCurrentDirectoryStr() + FILE_NAME_LAST_EXE_TEMP + L".exe";
			wstring strBackupExe =  FileHelper::GetCurrentDirectoryStr() + FILE_NAME_LAST_EXE_TEMP + L"."+ VERSION_NUMBER ;//+ ".exe";

			if(FileHelper::CheckFileExist(strBackupExe))//ɾ�����ܴ��ڵı����ļ�
				_wremove(strBackupExe.c_str());
			_wrename(strCurrentExe.c_str(),strBackupExe.c_str());

			//�����µ�exe��ԭ��Ŀ¼
			CopyFileW(strLastExe.c_str(), strCurrentExe.c_str(), FALSE);
		}

		//д�� lastUpdateDone �ļ��� 1 ��ʾ��������һ�εĸ���
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"lastUpdateDone",L"1"); 
	}
	
	UpdateProgressUI(100, wstring( L"����������") .c_str());

	return true;
}

//���ظ����ļ�Ȼ�����Ƿ���Ҫ����
bool CCheckIntegrityThread::DownloadUpdateFileAndMark()
{
	//�������ļ��Լ�
	updateHelper.UpdateItemSelfCheck();

	//д�� needUpdate �ļ��� 1 ��ʾ��Ҫ��0 ��ʾ����Ҫ
	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	FileOperator::WriteAllText(strTemp,L"0");  //��д��һ������Ҫ����updateItem���

	//��֤temp�ļ��д���
	wstring strTempDir = updateHelper.GetEtcDir() + L"tempDir";
		
	// �ȱ�֤temp�ļ�Ŀ¼����
	if(!FileHelper::CheckFolderExist(strTempDir))
	{
		if(RET_SUCCEEDED != _wmkdir(strTempDir.c_str()))
		{
			wstring strTip = L"�����޷�����Ŀ¼��\\n";
			strTip += strTempDir;
			_MessageBox(NULL, strTip.c_str(), L"��ʾ", MB_OK|MB_ICONWARNING);
			return false;
		}
	}

	//��ʱ���ص�update�ļ�·��
	wstring strTempUpdate = updateHelper.GetEtcDir() + L"tempDir\\update.xml" ;
	if(!CDownloader::DownloadFile(LINK_UPDATE_ITEM_FILE, strTempUpdate))
	{
		//����ʾʧ�ܣ���Ȼÿ�β�������������ʾ��һ��
		//_MessageBox(NULL, L"�޷�����update.xml�ļ���������ʧ��", L"��ʾ", MB_OK|MB_ICONWARNING);
		return false;
	}

	if(!updateHelper.IsValidXml(strTempUpdate))
	{
		return false;
	}

	//����2��update�ļ���md5,��ͬ�������Ҫ����������Ҫ�������update item���ļ��Ƿ���Ҫ����

	wstring strUpdate = updateHelper.GetEtcDir() + L"update" ;
	if(!FileHelper::CheckFileExist(strUpdate)) 
	{
		//�ļ�������
		FileOperator::WriteAllText(strTemp,L"1");  //д��һ����Ҫ����updateItem���
	}
	else
	{
		//�Ƚ�2���ļ�md5
		string md5Now;
		string md5Temp;
		bool bRet = updateHelper.GetFileMd5(strUpdate, md5Now);
		bRet = updateHelper.GetFileMd5(strTempUpdate, md5Temp);
		bool bNeedToUpdate = false;

		if(md5Now == md5Temp)
		{
			//���ϴ��ļ���֮ͬ�⣬�������ȡ lastUpdateDone �����״̬������ȷ���Ƿ������ɸ���
			//��Ϊ�п��� ���һ�θ����� 1�� �������¸����ж�
			wstring strTemp = updateHelper.GetEtcDir() + L"lastUpdateDone" ;
			wstring buffer;
			if(!FileOperator::ReadAllText(strTemp, buffer))
				bNeedToUpdate = true;   //�Ҳ���lastUpdateDone �ļ�����Ϊ��Ҫ����
			else
			{
				if(buffer[0] == L'0')
					bNeedToUpdate = true;
				else
					bNeedToUpdate = false;	 //ֻ�� ���ϴ��ļ���ͬ���� lastUpdateDone �еı�׼ ��Ϊ0 ʱ������Ϊ���ø���
			}
		}
		else
		{
			bNeedToUpdate = true;
		}

		FileOperator::WriteAllText(strTemp, bNeedToUpdate ? L"1" : L"0");  //д��һ����Ҫ����updateItem���
	}

	//�������ص��ļ����Ƶ�Ŀ��λ��
	CopyFileW(strTempUpdate.c_str(), strUpdate.c_str(), FALSE);
	return true;
}


//���ffmpeg.exe �Ƿ���ڣ�������������
bool CCheckIntegrityThread::CheckFFmpeg()
{
	//���ffpmeg�Ƿ����
	wstring strDir = FileHelper::GetCurrentDirectoryStr() + TEMP_WAV_FLODER_NAME ;
	wstring strFfmpeg = strDir + L"\\ffmpeg.exe";
	
	UpdateProgressUI(0, wstring(L"��� "+strFfmpeg + L"�Ƿ����...").c_str());

	bool bFileExist =  FileHelper::CheckFileExist(strFfmpeg);
	string strMd5;
	bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
	if(!bFileExist || !bRet 
		|| (bFileExist && !updateHelper.IsValidFFmpegMd5(strMd5))) //���
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
		
		vector<LinkValue> links = LinkHelper::getSingleton().GetAllLinksFFmpeg();

		bool bTrySuceed = false;
		for(size_t i = 0; i < links.size(); ++i)
		{
			UpdateProgressUI(60 + i, wstring( L"����ת���� ffmpeg(42.41MB),�����ĵȴ� ... (try "+links[i].value +L")").c_str());
			if(!CDownloader::DownloadFile( links[i].link, strFfmpeg, &m_hCheckWnd))
				continue;
			
			bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
			if(!bRet || !updateHelper.IsValidFFmpegMd5(strMd5))
				continue;
			else
			{
				bTrySuceed = true;
				break;
			}
		}

		if(!bTrySuceed)//���ز��ɹ�
		{
			UpdateProgressUI(65, wstring( L"����ת���� ffmpeg(42.41 MB)�������ĵȴ� ...(try final)").c_str());
			bool bHaveDownload = true;

			//�������ط���
			//��ʼ�ӷ���������ffmpeg
			wstring strNameExt = L"ffmpeg.ext"+ SERVER_FILE_EXTENTION_W;
			UpdateProgressUI(10,wstring( L"�������� "+ strNameExt +L" ...").c_str());

			wstring strFileExt = strDir +L"\\"+ strNameExt;
			wstring strLinkExt = LINK_SERVER_PATH_2 + strNameExt;
			int nCountSplit = 11;

			//���� ffmpeg.ext.zip
			if( CDownloader::DownloadFile(strLinkExt, strFileExt, &m_hCheckWnd))
			{
				//�ļ��������ƣ�{"success":false,"message":"���ļ��ѳ���������������(200MB)����������"}
				//���� ffmpeg.1.zip - ffmpeg.11.zip 11���ļ�
				WCHAR szBuffer[MAX_BUFFER_SIZE/2];
				WCHAR szBuffer2[MAX_BUFFER_SIZE/2];
				for(auto i=1; i<= nCountSplit ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					UpdateProgressUI(10 + i*(90-10)/nCountSplit, wstring(L"�������� "+strNameSplited 
						+ L"... ("+_itow(i,szBuffer, 10)+ L"/" +_itow(nCountSplit,szBuffer2, 10) + L", �� 42.41 MB)").c_str());

					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					wstring strLinkSplited = LINK_SERVER_PATH_2 + strNameSplited;

					bRet = CDownloader::DownloadFile(strLinkSplited, strFileSplited, &m_hCheckWnd);
					if(false == bRet)
					{
						bHaveDownload = false;
						break;
					}
				}
			}
			else
				bHaveDownload = false;

			if(!bHaveDownload)
			{
				UpdateProgressUI(100, L"����ʧ�ܣ��ɳ����ڡ����á�ҳ�����µ���������Լ�⡱");

				//��ʾ�û�ȷʵ ת���ļ�������ʾ��δ���
				wstring strTip = L"�޷��ɹ������ļ���\\n";
				strTip+= strFfmpeg + L"\\n\\n";
		
				strTip+= wstring(L"����ԭ��\\n");
				strTip+= wstring(L"1�������޷��������磬�޷�����\\n");
				strTip+= wstring(L"2����ȡ�ļ��ķ�����쳣��github���������ȶ��Ƚ�����������ೢ�Լ������淽��1��\\n\\n");

				strTip+= wstring(L"���������\\n");
				strTip+= wstring(L"1���ȳ��ԣ���֤������������������,Ȼ�������ڡ����á�ҳ����С������Լ�⡱\\n");
				strTip+= wstring(L"2����������޷�����������Լ�����ffmpeg.exe �ļ��������ȷ�����󣬵���������������ԡ��µġ��ֶ�����ffmpeg.exe������\\n");
			
				strTip+= wstring(L"\\n");
				strTip+= wstring(L"��ܰ��ʾ:���򲥷� mp3 ʱ���õ� ffmpeg.exe");

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
					//����Ϊֹ���û��Ѿ��ȴ��˶�����ص��ˣ��Ѿ�û�н����

					UpdateProgressUI(100, L"����ʧ�ܣ��ɳ����ڡ����á�ҳ�����µ���������Լ�⡱");

					wstring strTip = L"�޷����������ļ���\\n";
					strTip += strFfmpeg + L"\\n\n";			
			
					strTip+= wstring(L"����ԭ��\\n");
					strTip+= wstring(L"1�������޷��������磬�޷�����\\n");
					strTip+= wstring(L"2����ȡ�ļ��ķ�����쳣��github���������ȶ��Ƚ�����������ೢ�Լ������淽��1��\\n\\n");

					strTip+= wstring(L"���������\\n");
					strTip+= wstring(L"1���ȳ��ԣ���֤������������������,Ȼ�������ڡ����á�ҳ����С������Լ�⡱\\n");
					strTip+= wstring(L"2����������޷�����������Լ�����ffmpeg.exe �ļ��������ȷ�����󣬵���������������ԡ��µġ��ֶ�����ffmpeg.exe������\\n");
					strTip+= wstring(L"\\n");
				    strTip+= wstring(L"��ܰ��ʾ:���򲥷� mp3 ʱ���õ� ffmpeg.exe");
					_MessageBox(NULL,strTip.c_str(), L"��ʾ", MB_OK|MB_ICONWARNING);
					bFail = true;
				}

				//ɾ����ʱ���ص��ļ�
				if(!bFail)
					UpdateProgressUI(95, L"ɾ����ʱ�ļ�...");

				_wremove(strFileExt.c_str());
				wchar_t szBuffer[255];
				for(auto i=1; i<= nCountSplit ;i++)
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