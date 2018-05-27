#include "stdafx.h"
#include "PageSearchNcmID.h"
#include "controlEx\LyricListView.h"
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "utility\StringHelper.h"
#include "entity\SearchIDThread.h"
#include "entity\NcmIDManager.h"
#include "utility\Downloader.h"

#include "entity\LoadAndCheckNcmIDThread.h"

CPageSearchNcmID::CPageSearchNcmID()
{
	m_pMainWnd = NULL;		/* ������ָ�� */

	m_window_search_ncm_id_tip = NULL;
	m_editSongFileName = NULL;
	m_editNcmID = NULL;
	m_editSearchName= NULL;
	m_editSearchArtist= NULL;

	
	m_txtSearchNameTip =NULL;
	m_txtSearchArtistTip  =NULL;
	m_wndSearchArtistTip =NULL;
	m_txtSearchResultTip =NULL;
	m_txtIsSearchingIDTip =NULL;
}

//��ʼ������ҳ��
void CPageSearchNcmID::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;
	
	//��ô����г��õĿؼ���ָ��
	m_window_search_ncm_id_tip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_ncm_id_tip);
	m_editSongFileName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_song_file_name);
	m_editNcmID = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_ncm_id);
	
	m_editSearchName= m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_ncm_id_name);
	m_editSearchArtist= m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_ncm_id_artist);
	
	
	m_txtSearchNameTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_name_tip);
	m_txtSearchArtistTip  = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_artist_tip);
	m_wndSearchArtistTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_ncm_id_artist_tip);
	m_txtSearchResultTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_result_tip);
	m_txtIsSearchingIDTip =m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_is_searching_ncm_id_tip);


	SASSERT(m_window_search_ncm_id_tip != NULL);
	SASSERT(m_editSongFileName != NULL);
	SASSERT(m_editNcmID != NULL);
	SASSERT(m_editSearchName != NULL);
	SASSERT(m_editSearchArtist != NULL);
	

	m_window_search_ncm_id_tip->SetVisible(FALSE, TRUE);	//����������ʾ

	//����listview
    CLyricListView * pMcListView = m_pMainWnd->FindChildByName2<CLyricListView>("mclv_ncm_id");
    if(pMcListView)
    {
        IMcAdapter *pAdapter = m_IDListAdapter =new CSongIDMcAdapterFix;
		m_IDListAdapter->AttachMainDlgPointer(M());
        pMcListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }

	//���̼߳��غͼ��NCMID ,֮����ʹ���̣߳�����Ϊ�ò���������ʱ
	CLoadAndCheckNcmIDThread::getSingleton().Start();
}

//��������ڶ���
CMainDlg* CPageSearchNcmID::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	��Ӧ����
*/

//������Ҫ����ҳ��
void CPageSearchNcmID::OnBtnReturnMain()
{
	M()->FindChildByID2<STabCtrl>(R.id.tab_content_container)->SetCurSel(0); //����
}


//ʹ���ļ�·�����ļ����������ļ����༭�ؼ�
void CPageSearchNcmID::SetSongFileNameEditWithPath(wstring strFilePath)
{
	wstring strFileName = L"";

	auto indexSlash = strFilePath.find_last_of(L'\\');
	if(indexSlash != wstring::npos)
	{
		wstring strFile = strFilePath.substr(indexSlash+1);
		if(strFile.size() != 0)
		{
			auto indexDot = strFile.find_last_of(L'.');
			strFileName = strFile.substr(0,indexDot);
		}
	}

	//ʹ�� strFileName ������༭��
	m_editSongFileName->SetWindowTextW(strFileName.c_str());

	//������ļ����Ѿ�ƥ����ID������ʾID����Ӧ�ı༭��
	wstring strID = L"";
	CNcmIDManager::GetInstance()->FindID(strFileName, strID); //������ҵ����������µ�strID
		
	m_editNcmID->SetWindowTextW(strID.c_str());
}


//ѡ��ID��Ϊ��ǰ������ncm ID
void CPageSearchNcmID::OnBtnSelectID()
{
	wstring strName = m_editSongFileName->GetWindowTextW().Trim(' ').GetBuffer(1);
	wstring strID = m_editNcmID->GetWindowTextW().Trim(' ').GetBuffer(1);

	if(strID.size() == 0)
	{
		_MessageBox(M()->m_hWnd, L"ID����Ϊ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	//������ID���Ƿ��ID��Ч�����ݲ����ʵĽ�����ж��Ƿ���Ч��
	bool bValid = false;
	if(CNcmIDManager::GetInstance()->CheckIDValidity(strID,bValid))
	{
		if(!bValid)
		{
			_MessageBox(M()->m_hWnd, L"��ǰ��дIDΪ��ЧID��ƥ��IDʧ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"��ǰ�޷���ѯ��ƥ��IDʧ��\\n������������������", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	//����оɵ�ID�������Ƿ��֮ǰ��ͬ����ͬ�Ļ�����Ҫɾ��mp3Ŀ¼�µ� mp3�ļ����Ժ����������أ���ɾ���Ļ������ǻᲥ��ԭ����mp3
	wstring strOldID = L"";
	bool bHasOld = CNcmIDManager::GetInstance()->FindID(strName,strOldID);
	if(bHasOld)
	{
		if(strOldID != strID)
		{
			wstring strMp3FloderPath = FileHelper::GetCurrentDirectoryStr() + TEMP_MP3_FLODER_NAME ;
			wstring strMp3FilePath = strMp3FloderPath + L"\\" + strName + L".mp3" ;
			if(FileHelper::CheckFileExist(strMp3FilePath))
				_wremove(strMp3FilePath.c_str());
		}
	}

	//��Ӽ�ֵ��
	CNcmIDManager::GetInstance()->InsertNcmIDPair(strName, strID);

	if(!CNcmIDManager::GetInstance()->SaveDataPairs())
	{
		_MessageBox(M()->m_hWnd, L"�޷��ɹ��������ݣ���Ӽ�ֵ��ʧ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	OnBtnReturnMain();
}



//����ID
void CPageSearchNcmID::OnBtnSearch()
{
	if(CSearchIDThread::getSingleton().IsSearching())
	{
		_MessageBox(NULL,L"����ģ������ռ�ã���ȴ�.....",L"�ȴ���ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;

	//��ʾ��������
	m_txtSearchNameTip->SetWindowTextW(L"��"+ strMusicName  + L"��");
	m_txtSearchArtistTip->SetWindowTextW(L"��"+ strMusicArtist  + L"��");

	m_wndSearchArtistTip->SetVisible( strMusicArtist.GetLength() == 0 ? FALSE:TRUE, TRUE);
	m_txtSearchResultTip->SetVisible(FALSE,TRUE);
	m_txtIsSearchingIDTip->SetVisible(TRUE,TRUE);
	
	m_window_search_ncm_id_tip->SetVisible(TRUE, TRUE); //��ʾ��ʾ

	//���ذ�ť�����ü�������
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_ncm_id)->EnableWindow(FALSE, TRUE);
	

	//��������ID�߳�
	CSearchIDThread::getSingleton().Start(M()->m_hWnd, strMusicName,strMusicArtist);
}

void CPageSearchNcmID::ShowIDResult(IDSearchResult* pResult)
{
	//ɾ���б������е���������
	m_IDListAdapter->DeleteAllItem();

	SStringW strResultTip = L"";
	if(pResult->bShowUnexpectedResultTip)
		strResultTip = pResult->strUnexpectedResultTip;

	//��ʾ�������
	m_txtSearchResultTip->SetWindowTextW(SStringW().Format(L"���ҵ�%d��ID�� %s",pResult->vecIDInfoTotal.size() , strResultTip.GetBuffer(1) ));
	m_txtSearchResultTip->SetVisible(TRUE,TRUE);

	for(auto iter = pResult->vecIDInfoTotal.begin(); iter != pResult->vecIDInfoTotal.end(); iter++)
		m_IDListAdapter->AddItem( iter->strSong, iter->strArtist, iter->strID);
	m_IDListAdapter->notifyDataSetChanged();
	

	//�ָ���ť��������һ�μ�������
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_ncm_id)->EnableWindow(TRUE, TRUE);
	
	m_txtIsSearchingIDTip->SetVisible(FALSE,TRUE); //��������������ʾ
	
}

////ѡ��ԭ��ʱ���·��
//void CPageSearchNcmID::OnBtnSelectOriginLyricPath(LPCWSTR pFilePath)
//{
//	BOOL bRet = TRUE;
//	LPCWSTR pPath = NULL;
//
//	if(pFilePath == NULL)
//	{
//		SStringW pathBefore = m_editOriginLyricPath->GetWindowTextW();
//		CBrowseDlg Browser;
//		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��ԭ��ʡ� ����·��"), TRUE, pathBefore);
//
//		if(bRet == TRUE)
//			pPath = Browser.GetDirPath();
//	}
//	else
//		pPath = pFilePath;
//
//	if(bRet == TRUE)
//	{
//		m_editOriginLyricPath->SetWindowTextW(pPath);
//	}
//}
//
////ѡ��Lrc��ʱ���·��
//void CPageSearchNcmID::OnBtnSelectLrcLyricPath(LPCWSTR pFilePath)
//{
//	BOOL bRet = TRUE;
//	LPCWSTR pPath = NULL;
//
//	if(pFilePath == NULL)
//	{
//		SStringW pathBefore = m_editLrcLyricPath->GetWindowTextW();
//		CBrowseDlg Browser;
//		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��LRC��ʡ� ����·��"), TRUE, pathBefore);
//		
//		if(bRet == TRUE)
//			pPath = Browser.GetDirPath();
//	}
//	else
//		pPath = pFilePath;
//
//	if(bRet == TRUE)
//	{
//		m_editLrcLyricPath->SetWindowTextW(pPath);
//	}
//}	
//
////����ԭ���
//void CPageSearchNcmID::OnBtnSaveOriginLyricPath()
//{
//	SStringW strContent = CStringHelper::Trim(m_editOriginLyricContent->GetWindowTextW(),L" \t\r\n");
//
//	if(strContent.GetLength() == 0)
//	{
//		_MessageBox(M()->m_hWnd, L"�������Ϊ�գ��޷����棡", L"��ʾ", MB_OK|MB_ICONINFORMATION);
//		return;
//	}
//
//	SStringW strPath = m_editOriginLyricPath->GetWindowTextW();
//	SStringW strSong = CStringHelper::Trim(m_editOriginLyricName->GetWindowTextW());
//	SStringW strArtist = CStringHelper::Trim(m_editOriginLyricArtist->GetWindowTextW());
//	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";
//
//	CFileDialogEx saveDlg(FALSE, strArtist+ strConnector+ strSong ,strPath.GetBuffer(1),L"txt",L"ԭ����ļ�(txt)\0*.txt;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
//	int ret = saveDlg.DoModal();
//	if(ret == TRUE)
//	{
//		//���ı����ݱ��浽�ļ���
//		SStringW savePath = saveDlg.m_szFileName;
//		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
//			_MessageBox(M()->m_hWnd, (L"ԭ����ѱ��棡\\n\\n����·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONINFORMATION);
//		else
//			_MessageBox(M()->m_hWnd, (L"�������ʧ�ܣ�\\n\\n·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONWARNING);
//	}
//}
//
////����Lrc���
//void CPageSearchNcmID::OnBtnSaveLrcLyricPath()
//{
//	SStringW strContent = CStringHelper::Trim(m_editLrcLyricContent->GetWindowTextW(),L" \t\r\n");
//
//	if(strContent.GetLength() == 0)
//	{
//		_MessageBox(M()->m_hWnd, L"�������Ϊ�գ��޷����棡", L"��ʾ", MB_OK|MB_ICONINFORMATION);
//		return;
//	}
//
//	SStringW strPath = m_editLrcLyricPath->GetWindowTextW();
//	SStringW strSong = CStringHelper::Trim(m_editLrcLyricName->GetWindowTextW());
//	SStringW strArtist = CStringHelper::Trim(m_editLrcLyricArtist->GetWindowTextW());
//	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";
//
//	CFileDialogEx saveDlg(FALSE, strSong+ strConnector+ strArtist ,strPath.GetBuffer(1),L"lrc",L"LRC����ļ�(lrc)\0*.lrc;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
//	int ret = saveDlg.DoModal();
//	if(ret == TRUE)
//	{
//		//���ı����ݱ��浽�ļ���
//		SStringW savePath = saveDlg.m_szFileName;
//		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
//			_MessageBox(M()->m_hWnd, (L"LRC����ѱ��棡\\n\\n����·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONINFORMATION);
//		else
//			_MessageBox(M()->m_hWnd, (L"�������ʧ�ܣ�\\n\\n·����\\n"+savePath).GetBuffer(1), L"��ʾ", MB_OK|MB_ICONWARNING);
//	}
//}	
//

//���ݲ²����Զ���������ؼ��ʲ���ʼ����
void CPageSearchNcmID::OnSearchWithGuess(SongInfoGuessResult* pGuessRes)
{
	SASSERT( pGuessRes->nResultType == 1 || pGuessRes->nResultType == 2);

	//������²���һ�����������
	m_editSearchName->SetWindowTextW(pGuessRes->strSongName);

	if(pGuessRes->nResultType == 1)//������
	{
		m_editSearchArtist->SetWindowTextW(pGuessRes->strArtist);
	}
	else
	{
		m_editSearchArtist->SetWindowTextW(SStringW());
	}

	OnBtnSearch();
}


//��ý�������д�ĸ������͸�����
bool CPageSearchNcmID::GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist)
{
	strMusicName = m_editSearchName->GetWindowTextW();
	strMusicArtist = m_editSearchArtist->GetWindowTextW();
	
	strMusicName = CStringHelper::Trim(strMusicName);
	strMusicArtist = CStringHelper::Trim(strMusicArtist);

	if(strMusicName.GetLength() == 0)
	{
		_MessageBox(m_pMainWnd->m_hWnd, L"����������Ϊ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return false;
	}

	return true;
}


