#include "stdafx.h"
#include "PageResult.h"
#include "utility/WinDialog.h"
#include "utility/WinFile.h"
#include "entity\CheckIntegrityThread.h"

CPageResult::CPageResult()
{
	wcscpy_s(PATH_STATE_2[0].nameOfPath, L"�����ļ�");
	wcscpy_s(PATH_STATE_2[1].nameOfPath, L"����ļ�");
	PATH_STATE_2[0].isInited = false;
	PATH_STATE_2[1].isInited = false;

	P2_Line1 = NULL;
	P2_Line2 = NULL;
	P2_Line3 = NULL;
	m_wndHighlight = NULL;
	
	emptyTip2= NULL;
	
	m_EditMusic   = NULL;
	m_EditLyric   = NULL;
	
	m_txtMusic = NULL;
	m_txtLyric = NULL;

	m_txtTime = NULL;
	m_ProgressTime = NULL;
	
	m_btnStart = NULL;
	m_btnAdjust =NULL;
	m_btnEndPreview = NULL;
	m_btnLoad = NULL;

	m_bSingleCycle = false;	//����ѭ��
}

//��ʼ������ҳ��
void CPageResult::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ô����г��õĿؼ���ָ��
	P2_Line1 = M()->FindChildByID2<SStatic>(R.id.name_2_line_1);
	SASSERT(P2_Line1 != NULL);
	P2_Line2 = M()->FindChildByID2<SStatic>(65001);
	SASSERT(P2_Line2 != NULL);
	P2_Line3 = M()->FindChildByID2<SStatic>(R.id.name_2_line_3);
	SASSERT(P2_Line3 != NULL);
	m_wndHighlight = m_pMainWnd->FindChildByID(R.id.highlight_bkgnd_2);
	SASSERT(m_wndHighlight != NULL);

	emptyTip2 = M()->FindChildByID2<SStatic>(R.id.empty_tip_block_2);
	SASSERT(emptyTip2 != NULL);


	m_EditMusic = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_music_2);
	SASSERT(m_EditMusic != NULL);
	m_EditLyric = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lyric_2);
	SASSERT(m_EditLyric != NULL);
	
	m_txtMusic = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_music_2);
	SASSERT(m_txtMusic != NULL);
	m_txtLyric = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_lyric_2);
	SASSERT(m_txtLyric != NULL);
	
	m_txtTime = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_time_2);
	SASSERT(m_txtTime != NULL);
	m_ProgressTime = m_pMainWnd->FindChildByID2<SProgress>(R.id.progress_music_2);
	SASSERT(m_ProgressTime != NULL);
	
	m_btnStart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_start_playing);
	SASSERT(m_btnStart != NULL);
	m_btnAdjust = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_manual_adjust);
	SASSERT(m_btnAdjust != NULL);
	m_btnEndPreview = M()->FindChildByID2<SButton>(R.id.btn_end_preview);
	SASSERT(m_btnEndPreview != NULL);
	m_btnLoad = M()->FindChildByID2<SButton>(R.id.btn_load_2);
	SASSERT(m_btnLoad != NULL);
	
	m_checkDesktopLyric = M()->FindChildByID2<SCheckBox>(R.id.check_desktop_lyric);
	SASSERT(m_checkDesktopLyric != NULL);
	m_checkSingleCycle = M()->FindChildByID2<SCheckBox>(R.id.check_single_cycle);
	SASSERT(m_checkSingleCycle != NULL);
}

//��������ڶ���
CMainDlg* CPageResult::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	����Ԥ��ҳ�����Ӧ����
*/

//����·����ѡ��
void CPageResult::OnBtnSelectMusic2(LPCWSTR pFilePath)
{	
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"��Ƶ�ļ�(mp3,mp2,wma,wav,wv,ape,flac)\0*.mp3;*.mp2;*.wma;*.wav;*.wv;*.ape;*.flac;\0�ֻ�¼��(amr)/�ֻ�����(mmf)\0*amr;*.mmf\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_music_path)? M()->m_settingPage.m_default_music_path.c_str():nullptr
			);

		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if( CFileDialogEx::checkPathName(_T("*.mp3"),pPath) 
		 || CFileDialogEx::checkPathName(_T("*.mp2"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wma"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wav"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wv"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.ape"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.flac"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.amr"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.mmf"),pPath))
		{	
			; //ʹ�á��������ж�( || )�� ���á��ҡ������жϣ�&&�����Լ��� checkPathName ���õĴ���
		}
		else 
		{
			_MessageBox(M()->m_hWnd,_T("��ʽ��֧��\\n��ȷ���ļ���ʽΪ��ѡ��Ի���ָ�����ļ����͡�"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//��ʾ��ѡ����ļ�
		m_EditMusic->SetWindowTextW(pPath);

		PATH_STATE_2[0].isInited = true;

		//��ʲ�����
		M()->player.setMusicPath(pPath, M()->m_hWnd);
	}
    
}

void CPageResult::OnBtnSelectLyric2(LPCWSTR pFilePath)
{	
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"LRC�ı��ļ�(*.lrc)\0*.lrc\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_output_path)? M()->m_settingPage.m_default_output_path.c_str():nullptr
			);
		
		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T("*.lrc"),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("��ʽ��֧��\\n��ȷ���ļ���ʽΪ��*.lrc��"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		//��ʾ��ѡ����ļ�
		m_EditLyric->SetWindowTextW(pPath);
		
		PATH_STATE_2[1].isInited = true;
		
		//�����ʲ�����
		M()->player.setLyricPath(pPath);
	}
}

void CPageResult::OnBtnLoad2()
{
	//ȷ��������·�������ݶ��Ѿ���ʼ��
	if(!isPathReady_2())
	{
		//CMainDlg::getPathNotReady_2() ��ʱ���ص����� δ��ʼ����·�� �� PATH_STATE �ж�Ӧ���±�
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("����ûѡ��\\n��%s��"),PATH_STATE_2[getPathNotReady_2()].nameOfPath),
			_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	//ȷ�� ����ļ� �� �����ļ� ·����Ч
	File lyricFile(M()->player.m_szLyricPathName,_T("r"));

	if(!lyricFile.isValidFile()) 
	{
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("�ļ���ʧ��:\\n��%s��\\n��ȷ���ļ���Ч!"),M()->player.m_szLyricPathName),
			_T("ʧ����ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}

	//���� LyricPlayer�� �������Ϊ��
	M()->player.reloadPlayer();

	//���ļ���ȡ��ʱ����Ϣ��ÿ�и�ʵļ�������
	M()->player.m_vLineInfo = M()->player.getLyricWithLineInfo(lyricFile);

	//û�и�ʣ����ܲ���
	if(M()->player.m_vLineInfo.empty())
	{
		//��� �����ܴ��ڵģ�ҳ��� ��ǰ���� �� ��ǰ��ʵ���Ϣ
		m_txtMusic->SetWindowTextW(_T(""));
		m_txtLyric->SetWindowTextW(_T(""));

		//��� �����ܴ��ڵģ������и��
		P2_Line3->SetWindowTextW(_T(""));

		//���� ���������õģ���ʼ��ť
		m_btnStart->EnableWindow(FALSE,TRUE); //��һ������bEnableΪ �Ƿ����ã��ڶ�������bUpdateΪ�Ƿ��ڸı�״̬�������ʾ

		//���� ���������õģ��ֶ�΢��ʱ����
		m_btnAdjust->EnableWindow(FALSE,TRUE); 

		_MessageBox(M()->m_hWnd,SStringT().Format(_T("��ǰ����ļ�û�пɲ������ݣ�\\n�ļ���\\n��%s��"),M()->player.m_szLyricPathName),
			_T("ʧ����ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}

	//���� ҳ��� ��ǰ���� �� ��ǰ��ʵ���Ϣ
	m_txtMusic->SetWindowTextW(M()->player.m_szMusicPathName);
	m_txtLyric->SetWindowTextW(M()->player.m_szLyricPathName);

	//���»�ҳ�����ʾ
	//��ȡ��ϣ������ʾ��壬��ʾ��һ���ǿո��
	P2_Line1->SetWindowTextW(_T(""));
	P2_Line2->SetWindowTextW(_T(""));
	P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[0].m_strLine);

	//�ı䰴ť��״̬
	m_btnStart->EnableWindow(TRUE,TRUE); //��һ������bEnableΪ �Ƿ����ã��ڶ�������bUpdateΪ�Ƿ��ڸı�״̬�������ʾ

	//������ ���ֶ�΢��ʱ���ᡱ��ť
	m_btnAdjust->EnableWindow(TRUE,TRUE); 

}

//�ڶ���ҳ��(����Ԥ��)���ص������ذ�ť�����º��״̬
void CPageResult::backToInit_2()
{
	//�������ź͸�ʹ���
	M()->player.playingEnd(m_pMainWnd);

	//������ʾ��壬��ʾ��һ���ǿո��
	P2_Line1->SetWindowTextW(_T(""));
	P2_Line2->SetWindowTextW(_T(""));
	P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[0].m_strLine);

	//���ؿ�����ʾ
	emptyTip2->SetVisible(FALSE,TRUE);

	//�ı䰴ť��״̬
	m_btnLoad->EnableWindow(TRUE,TRUE);
	m_btnStart->EnableWindow(TRUE,TRUE);
	m_btnEndPreview->EnableWindow(FALSE,TRUE);
	
	//ȡ�������ʾ����еġ���ǰ�С��ĸ�������
	m_wndHighlight->SetVisible(FALSE,TRUE);

	//����ʱ���ǩ�ͽ�����
	m_txtTime->SetWindowTextW(_T("00:00.000"));
	m_ProgressTime->SetValue( 0 );//����ǧ����ֵ

	//����������Ԥ����״̬
	M()->m_bIsLyricPlaying = FALSE;
}

//����Ԥ�����ţ�����״̬
void CPageResult::OnBtnEndPreview()
{	
	//����״̬
	backToInit_2();
}

//��ʼ���Ÿ��
void CPageResult::OnBtnStartPlaying()
{
	//������ڣ�������� ��״̬����������Ԥ�����
	if(M()->m_bIsLyricMaking == TRUE)
	{
		_MessageBox(M()->m_hWnd,_T("���Ƚ����������������"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	//��ʼ���ź͸�ʹ���
	bool bSuccess = M()->player.playingStart(m_pMainWnd);
	if(!bSuccess)
	{
		//���Ų��ɹ�����Ҫת���ļ�
		
		//��ʧ�ܣ��п�����MCI��ʼ������
		//�����߳̽���ת��
		::CreateThread(NULL, 0, ThreadConvertProc, this, 0 ,NULL);

		return;
	}

	//�ı䰴ť��״̬
	m_btnLoad->EnableWindow(FALSE,TRUE);
	m_btnStart->EnableWindow(FALSE,TRUE);
	m_btnEndPreview->EnableWindow(TRUE,TRUE);
	
	//��ʾ�����ʾ����еġ���ǰ�С��ĸ�������
	SWindow *highLight = M()->FindChildByID(R.id.highlight_bkgnd_2);
	m_wndHighlight->SetVisible(TRUE,TRUE);

	//���롰����Ԥ����״̬
	M()->m_bIsLyricPlaying = TRUE;

	//��¼ҳ�沥���㼣���꿴������˵��
	M()->FootPrintPage = 1;
}

void CPageResult::OnBtnManualAdjust()
{	
	if(_tcslen(M()->player.m_szLyricPathName)!=0)
	{
		//��windowĬ�ϱ༭���򿪸���ļ�
		ShellExecute(NULL,L"open",L"notepad", M()->player.m_szLyricPathName,NULL,SW_SHOWNORMAL);
	}
}



//�жϵڶ���ҳ��(����Ԥ��) �� ����·���Ƿ�ѡ����� */
bool CPageResult::isPathReady_2()				
{
	bool ret = true;
	for(int i=0; i< 2; i++)
		if(!PATH_STATE_2[i].isInited)
		{
			ret = PATH_STATE_2[i].isInited;
			break;
		}
	return ret;
}

/* ��õ�ǰδ��ʼ���ĵ�һ��·�� ��PATH_STATE_2 �ж�Ӧ���±�;����ʼ�����򷵻�-1 */
int CPageResult::getPathNotReady_2()			
{
	int index = -1;
	for(int i=0; i< sizeof(PATH_STATE_2)/sizeof(PATH_STATE_2[0]); i++)
		if(!PATH_STATE_2[i].isInited)
		{
			index = i;
			break;
		}
	return index;
}


//�ڹ���Ԥ����ҳ�棬ִ�и����ʾ m_nCurLine
void CPageResult::scrollToLyricCurLine()
{
	//player.m_nCurLine �ķ�Χ��1 ~ player.m_nTotalLine

	//����ÿ��������ĵڶ��б��������ó�����ִ�У��������ڻ��Ҳ��˽����������ˢ�»��� һ��ȫˢ�£�
	//Ϊ��֤��ʾ˳����ϵ���������ˢ�£�����ѡ����׸һ��ķֱ�д�ڸ���������
	if(M()->player.m_nCurLine == 1)
	{
		P2_Line1->SetWindowTextW(L"");

		//���µڶ��к͵�����
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);
		P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine+1 -1].m_strLine);
	}
	else if(M()->player.m_nCurLine < M()->player.m_nTotalLine)
	{
		//��������
		P2_Line1->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine-1 -1].m_strLine);
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);
		P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine+1 -1].m_strLine);
	}
	else // player.m_nCurLine == player.m_nTotalLine
	{
		//���µ�һ�к͵ڶ���
		P2_Line1->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine-1 -1].m_strLine);
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);

		//�����һ�����
		P2_Line3->SetWindowTextW(L"");
	}

	//���ݸ���Ƿ�Ϊ�գ���ʾ�����ؿ�����ʾ
	emptyTip2->SetVisible(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_bIsEmptyLine,TRUE);
}

//������
void CPageResult::OnCheckDesktopLyricChanged()
{
	BOOL bDesktopChecked = m_checkDesktopLyric->IsChecked();
	//if(bDesktopChecked)
	//	M()->m_wndDesktopLyric->ShowDesktopLyric();
	//else
	//	M()->m_wndDesktopLyric->HideDesktopLyric();
	if(bDesktopChecked)
		M()->m_wndDesktopLyric->ShowWindow(SW_SHOW);
	else
		M()->m_wndDesktopLyric->ShowWindow(SW_HIDE);
}

//����ѭ��
void CPageResult::OnCheckSingleCycleChanged()
{
	BOOL bSingleCycleChecked = m_checkSingleCycle->IsChecked();
	if(bSingleCycleChecked)
		 m_bSingleCycle = true;
	else
		 m_bSingleCycle = false;
}

void CPageResult::OnBtnSoundOpen2()
{
	M()->OnBtnSoundOpen();
}

void CPageResult::OnBtnSoundClose2()
{
	M()->OnBtnSoundClose();
}

void  CPageResult::OnSliderPos2(EventArgs *pEvt)
{
	M()->OnSliderPos(false);
}


//�߳�ִ�е�ַ(��ʽת���߳�)
DWORD WINAPI CPageResult::ThreadConvertProc(LPVOID pParam)
{
	CPageResult* pPageResult  = (CPageResult*)pParam;

	WCHAR szMusicPathName[_MAX_PATH];
	pPageResult->M()->player.m_musicPlayer.GetMusicPathName(szMusicPathName, _MAX_PATH);

	wstring strMusicPath(szMusicPathName);
	wstring strDir = FileHelper::GetCurrentDirectoryStr();
	wstring strName;
	wstring strExt;
	FileHelper::SplitPath(strMusicPath, NULL,NULL,&strName,&strExt);

	if(strExt == L".wav")
	{
		_MessageBox(NULL,   (L"�޷������ļ���"+strName + L"\\n�볢��ʹ�ñ�������ļ�����ת����ʽ").c_str(), L"��ʾ", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	wstring strTargetDir = strDir + TEMP_WAV_FLODER_NAME ; //�õ�Ŀ���ļ���
	wstring strTargetFilePath = strDir + TEMP_WAV_FLODER_NAME +L"\\"+ strName + L".wav"; //�õ�Ŀ���ļ���
	wstring strFfmpegPath = strDir + TEMP_WAV_FLODER_NAME +L"\\ffmpeg.exe"; //�õ�Ŀ���ļ���

	if(!FileHelper::CheckFolderExist(strTargetDir))
	{
		_MessageBox(NULL, (L"�ļ��в����ڣ�"+strTargetDir + L"\\n��ͼת��ʽʧ�ܣ�������������������").c_str(), L"��ʾ", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	bool bFileExist =  FileHelper::CheckFileExist(strFfmpegPath);
	string strMd5;
	bool bRet = UpdateHelper::GetFileMd5(strFfmpegPath,strMd5);
	if(!bFileExist || !bRet || (bFileExist && strMd5 != "949ed6af96c53ba9e1477ded35281db5")) //���
	{
		_MessageBox(NULL, (L"�ļ������ڻ�������\\n"+strFfmpegPath + L"\\n\\n��ͼת��ʽʧ��\\n�볢�ԣ�\
			\\n1���ڡ����á�ҳ��ġ��������⡿�£������ť�������Լ�⡿"
			).c_str(), L"��ʾ", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	//��ʼת��
	WCHAR command[_MAX_PATH * 3];
	_swprintf(command, L"-y -i \"%s\" \"%s\"",szMusicPathName, strTargetFilePath.c_str());

	//ShellExecute(NULL,L"open",strFfmpegPath.c_str(), command ,NULL,SW_SHOWNORMAL);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strFfmpegPath.c_str();	
	ShExecInfo.lpParameters = command;	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;	

	if(!FileHelper::CheckFileExist(strTargetFilePath))//�����ڣ���ִ��ת�����Ѵ�����ת��
	{
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);  //�ȴ�ֱ��ִ����ϣ��õ�Ŀ���ļ�
	}

	if(!FileHelper::CheckFileExist(strTargetFilePath))
	{
		_MessageBox(NULL, (L"�ļ�ת��ʽʧ�ܣ�"+ wstring(szMusicPathName)).c_str(), L"��ʾ", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	pPageResult->M()->player.setMusicPath(strTargetFilePath.c_str(),pPageResult->M()->m_hWnd);

	//ע������ο� PageMaking ��ThreadConvertProc �ص�����

	::PostMessage(pPageResult->M()->m_hWnd,MSG_USER_PLAYING_START_BUTTON,0,0);

	return true;
}


