#include "stdafx.h"
#include "SearchLyricThread.h"
#include "Define.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"

#include "entity\LyricSearcherFactory.h"

using namespace SOUI;

CSearchLyricThread* CSearchLyricThread::ms_Singleton = NULL;


//��ʼ�߳�
bool CSearchLyricThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
{
	m_hMainWnd = hMainWnd;
	m_strMusicName = strMusicName;			
	m_strMusicArtist = strMusicArtist;

	//��������߳�
	m_handleThreadSearch = ::CreateThread(NULL, 0, ProcSearch, this, 0 ,NULL);

	return (m_handleThreadSearch != NULL);
}


// �߳�ִ�е�ַ
DWORD WINAPI CSearchLyricThread::ProcSearch(LPVOID pParam)
{
	CSearchLyricThread* pThread = static_cast<CSearchLyricThread*>(pParam);


	vector<LyricInfo> vecLyricInfoTotal;	//�������л�õĸ��
	vector<LyricInfo> vecLyricInfo;			//��ʱ����ÿһ�λ�õ�
	CAutoRefPtr<ISearcher> pSearcher;

	bool bShowUnexpectedResultTip = false; //��ʾ��������ʾ

	SStringW strResultTip = L"";

	for( int i= 0; i < (int)SEARCH_FROM::UNDEFINED; i++) //����ÿһ�ֶ���ĸ�ʻ�ȡ��ʽ
	{
		pSearcher = nullptr;//�� CreateSearcher �ж� SOUI����ָ��ȡ ��ַ& ǰ������Ϊ�գ��ͷ���һ�η���ĶѶ���
		CLyricSearcherFactory::CreateSearcher((SEARCH_FROM)i, pSearcher);

		vecLyricInfo.clear();
		if(!pSearcher->SearchLyric(pThread->m_strMusicName, pThread->m_strMusicArtist, vecLyricInfo))
		{
			bShowUnexpectedResultTip = true;
			pSearcher->GetLastResult(strResultTip);
		}

		if(!vecLyricInfo.empty())
			vecLyricInfoTotal.insert(vecLyricInfoTotal.end(), vecLyricInfo.begin(), vecLyricInfo.end());
	}

	LyricSearchResult* pSearchResult = new LyricSearchResult;
	pSearchResult->vecLyricInfoTotal = vecLyricInfoTotal;
	pSearchResult->strUnexpectedResultTip = strResultTip;
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;

	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);

	return 0;
}
