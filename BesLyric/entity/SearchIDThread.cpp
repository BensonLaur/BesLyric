#include "stdafx.h"
#include "SearchLyricThread.h"
#include "Define.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"
#include "SearchIDThread.h"
#include "SearcherNetEaseCloud.h"

using namespace SOUI;

//��ʼ�߳�
bool CSearchIDThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
{
	if(m_bIsSearching)
	{
		_MessageBox(NULL,L"����ģ������ռ�ã���ȴ�.....",L"�ȴ���ʾ", MB_OK|MB_ICONINFORMATION);
		return true;
	}

	m_hMainWnd = hMainWnd;
	m_strMusicName = strMusicName;			
	m_strMusicArtist = strMusicArtist;

	//��������߳�
	m_handleThreadSearch = ::CreateThread(NULL, 0, ProcSearch, this, 0 ,NULL);

	return (m_handleThreadSearch != NULL);
}


// �߳�ִ�е�ַ
DWORD WINAPI CSearchIDThread::ProcSearch(LPVOID pParam)
{
	CSearchIDThread* pThread = static_cast<CSearchIDThread*>(pParam);

	pThread->m_bIsSearching = true; //��������״̬

	vector<IDInfo> vecLyricInfo;			//��ʱ����ÿһ�λ�õ�


	bool bShowUnexpectedResultTip = true; //��ʾ��������ʾ
	SStringW strResultTip = L"";

	
	vector< SONGINFO > vecSongList;
	SearcherNetEaseCloud::GetSongListWithNameAndArtist(pThread->m_strMusicName, pThread->m_strMusicArtist, vecSongList, strResultTip);


	for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
	{
		IDInfo info;
		info.strSong = iter->strSong;
		info.strArtist = iter->strArtists;
		info.strID = SStringW().Format(L"%d",iter->nID);
		vecLyricInfo.push_back(info);
	}

	IDSearchResult* pSearchResult = new IDSearchResult;
	pSearchResult->vecIDInfoTotal = vecLyricInfo;				
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;	//�Ƿ���ʾ�쳣��Ϣ
	pSearchResult->strUnexpectedResultTip = strResultTip;				//�쳣��Ϣ

	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_ID_RESULT, (WPARAM)pSearchResult,0);
	
	pThread->m_bIsSearching = false;

	return 0;
}
