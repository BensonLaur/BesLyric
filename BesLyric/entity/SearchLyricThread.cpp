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

//��ʼ�߳�
bool CSearchLyricThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
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
DWORD WINAPI CSearchLyricThread::ProcSearch(LPVOID pParam)
{
	CSearchLyricThread* pThread = static_cast<CSearchLyricThread*>(pParam);

	pThread->m_bIsSearching = true; //��������״̬

	vector<LyricInfo> vecLyricInfo;			//��ʱ����ÿһ�λ�õ�
	CAutoRefPtr<ISearcher> pSearcher;

	bool bShowUnexpectedResultTip = true; //��ʾ��������ʾ
	SStringW strResultTip = L"";

	int nTotalCount = 0;
	for( int i= 0; i < (int)SEARCH_FROM::UNDEFINED; i++) //����ÿһ�ֶ���ĸ�ʻ�ȡ��ʽ
	{
		pSearcher = nullptr;//�� CreateSearcher �ж� SOUI����ָ��ȡ ��ַ& ǰ������Ϊ�գ��ͷ���һ�η���ĶѶ���
		CLyricSearcherFactory::CreateSearcher((SEARCH_FROM)i, pSearcher);

		vecLyricInfo.clear();
		if(!pSearcher->SearchLyric(pThread->m_strMusicName, pThread->m_strMusicArtist, vecLyricInfo))
		{
			pSearcher->GetLastResult(strResultTip);  //�������һ�ε��쳣��Ϣ
		}
		else
		{
			bShowUnexpectedResultTip = false;  //ֻҪ���ֹ�һ�����������Ľ�����Ͳ���ʾ�쳣��Ϣ
		}
		
		if(!vecLyricInfo.empty())
		{
			LyricSearchResult* pSearchResult = new LyricSearchResult;
			pSearchResult->vecLyricInfoTotal = vecLyricInfo;
			//pSearchResult->strUnexpectedResultTip = strResultTip;
			pSearchResult->bShowUnexpectedResultTip = false;			//����ÿһ���������Ĺ��̶�����ʾ�쳣��Ϣ
			pSearchResult->bCurrentSearchDone = false;					//������û����
			pSearchResult->bAppendToList = (nTotalCount==0 ? false: true); //֮ǰ��û������������Ҫ����б�bAppendToList ��Ϊfalse
			nTotalCount += vecLyricInfo.size();

			::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);
		}
	}

	//�������һ���źţ���������
	LyricSearchResult* pSearchResult = new LyricSearchResult;
	pSearchResult->bCurrentSearchDone = true;							//�����������
	pSearchResult->bAppendToList = nTotalCount == 0 ? false:true;//������ʾ�쳣��Ϣ˵��û�л�ȡ���κ����ݣ�����б����򣬲�����б�
	pSearchResult->vecLyricInfoTotal = vector<LyricInfo>();				//���Ϳյ����ݻ�ȥ
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;	//�Ƿ���ʾ�쳣��Ϣ
	pSearchResult->strUnexpectedResultTip = strResultTip;				//�쳣��Ϣ
	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);
	
	pThread->m_bIsSearching = false;

	return 0;
}
