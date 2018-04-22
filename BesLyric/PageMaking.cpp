#include "stdafx.h"
#include "PageMaking.h"
#include "utility/WinDialog.h"
#include "utility/WinFile.h"
#include "entity\GuessLyricInfoThread.h"
#include "entity\NcmIDManager.h"
#include "DlgDownloadNcmMp3.h"

CPageMaking::CPageMaking()
{
	wcscpy_s(PATH_STATE_1[0].nameOfPath, L"�����ļ�");
	wcscpy_s(PATH_STATE_1[1].nameOfPath, L"����ļ�");
	wcscpy_s(PATH_STATE_1[2].nameOfPath, L"���·��");
	PATH_STATE_1[0].isInited = false;
	PATH_STATE_1[1].isInited = false;
	PATH_STATE_1[2].isInited = false;
	
	P1_Line1 = NULL;
	P1_Line2 = NULL;
	P1_Line3 = NULL;
	m_wndHighlight = NULL;
	
	emptyTip1= NULL;

	m_EditMusic   = NULL;
	m_EditLyric   = NULL;
	m_EditOutPath = NULL;

	m_txtMusic = NULL;
	m_txtLyric = NULL;
	
	m_txtTime = NULL;
	m_ProgressTime = NULL;

	m_btnStart = NULL;
	m_btnPreview = NULL;
	m_btnOpenOutput = NULL;
	m_btnLoad = NULL;
	m_btnRestart= NULL;

	m_btnMatchNcmID = NULL;
}

//��ʼ������ҳ��
void CPageMaking::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ô����г��õĿؼ���ָ��
	P1_Line1 = M()->FindChildByID2<SStatic>(R.id.name_1_line_1);
	SASSERT(P1_Line1 != NULL);
	P1_Line2 = M()->FindChildByID2<SStatic>(65000);
	SASSERT(P1_Line2 != NULL);
	P1_Line3 = M()->FindChildByID2<SStatic>(R.id.name_1_line_3);
	SASSERT(P1_Line3 != NULL);
	m_wndHighlight = m_pMainWnd->FindChildByID(R.id.highlight_bkgnd);
	SASSERT(m_wndHighlight != NULL);

	emptyTip1 = M()->FindChildByID2<SStatic>(R.id.empty_tip_block);
	SASSERT(emptyTip1 != NULL);

	
	m_EditMusic = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_music_1);
	SASSERT(m_EditMusic != NULL);
	m_EditLyric = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lyric_1);
	SASSERT(m_EditLyric != NULL);
	m_EditOutPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_output);
	SASSERT(m_EditOutPath != NULL);

	m_txtMusic = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_music_1);
	SASSERT(m_txtMusic != NULL);
	m_txtLyric = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_lyric_1);
	SASSERT(m_txtLyric != NULL);

	m_txtTime = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_time_1);
	SASSERT(m_txtTime != NULL);
	m_ProgressTime = m_pMainWnd->FindChildByID2<SProgress>(R.id.progress_music_1);
	SASSERT(m_ProgressTime != NULL);

	m_btnStart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_start_making);
	SASSERT(m_btnStart != NULL);
	m_btnPreview = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_preview);
	SASSERT(m_btnPreview != NULL);
	m_btnOpenOutput = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_open_output);
	SASSERT(m_btnOpenOutput != NULL);
	m_btnLoad = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_load_1);
	SASSERT(m_btnLoad != NULL);
	m_btnRestart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_restart);
	SASSERT(m_btnRestart != NULL);

	m_btnMatchNcmID = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_match_ncm_id);
	SASSERT(m_btnMatchNcmID != NULL);
}

//��������ڶ���
CMainDlg* CPageMaking::M()
{
	return (CMainDlg*)m_pMainWnd;
}

/*
*	�������ҳ�����Ӧ����
*/

//����·����ѡ��
void CPageMaking::OnBtnSelectMusic1(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"��Ƶ�ļ�(mp3,mp2,wma,wav,wv,ape,flac),����������ncm�ļ�\0*.mp3;*.mp2;*.wma;*.wav;*.wv;*.ape;*.flac;*.ncm\0�ֻ�¼��(amr)/�ֻ�����(mmf)\0*amr;*.mmf\0\0",
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
		 || CFileDialogEx::checkPathName(_T("*.mmf"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.ncm"),pPath))
		{	
			; //ʹ�á��������ж�( || )�� ���á��ҡ������жϣ�&&�����Լ��� checkPathName ���õĴ���

			if(CFileDialogEx::checkPathName(_T("*.ncm"),pPath))
				m_btnMatchNcmID->SetVisible(TRUE,TRUE);
			else
				m_btnMatchNcmID->SetVisible(FALSE,TRUE);
		}
		else 
		{
			_MessageBox(M()->m_hWnd,_T("��ʽ��֧��\\n��ȷ���ļ���ʽΪ��ѡ��Ի���ָ�����ļ����͡�"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//��ʾ��ѡ����ļ�
		m_EditMusic->SetWindowTextW(pPath);

		PATH_STATE_1[0].isInited = true;

		//������������
		M()->maker.setMusicPath(pPath,M()->m_hWnd);
	}
}

void CPageMaking::OnBtnSelectLyric1(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		if(bRet == TRUE)
			pPath = pPath;
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"�ı��ļ�(*.txt)\0*.txt\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_lyric_path)? M()->m_settingPage.m_default_lyric_path.c_str():nullptr
			);
		
		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T("*.txt"),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("��ʽ��֧��\\n��ȷ���ļ���ʽΪ��*.txt��"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		//��ʾ��ѡ����ļ�
		m_EditLyric->SetWindowTextW(pPath);
		
		PATH_STATE_1[1].isInited = true;
		
		//������������
		M()->maker.setLyricPath(pPath);
	}
}

void CPageMaking::OnBtnSelectOutput(LPCWSTR pDirPath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pDirPath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��LRC����ļ��� Ĭ�����·��"), TRUE,M()->m_settingPage.m_default_output_path.c_str());
			
		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pDirPath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T(".."),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("��ʽ��֧��\\n��ȷ����ѡ���ļ��С�"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//��ʾ��ѡ����ļ�
		m_EditOutPath->SetWindowTextW(pPath);
		
		PATH_STATE_1[2].isInited = true;
		
		//������������
		M()->maker.setm_szOutputPath(pPath);
	}
    
}

//������ʣ����� (��ʼ�� LyricMaker)
void CPageMaking::OnBtnLoad1()
{
	//ȷ������·�������ݶ��Ѿ���ʼ��
	if(!isPathReady_1())
	{
		//CMainDlg::getPathNotReady_1() ��ʱ���ص����� δ��ʼ����·�� �� PATH_STATE �ж�Ӧ���±�
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("����ûѡ��%s��"),PATH_STATE_1[getPathNotReady_1()].nameOfPath),
			_T("��ʾ"),MB_OK|MB_ICONASTERISK);
		return;
	}
	
	//��ʾ������·��
	wstring FilePathToShow = M()->maker.m_szMusicPathName;

	//����ǰ����ncm �ļ������
	bool bIsCurrentNcm = false;
	wstring strID = L"";
	wstring strName =L"";
	wstring strMp3FullPath = L"";
	int nRet = DealWithNcmFile(bIsCurrentNcm, strID, strName, strMp3FullPath);
	if(nRet > 0) //Ϊncm �ļ������
	{
		if(nRet == 1) //δƥ��ncm ��ID
		{
			_MessageBox(M()->m_hWnd,SStringT().Format(_T("��û��Ϊ�ļ�����%s��ƥ������������ID\\n���ȵ����ƥ��ID����ťƥ��"),strName.c_str()),
			_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		else if(nRet == 2) //������������mp3ʧ��
		{
			_MessageBox(M()->m_hWnd,SStringT().Format(_T("��������ʧ�ܣ��޷�Ϊ������ncm�ļ���Ӧ��mp3"),M()->maker.m_szLyricPathName),
			_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		else //==3
		{
			FilePathToShow = M()->maker.m_szMusicPathName + wstring(L" ( ID: ")+ strID + L" )";
			M()->maker.setMusicPath(strMp3FullPath.c_str(), M()->m_hWnd);
		}
	}
	else if(nRet < 0)
	{
		//�����ļ���ʧ��
		return;
	}

	//ȷ�� ����ļ� �� �����ļ���Ч
	File lyricFile(M()->maker.m_szLyricPathName,_T("r"));

	if(!lyricFile.isValidFile()) 
	{
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("�ļ���ʧ��:\\n��%s��\\n!��ȷ���ļ���Ч"),M()->maker.m_szLyricPathName),
			_T("ʧ����ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}

	//���� ҳ��� ��ǰ���� �� ��ǰ��ʵ���Ϣ
	m_txtMusic->SetWindowTextW(FilePathToShow.c_str());
	m_txtLyric->SetWindowTextW(M()->maker.m_szLyricPathName);
	
	//���� LyricMaker�� �������Ϊ��
	M()->maker.reloadMaker();

	//���ļ���ȡÿһ�еĸ��
	M()->maker.m_vLyricOrigin = M()->maker.getLyricOrigin(lyricFile);
	M()->maker.m_vLyricOriginWithEmptyLine = M()->maker.m_vLyricOrigin; //��ʼ�� m_vLyricOriginWithEmptyLine
	 
	//��ȡ��ϣ������ʾ��壬��ʾ��һ���ǿո��
	P1_Line1->SetWindowTextW(_T(""));
	P1_Line2->SetWindowTextW(_T(""));
	P1_Line3->SetWindowTextW(M()->maker.m_vLyricOriginWithEmptyLine[0]);

	//�ı䰴ť��״̬
	m_btnStart->EnableWindow(TRUE,TRUE); //��һ������bEnableΪ �Ƿ����ã��ڶ�������bUpdateΪ�Ƿ��ڸı�״̬�������ʾ
	//m_btnStart->Invalidate();			//���û��ָ���ڶ�������bUpdate����Ĭ��ֵΪ FALSE��Ҫˢ������Ҫ�Լ�Invalidate()
	
	//�ı�Ԥ����ť��״̬
	m_btnPreview->EnableWindow(FALSE,TRUE);
	m_btnOpenOutput->EnableWindow(FALSE,TRUE);
}

//����Ԥ���ո�������ɵĴ�ʱ��ʱ������
void CPageMaking::OnBtnPreview()
{
	//�л���Ԥ��ҳ��
	STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
	if(tab)
		tab->SetCurSel(1);

	Sleep(500);

	//����player��״̬
	M()->player.setMusicPath(M()->maker.m_szMusicPathName,M()->m_hWnd);
	M()->player.setLyricPath(M()->maker.m_szOutputPathName);
	
	M()->m_pageResult->PATH_STATE_2[0].isInited = true;
	M()->m_pageResult->PATH_STATE_2[1].isInited = true;

	//����ҳ�� ·�������͵�ǰѡ������ֺ͸�ʣ�������׼��������֮���״̬��
	//��ʾ��ѡ����ļ�
	M()->m_pageResult->m_EditMusic->SetWindowTextW(M()->player.m_szMusicPathName);
	//��ʾ��ѡ����ļ�
	M()->m_pageResult->m_EditLyric->SetWindowTextW(M()->player.m_szLyricPathName);


	M()->m_pageResult->OnBtnLoad2();
}

//�����ɵ��ļ�
void CPageMaking::OnBtnOpenOutput()
{
	if(_tcslen(M()->maker.m_szOutputPath)!=0)
	{
		//���ļ���
		ShellExecute(NULL,L"explore", M()->maker.m_szOutputPath,NULL, NULL,SW_SHOWNORMAL);
			
		//��windowĬ�ϱ༭���򿪸���ļ�
		ShellExecute(NULL,L"open",L"notepad", M()->maker.m_szOutputPathName,NULL,SW_SHOWNORMAL);
	}
}

//��ͼ�������
void CPageMaking::OnBtnTrySearchLyric()
{
	if(_tcslen(M()->maker.m_szMusicPathName)!=0)
	{	
		CGuessLyricInfoThread::getSingleton().Start( M()->m_hWnd, SStringW(M()->maker.m_szMusicPathName));
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"����ѡ�������ļ� :)", L"��ʾ", MB_OK|MB_ICONINFORMATION);
	}
}

//�򿪱༭ѡ�еĸ���ļ�
void CPageMaking::OnBtnEditLyric()
{
	if(_tcslen(M()->maker.m_szLyricPathName)!=0)
	{	
		//��windowĬ�ϱ༭���򿪸���ļ�
		ShellExecute(NULL,L"open",L"notepad", M()->maker.m_szLyricPathName,NULL,SW_SHOWNORMAL);
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"����ѡ�����ļ� :)", L"��ʾ", MB_OK|MB_ICONINFORMATION);
	}
}

//����ƥ��IDҳ�棬�Զ������������ID�������ƥ���ID����ʼ����ҳ����ʾ
void CPageMaking::OnBtnMatchID()
{
	M()->FindChildByID2<STabCtrl>(R.id.tab_content_container)->SetCurSel(1); //����ҳ��

	//ʹ���ļ�·�����ļ����������ļ����༭�ؼ�
	M()->m_pageSearchNcmID->SetSongFileNameEditWithPath(M()->maker.m_szMusicPathName);  

	//���ò²����������Ϣ�̣߳����²�����ID��Ҫ����Ϣ
	CGuessLyricInfoThread::getSingleton().Start( M()->m_hWnd, SStringW(M()->maker.m_szMusicPathName), true);
}


//��һ��ҳ��(�������)���ص������ذ�ť�����º��״̬
void CPageMaking::backToInit_1()
{
	M()->maker.m_vLyricOriginWithEmptyLine = M()->maker.m_vLyricOrigin;
	M()->maker.m_vLyricOutput.clear();
	M()->maker.stopMusic();

	//������ʾ��壬��ʾ��һ���ǿո��
	P1_Line1->SetWindowTextW(_T(""));
	P1_Line2->SetWindowTextW(_T(""));
	P1_Line3->SetWindowTextW(M()->maker.m_vLyricOrigin[0]); //��һ���������

	//���ؿ�����ʾ
	emptyTip1->SetVisible(FALSE,TRUE);
	
	//�ı䰴ť��״̬
	m_btnLoad->EnableWindow(TRUE,TRUE);
	m_btnStart->EnableWindow(TRUE,TRUE);
	m_btnRestart->EnableWindow(FALSE,TRUE);
	
	//ȡ�������ʾ����еġ���ǰ�С��ĸ�������
	m_wndHighlight->SetVisible(FALSE,TRUE);

	//����ʱ���ǩ�ͽ�����
	m_txtTime->SetWindowTextW(_T("00:00.000"));
	m_ProgressTime->SetValue( 0 );//����ǧ����ֵ

	//�ı�״̬  ����������Ϣ
	M()->m_bIsLyricMaking = FALSE;

	M()->KillTimer(101);
}

//��Ҫ����
void CPageMaking::OnBtnRestart()
{
	//����״̬
	backToInit_1();
}

//��ʼ����
void CPageMaking::OnBtnStartMaking()
{
	//������ڣ�����Ԥ����� ��״̬���������������
	if(M()->m_bIsLyricPlaying == TRUE)
	{
		_MessageBox(M()->m_hWnd,_T("���Ƚ�������ʹ���Ԥ������"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	bool bSuccess = M()->maker.makingStart();
	if(!bSuccess)
	{
		//���Ų��ɹ�����Ҫת���ļ�
		
		//��ʧ�ܣ��п�����MCI��ʼ������
		//�����߳̽���ת��
		::CreateThread(NULL, 0, ThreadConvertProc, this, 0 ,NULL);

		return;
	}
	
	//�ı���Ӧ������״̬
	M()->m_bIsLyricMaking = TRUE;
	
	//�ı䰴ť��״̬
	m_btnLoad->EnableWindow(FALSE,TRUE);
	m_btnStart->EnableWindow(FALSE,TRUE);
	m_btnRestart->EnableWindow(TRUE,TRUE);
	
	//�ı�Ԥ����ť��״̬
	m_btnPreview->EnableWindow(FALSE,TRUE);
	m_btnOpenOutput->EnableWindow(FALSE,TRUE);

	//��ʾ�����ʾ����еġ���ǰ�С��ĸ�������
	m_wndHighlight->SetVisible(TRUE,TRUE);

	M()->SetTimer(101,1);

	//��¼ҳ�沥���㼣���꿴������˵��
	M()->FootPrintPage = 0;
}

//����maker������ʵĵ�ǰ���������½�������
void CPageMaking::UpdataMakerLyricShowing()
{
	if(M()->maker.m_nCurLine==0) //�ʼ
	{
		P1_Line1->SetWindowTextW(_T(""));
		P1_Line2->SetWindowTextW(_T(""));
		P1_Line3->SetWindowTextW( M()->maker.m_vLyricOrigin[0]); //��һ���������
		emptyTip1->SetVisible(FALSE,TRUE);
	}
	else
	{
		//��һ�ε�һ��û������
		if(M()->maker.m_nCurLine == 1)
		{
			P1_Line1->SetWindowTextW( _T(""));
		}else
		{
			P1_Line1->SetWindowTextW( M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine -1));
		}

		P1_Line2->SetWindowTextW(  M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine));
	
		if(M()->maker.m_nCurLine != M()->maker.m_nTotalLine)
		{
			P1_Line3->SetWindowTextW(  M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine +1));
		}
		else//����к�������Ϊ��
		{
			P1_Line3->SetWindowTextW(_T(""));
		}

		//������һ���Ƿ�Ϊ���У���ʾ������ʾ
		if(M()->maker.isLastLineSpace())
			emptyTip1->SetVisible(TRUE,TRUE);
		else
			emptyTip1->SetVisible(FALSE,TRUE);
	}
}

void CPageMaking::OnBtnSoundOpen1()
{
	M()->OnBtnSoundOpen();
}

void CPageMaking::OnBtnSoundClose1()
{
	M()->OnBtnSoundClose();
}

//��������slider λ�õı仯
void  CPageMaking::OnSliderPos1(EventArgs *pEvt)
{
	M()->OnSliderPos(true);
}


//�жϵ�һ��ҳ��(�������) �� ����·���Ƿ�ѡ����� */
bool CPageMaking::isPathReady_1()				
{
	bool ret = true;
	for(int i=0; i< 3; i++)
		if(!PATH_STATE_1[i].isInited)
		{
			ret = PATH_STATE_1[i].isInited;
			break;
		}
	return ret;
}

/* ��õ�ǰδ��ʼ���ĵ�һ��·�� ��PATH_STATE_1 �ж�Ӧ���±�;����ʼ�����򷵻�-1 */
int CPageMaking::getPathNotReady_1()			
{
	int index = -1;
	for(int i=0; i< sizeof(PATH_STATE_1)/sizeof(PATH_STATE_1[0]); i++)
		if(!PATH_STATE_1[i].isInited)
		{
			index = i;
			break;
		}
	return index;
}


//�߳�ִ�е�ַ(��ʽת���߳�)
DWORD WINAPI CPageMaking::ThreadConvertProc(LPVOID pParam)
{
	CPageMaking* pPageMaking  = (CPageMaking*)pParam;

	WCHAR szMusicPathName[_MAX_PATH];
	pPageMaking->M()->maker.m_musicPlayer.GetMusicPathName(szMusicPathName, _MAX_PATH);

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
		_MessageBox(NULL, (L"�ļ��в����ڣ�"+strTargetDir + L"\\n\\n��ͼת��ʽʧ��\\n�볢�ԣ�\
			\\n1��ȷ�� �ļ��С�wav�����ڣ����롾BesLyric.exe����ͬһ��Ŀ¼��\
			\\n2������������������"
			).c_str(), L"��ʾ", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	if(!FileHelper::CheckFileExist(strFfmpegPath))
	{
		_MessageBox(NULL, (L"�ļ������ڣ�"+strFfmpegPath + L"\\n\\n��ͼת��ʽʧ��\\n�볢�ԣ�\
			\\n1���ڡ����á�ҳ��ġ��������⡿�£������ť�������Լ�⡿\
			\\n2������������������"
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

	//pPageMaking->M()->maker.m_musicPlayer.m_szMusicPathName;
	pPageMaking->M()->maker.setMusicPath(strTargetFilePath.c_str(),pPageMaking->M()->m_hWnd);

	//ע�⣺���ﲻ��ʹ������2�ַ�ʽ����

	//����ʽ һ��
	//������Ϣ�������ڣ����� OnBtnStartMaking ����
	//EventCmd evt(pPageMaking->M());
	//evt.idFrom = R.id.btn_start_making;
	//pPageMaking->M()->FireEvent(evt);

	//����ʽ ����
	//pPageMaking->OnBtnStartMaking();

	//��Ϊ�����ַ�ʽ�����ڱ��߳�ִ�У������̲߳������Ӱ�죬�����ڵ�ǰ�߳�ֱ�Ӳ���

	::PostMessage(pPageMaking->M()->m_hWnd,MSG_USER_MAKING_START_BUTTON,0,0);

	return true;
}


//������ʱ����Ncm�ļ�
// ���� 0 ��ʾ����ncm�ļ�
//		1 ����������ļ�Ϊncm�ļ�,���ļ�û��ƥ��ID
//		2 ����������ļ�Ϊncm�ļ�������mp3ʧ��
//		3 ����������ļ�Ϊncm�ļ�������mp3�ɹ���׼������
//		-1; //�����ļ��д���
int CPageMaking::DealWithNcmFile(OUT bool& isCurrentNcm, OUT wstring& strID, OUT wstring& strName, OUT wstring& strMp3FilePath)
{
	if(CFileDialogEx::checkPathName(_T("*.ncm"),M()->maker.m_szMusicPathName))
	{
		isCurrentNcm = true;

		//�õ��ļ�������ѯ��ǰ������id
		wstring strMusicPath = M()->maker.m_szMusicPathName;
		auto indexBeg = strMusicPath.find_last_of(L'\\')+1;
		auto indexEnd = strMusicPath.find_last_of(L'.');
		strName = strMusicPath.substr( indexBeg,indexEnd - indexBeg);

		if(!CNcmIDManager::GetInstance()->FindID(strName, strID))
			return 1;// 1��ʾncm ��ûƥ��ID

		//��ѯmp3 �Ƿ��Ѿ�����
		wstring strMp3FloderPath = FileHelper::GetCurrentDirectoryStr() + TEMP_MP3_FLODER_NAME ;
		strMp3FilePath = strMp3FloderPath + L"\\" + strName + L".mp3" ;
		if(FileHelper::CheckFileExist(strMp3FilePath))
			return 3; //�Ѿ����ڣ�׼������

		//ȷ��mp3�ļ��д���
		if(!FileHelper::CheckFolderExist(strMp3FloderPath))
		{
			if(RET_SUCCEEDED != _wmkdir(strMp3FloderPath.c_str()))
			{
				wstring strTip = L"�����޷�����Ŀ¼��\\n";
				strTip += strMp3FloderPath +L"\\n";
				strTip += L"�⽫����ncm�޷�����mp3,�Ӷ��޷���������\\n";
				
				_MessageBox(NULL, strTip.c_str(), L"��ʾ", MB_OK|MB_ICONWARNING);
				return -1; //�����ļ��д���
			}
		}

		//��������������mp3, ���̽�����ر�mp3
		
		DlgDownloadNcmMp3 dlg(L"xml_processing_tip");

		dlg.SetNcmNameAndID(strName,strID);
		
		int ret = dlg.DoModal(NULL);
		
		_MessageBox(NULL,L"",L"",MB_OK); //���ϴ˾�������ڹر�������������󣬲�֪�ǲ���ʹ��SOUI�ϵĴ��󣬵����Ĵ��ڹرպ�ü��ϴ˾䡰����������ر���Ϣ

		if(ret == IDCANCEL)
			return 2;//�����ʾʧ��
		else
			return 3;//�����Ѿ�ת������
	}
	else
		isCurrentNcm = false;

	return 0;
}









