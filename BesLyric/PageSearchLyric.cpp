#include "stdafx.h"
#include "PageSearchLyric.h"
#include "controlEx\LyricListView.h"
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "utility\StringHelper.h"
#include "entity\SearchLyricThread.h"


CPageSearchLyric::CPageSearchLyric()
{
	m_editSearchLyricName = NULL;
	m_editSearchLyricArtist = NULL;

	m_btnSearchHere = NULL;
	m_btnSearchBaidu = NULL;

	m_wndSearchLyricTip = NULL;
	m_txtSearchNameTip = NULL;
	m_txtSearchArtistTip = NULL;
	m_wndSearchArtistTip = NULL;
	m_txtSearchResultTip = NULL;
	m_txtIsSearchingLyricTip = NULL;

	m_editOriginLyricPath = NULL;
	m_editLrcLyricPath = NULL;
	
	m_btnSelectOriginLyricPath = NULL;
	m_btnSelectLrcLyricPath = NULL;
	
	m_editOriginLyricName = NULL;
	m_editOriginLyricArtist = NULL;
	m_editLrcLyricName = NULL;
	m_editLrcLyricArtist = NULL;
	
	m_editOriginLyricContent = NULL;
	m_editLrcLyricContent = NULL;
	
	m_pMainWnd = NULL;		/* ������ָ�� */
}

//��ʼ������ҳ��
void CPageSearchLyric::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ô����г��õĿؼ���ָ��
	m_editSearchLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_name);
	m_editSearchLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_artist);

	m_btnSearchHere = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program);
	m_btnSearchBaidu = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_baidu);

	m_wndSearchLyricTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_tip);
	m_txtSearchNameTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_name_tip);
	m_txtSearchArtistTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_artist_tip);
	m_wndSearchArtistTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_artist_tip);
	m_txtSearchResultTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_result_tip);
	m_txtIsSearchingLyricTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_is_searching_lyric_tip);

	m_editOriginLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_path);
	m_editLrcLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_path);
	
	m_btnSelectOriginLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_origin_lyric_path);;
	m_btnSelectLrcLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_lrc_lyric_path);;
	
	m_editOriginLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_name);;
	m_editOriginLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_artist);;
	m_editLrcLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_name);;
	m_editLrcLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_artist);;
	
	m_editOriginLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_content);;
	m_editLrcLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_content);;
	
	SASSERT(m_editSearchLyricName != NULL);
	SASSERT(m_editSearchLyricArtist != NULL);
	SASSERT(m_btnSearchHere != NULL);
	SASSERT(m_btnSearchBaidu != NULL);
	SASSERT(m_wndSearchLyricTip != NULL);
	SASSERT(m_txtSearchNameTip != NULL);
	SASSERT(m_txtSearchArtistTip != NULL);
	SASSERT(m_wndSearchArtistTip != NULL);
	SASSERT(m_txtSearchResultTip != NULL);
	SASSERT(m_txtIsSearchingLyricTip != NULL);
	SASSERT(m_editOriginLyricPath != NULL);
	SASSERT(m_editLrcLyricPath != NULL);
	SASSERT(m_btnSelectOriginLyricPath != NULL);
	SASSERT(m_btnSelectLrcLyricPath != NULL);
	SASSERT(m_editOriginLyricName != NULL);
	SASSERT(m_editOriginLyricArtist != NULL);
	SASSERT(m_editLrcLyricName != NULL);
	SASSERT(m_editLrcLyricArtist != NULL);
	SASSERT(m_editOriginLyricContent != NULL);
	SASSERT(m_editLrcLyricContent != NULL);

	m_wndSearchLyricTip->SetVisible(FALSE, TRUE);	//����������ʾ

	//��ʼ����ʱ���·��
	m_editOriginLyricPath->SetWindowTextW(M()->m_settingPage.m_default_lyric_path.c_str());
	m_editLrcLyricPath->SetWindowTextW(M()->m_settingPage.m_default_output_path.c_str());

	//����listview
    CLyricListView * pMcListView = m_pMainWnd->FindChildByName2<CLyricListView>("mclv_lyric");
    if(pMcListView)
    {
        IMcAdapter *pAdapter = m_lrcListAdapter =new CLyricMcAdapterFix;
		m_lrcListAdapter->AttachMainDlgPointer(M());
        pMcListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }
}

//��������ڶ���
CMainDlg* CPageSearchLyric::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	����Ԥ��ҳ�����Ӧ����
*/

//�ڰٶ��������
void CPageSearchLyric::OnBtnSearchBaidu()
{
	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;
	
	//�������е��ַ�����ת������ʹ�������ܹ�֧��
	/*
	"\"����б��  %5C 
	"|"      %7C 
	�س�  %0D%0A 
	�ո�  %20 
	˫���� %22 
	"&"		%26 
	*/
	wstring sMusicName = L"";
	wstring sMusicArtist = L"";
	for(auto i = 0;  i < strMusicName.GetLength(); i++)
	{
		switch(strMusicName.GetAt(i))
		{
		case L'\\':
			sMusicName += L"%5C";
			break;
		case L'|':
			sMusicName += L"%7C";
			break;
		case L'\n':
			sMusicName += L"%0D%0A";
			break;
		case L' ':
			sMusicName += L"%20";
			break;
		case L'"':
			sMusicName += L"%22";
			break;
		case L'&':
			sMusicName += L"%26";
			break;
		default:
			sMusicName += strMusicName.GetAt(i);
		}
	}

	if(strMusicArtist.GetLength() != 0)
	for(auto i = 0;  i < strMusicArtist.GetLength(); i++)
	{
		switch(strMusicArtist.GetAt(i))
		{
		case L'\\':
			sMusicArtist += L"%5C";
			break;
		case L'|':
			sMusicArtist += L"%7C";
			break;
		case L'\n':
			sMusicArtist += L"%0D%0A";
			break;
		case L' ':
			sMusicArtist += L"%20";
			break;
		case L'"':
			sMusicArtist += L"%22";
			break;
		case L'&':
			sMusicArtist += L"%26";
			break;
		default:
			sMusicArtist += strMusicArtist.GetAt(i);
		}
	}

	wstring sParam = L"start https://www.baidu.com/s?wd=%22";
	sParam += sMusicName.c_str();

	if(strMusicArtist.GetLength() != 0)
	{
		sParam += L"%22%20%22";
		sParam += sMusicArtist.c_str();
	}

	sParam += L"%22%20%22���%22";

	//ʹ�ÿ���̨��Ĭ��������ٶ��������
	_wsystem(sParam.c_str());
	
}


//ͨ��������ڻ�ȡ���
void CPageSearchLyric::OnBtnSearchInProgram()
{
	if(CSearchLyricThread::getSingleton().IsSearching())
	{
		_MessageBox(NULL,L"����ģ������ռ�ã���ȴ�.....",L"�ȴ���ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;

	//�л����б�ҳ��
	M()->FindChildByID2<STabCtrl>(R.id.tab_lyric_list)->SetCurSel(0);

	//��ʾ��������
	m_txtSearchNameTip->SetWindowTextW(L"��"+ strMusicName  + L"��");
	m_txtSearchArtistTip->SetWindowTextW(L"��"+ strMusicArtist  + L"��");

	m_wndSearchArtistTip->SetVisible( strMusicArtist.GetLength() == 0 ? FALSE:TRUE, TRUE);
	m_txtSearchResultTip->SetVisible(FALSE,TRUE);
	m_txtIsSearchingLyricTip->SetVisible(TRUE,TRUE);
	
	m_wndSearchLyricTip->SetVisible(TRUE, TRUE); //��ʾ��ʾ

	//���ذ�ť�����ü�������
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program)->EnableWindow(FALSE, TRUE);
	

	//������������߳�
	CSearchLyricThread::getSingleton().Start(M()->m_hWnd, strMusicName,strMusicArtist);
}

void CPageSearchLyric::ShowLyricResult(LyricSearchResult* pResult)
{
	//ɾ���б������е���������
	static int nLyricCount = 0;
	if(!pResult->bAppendToList)  
	{
		nLyricCount = 0;
		m_lrcListAdapter->DeleteAllItem();
	}

	SStringW strResultTip = L"";
	if(pResult->bShowUnexpectedResultTip)
		strResultTip = pResult->strUnexpectedResultTip;

	//��ʾ�������
	nLyricCount += pResult->vecLyricInfoTotal.size();
	m_txtSearchResultTip->SetWindowTextW(SStringW().Format(L"���ҵ�%d������ļ��� %s",nLyricCount , strResultTip.GetBuffer(1) ));
	m_txtSearchResultTip->SetVisible(TRUE,TRUE);

	for(auto iter = pResult->vecLyricInfoTotal.begin(); iter != pResult->vecLyricInfoTotal.end(); iter++)
		m_lrcListAdapter->AddItem( iter->strSong, iter->strArtist, iter->strLyricFrom,iter->strPlaneText, iter->strLabelText);
	m_lrcListAdapter->notifyDataSetChanged();
	
	if(pResult->bCurrentSearchDone) //��������ʱ���ָܻ���ť
	{
		//�ָ���ť��������һ�μ�������
		m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program)->EnableWindow(TRUE, TRUE);
		
		m_txtIsSearchingLyricTip->SetVisible(FALSE,TRUE); //��������������ʾ
	}
}

//ѡ��ԭ��ʱ���·��
void CPageSearchLyric::OnBtnSelectOriginLyricPath(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		SStringW pathBefore = m_editOriginLyricPath->GetWindowTextW();
		CBrowseDlg Browser;
		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��ԭ��ʡ� ����·��"), TRUE, pathBefore);

		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		m_editOriginLyricPath->SetWindowTextW(pPath);
	}
}

//ѡ��Lrc��ʱ���·��
void CPageSearchLyric::OnBtnSelectLrcLyricPath(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		SStringW pathBefore = m_editLrcLyricPath->GetWindowTextW();
		CBrowseDlg Browser;
		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��LRC��ʡ� ����·��"), TRUE, pathBefore);
		
		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		m_editLrcLyricPath->SetWindowTextW(pPath);
	}
}	

//����ԭ���
void CPageSearchLyric::OnBtnSaveOriginLyricPath()
{
	SStringW strContent = CStringHelper::Trim(m_editOriginLyricContent->GetWindowTextW(),L" \t\r\n");

	if(strContent.GetLength() == 0)
	{
		_MessageBox(M()->m_hWnd, L"�������Ϊ�գ��޷����棡", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strPath = m_editOriginLyricPath->GetWindowTextW();
	SStringW strSong = CStringHelper::Trim(m_editOriginLyricName->GetWindowTextW());
	SStringW strArtist = CStringHelper::Trim(m_editOriginLyricArtist->GetWindowTextW());
	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";

	CFileDialogEx saveDlg(FALSE, strArtist+ strConnector+ strSong ,strPath.GetBuffer(1),L"txt",L"ԭ����ļ�(txt)\0*.txt;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
	int ret = saveDlg.DoModal();
	if(ret == TRUE)
	{
		//���ı����ݱ��浽�ļ���
		SStringW savePath = saveDlg.m_szFileName;
		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
		{
			int nRet = _MessageBox(M()->m_hWnd, (L"ԭ����ѱ��棡\\n\\n����·����\\n"+savePath + L"\\n\\n�Ƿ�ֱ��ѡ��ø�ʵ������������ҳ�棿").GetBuffer(1), 
				L"��ʾ", MB_YESNO|MB_ICONINFORMATION);
			if(nRet == IDYES)
			{
				//�л���[�������]ҳ��
				STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
				if(tab)
					tab->SetCurSel(0);

				M()->m_pageMaking->OnBtnSelectLyric1(savePath);//ѡ��[�������]ҳ��
			}
		}
		else
			_MessageBox(M()->m_hWnd, (L"�������ʧ�ܣ�\\n\\n·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONWARNING);
	}
}

//����Lrc���
void CPageSearchLyric::OnBtnSaveLrcLyricPath()
{
	SStringW strContent = CStringHelper::Trim(m_editLrcLyricContent->GetWindowTextW(),L" \t\r\n");

	if(strContent.GetLength() == 0)
	{
		_MessageBox(M()->m_hWnd, L"�������Ϊ�գ��޷����棡", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strPath = m_editLrcLyricPath->GetWindowTextW();
	SStringW strSong = CStringHelper::Trim(m_editLrcLyricName->GetWindowTextW());
	SStringW strArtist = CStringHelper::Trim(m_editLrcLyricArtist->GetWindowTextW());
	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";

	CFileDialogEx saveDlg(FALSE, strSong+ strConnector+ strArtist ,strPath.GetBuffer(1),L"lrc",L"LRC����ļ�(lrc)\0*.lrc;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
	int ret = saveDlg.DoModal();
	if(ret == TRUE)
	{
		//���ı����ݱ��浽�ļ���
		SStringW savePath = saveDlg.m_szFileName;
		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
		{
			int nRet = _MessageBox(M()->m_hWnd, (L"LRC����ѱ��棡\\n\\n����·����\\n"+savePath+ L"\\n\\n�Ƿ�ֱ��ѡ��ø�ʵ�������Ԥ����ҳ�棿").GetBuffer(1), 
			L"��ʾ", MB_YESNO|MB_ICONINFORMATION);
			if(nRet == IDYES)
			{
				//�л���[����Ԥ��]ҳ��
				STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
				if(tab)
					tab->SetCurSel(1);

				M()->m_pageResult->OnBtnSelectLyric2(savePath);//ѡ��[�������]ҳ��
			}
		}
		else
			_MessageBox(M()->m_hWnd, (L"�������ʧ�ܣ�\\n\\n·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONWARNING);
	}
}	


//���ݲ²����Զ���������ؼ��ʲ���ʼ����
void CPageSearchLyric::OnSearchWithGuess(SongInfoGuessResult* pGuessRes)
{
	SASSERT( pGuessRes->nResultType == 1 || pGuessRes->nResultType == 2);

	//������²���һ�����������
	m_editSearchLyricName->SetWindowTextW(pGuessRes->strSongName);

	if(pGuessRes->nResultType == 1)//������
	{
		m_editSearchLyricArtist->SetWindowTextW(pGuessRes->strArtist);
	}
	else
	{
		m_editSearchLyricArtist->SetWindowTextW(SStringW());
	}

	OnBtnSearchInProgram();
}


//��ý�������д�ĸ������͸�����
bool CPageSearchLyric::GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist)
{
	strMusicName = m_editSearchLyricName->GetWindowTextW();
	strMusicArtist = m_editSearchLyricArtist->GetWindowTextW();
	
	strMusicName = CStringHelper::Trim(strMusicName);
	strMusicArtist = CStringHelper::Trim(strMusicArtist);

	if(strMusicName.GetLength() == 0)
	{
		_MessageBox(m_pMainWnd->m_hWnd, L"����������Ϊ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return false;
	}

	return true;
}


