#include "stdafx.h"
#include "GuessLyricInfoThread.h"
#include "Define.h"
#include <wininet.h>					//��������
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\utility\StringHelper.h"
#include "..\DlgCheckIntegrity.h"

#include "..\utility\HttpRequest.h"
#include "..\utility\UrlEncoding.h"
#include "..\lib\json\json.h"

#include "entity\LyricSearcherFactory.h"


using namespace SOUI;

//��ʼ�߳�
bool CGuessLyricInfoThread::Start(HWND hMainWnd, SStringW& strMusicPath, bool isForNcmID)
{
	if(m_bIsGuessing)
	{
		_MessageBox(NULL,L"���ڷ���������Ϣ����.....",L"�ȴ���ʾ", MB_OK|MB_ICONINFORMATION);
		return true;
	}

	m_hMainWnd = hMainWnd;
	m_strMusicPath = strMusicPath;
	m_isForNcmID = isForNcmID;

	//�����߳�
	m_handleThreadGuess = ::CreateThread(NULL, 0, ProcGuessLyricInfo, this, 0 ,NULL);

	return (m_handleThreadGuess != NULL);
}


//��mp3�ļ��л�ȡ�²���
bool CGuessLyricInfoThread::GetGuessResultFromMp3(SongInfoGuessResult* pGuessRes )
{
	pGuessRes->strArtist = SStringW();
	pGuessRes->strSongName = SStringW();

	//��ȡ�ļ����� �� m_pFileData ָ����ڴ���
	FILE *pf;
	errno_t err = _tfopen_s(&pf, m_strMusicPath, L"rb");
	if (err != 0)
		return false;

	fseek(pf, 0, SEEK_END);
	int nFileSize = ftell(pf);										//��ȡ�ļ���С
	rewind(pf);

	PBYTE pFileData = (PBYTE)malloc(sizeof(BYTE)* nFileSize);			//���������ļ��Ĵ�С����ȡ�ļ����ݿ��ָ��
	if (pFileData == NULL)
		return false;

	if (fread(pFileData, 1, nFileSize, pf) != nFileSize)		//�ļ�������m_pFileData��
		return false;

	fclose(pf);

	//��������͸���
	int nGetCount = 0;//���������
	for(int i=0;i <nFileSize; i++)
	{
		if(nGetCount >=2 )
			break;
		//��ǩ�ṹ
		//char FrameID[4];   
		//char Size[4];   
		//char Flags[2];  
		
		union _USize{byte bit[4]; int nSize;} USize;
			
		if( pFileData[i] == 'T' && pFileData[i+1] == 'I' && pFileData[i+2] == 'T' && pFileData[i+3] == '2') //����
		{
			USize.bit[0] = pFileData[i+4+3];
			USize.bit[1] = pFileData[i+4+2];
			USize.bit[2] = pFileData[i+4+1];
			USize.bit[3] = pFileData[i+4+0];
			
			int nSize = USize.nSize;  

			if(nSize <= 3 || (nSize-3)%2!=0) //�����ǩ��С����3ʱ�������ݣ���ֻ����ȥ��bomͷ��ʣ��ż����
				continue;

			BYTE szBom[3];
			char* pszTitle = new char[nSize-3];
			
			memcpy(szBom, pFileData+i+4+6, 3);
			memcpy(pszTitle, pFileData+i+4+9, nSize-3);

			//if(szBom[0] == 0x01 && szBom[1] == 0xff && szBom[2] == 0xfe){}
			
			pGuessRes->strSongName = S_CW2W( SStringW( (wchar_t*)pszTitle, (nSize-3)/2));

			delete pszTitle;
			nGetCount++;
		}
		else if(pFileData[i] == 'T' && pFileData[i+1] == 'P' && pFileData[i+2] == 'E' && pFileData[i+3] == '1')//����
		{
			USize.bit[0] = pFileData[i+4+3];
			USize.bit[1] = pFileData[i+4+2];
			USize.bit[2] = pFileData[i+4+1];
			USize.bit[3] = pFileData[i+4+0];
			
			int nSize = USize.nSize;

			if(nSize <= 3 || (nSize-3)%2!=0) //�����ǩ��С����3ʱ�������ݣ���ֻ����ȥ��bomͷ��ʣ��ż����
				continue;

			BYTE szBom[3];
			char* pszArtist = new char[nSize-3];
			
			memcpy(szBom, pFileData+i+4+6, 3);
			memcpy(pszArtist, pFileData+i+4+9, nSize-3);

			//if(szBom[0] == 0x01 && szBom[1] == 0xff && szBom[2] == 0xfe){}
			
			pGuessRes->strArtist = S_CW2W( SStringW( (wchar_t*)pszArtist, (nSize-3)/2));

			delete pszArtist;
			nGetCount++;
		}
	}

	if(pGuessRes->strSongName.GetLength() == 0)
	{
		pGuessRes->nResultType = 3; //û�л�ȡ������
		return false;
	}
	else
	{
		if(pGuessRes->strArtist.GetLength() == 0)
			pGuessRes->nResultType = 2; //ֻ��ȡ��������
		else
			pGuessRes->nResultType = 1;//��ȡ���ĸ�������������
	}

	return true;
}


//�������ƽӿڷ���ǰ���ַ����Ƿ�Ϊ����
bool CGuessLyricInfoThread::GuessWhetherFirstArtistFromNetease(SStringW& strFirst, SStringW& strSecond)
{
	CHttpRequest httpRequest;
	wstring wstrFirst = S_CW2W(strFirst).GetBuffer(1);
	wstring wstrSecond = S_CW2W(strSecond).GetBuffer(1);

	string strFirstUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strFirst).GetBuffer(1));
	string strSecondUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strSecond).GetBuffer(1));

	wstring strRes;
	if(!httpRequest.Post( "music.163.com/api/search/get/web", 
								"csrf_token=&s="+strFirstUrl+"+"+ strSecondUrl +"&type=1&offset=0&total=True&limit=8",
								strRes))
	{
		//L"��������ʧ�ܣ��޷���ȡ�����������",Ĭ�Ϸ���true���²��һ���ַ���������
		return true;
	}
	
	strRes = strRes.substr( strRes.find_first_of('{'), strRes.find_last_of('}') - strRes.find_first_of('{')+1);

	//��ȡid�б�
	vector< SONGINFO > vecSongList;
	if(!SearcherNetEaseCloud::GetSongListFromJson(strRes, vecSongList))
	{
		//L"�����Ƹ�����ݸ�ʽ�쳣���޷���������";,Ĭ�Ϸ���true���²��һ���ַ���������
		return true;
	}
	
	int nFirstArtistCount;
	int nFirstSongCount;
	int nSecondArtistCount;
	int nSecondSongCount;

	nFirstArtistCount = nFirstSongCount = nSecondArtistCount = nSecondSongCount = 0;

	for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
	{	
		if( wstring::npos != iter->strArtists.find(wstrFirst))//��һ���������ڽ�������
			nFirstArtistCount++;
		if( wstring::npos != iter->strSong.find(wstrFirst))//��һ���������ڽ��������
			nFirstSongCount++;
		if( wstring::npos != iter->strArtists.find(wstrSecond))//�ڶ����������ڽ�������
			nSecondArtistCount++;
		if( wstring::npos != iter->strSong.find(wstrSecond))//�ڶ����������ڽ��������
			nSecondSongCount++;
	}
	
	//����2���ַ����ĵ÷�
	int nPointFirstGot = 0;
	int nPointSecondGot = 0;

	if( nFirstArtistCount > nFirstSongCount)
		nPointFirstGot++;
	if( nSecondArtistCount > nSecondSongCount)
		nPointSecondGot++;

	if(nPointFirstGot >= nPointSecondGot)
		return true;
	else
		return false;
}


// �߳�ִ�е�ַ
DWORD WINAPI CGuessLyricInfoThread::ProcGuessLyricInfo(LPVOID pParam)
{
	CGuessLyricInfoThread* pThread = static_cast<CGuessLyricInfoThread*>(pParam);

	pThread->m_bIsGuessing = true;//��ǲ²�״̬

	//����ļ��������� �� ��׺
	SStringW strName;
	SStringW strExt;

	int nLastDot = -1;
	int nLastSlash = -1;
	for(int i = pThread->m_strMusicPath.GetLength()-1; i >=0 ; i--)
	{
		if(pThread->m_strMusicPath.GetAt(i) == L'.')
		{
			nLastDot = i;
		}
		else if(pThread->m_strMusicPath.GetAt(i) == L'\\')
		{
			nLastSlash = i;
			break;
		}
	}

	SASSERT( nLastDot != -1 && nLastSlash != -1 && "����·������");
	strExt = pThread->m_strMusicPath.Right( pThread->m_strMusicPath.GetLength() - nLastDot-1);
	strName = pThread->m_strMusicPath.Right( pThread->m_strMusicPath.GetLength() - nLastSlash-1).Left(nLastDot - nLastSlash-1);
	
	SongInfoGuessResult* pGuessRes = new SongInfoGuessResult(); //����²�Ľ��

	//�����mp3����ȡ��ȡ�������ļ�
	bool bHasGuessed = false;
	if(strExt.Compare(SStringW(L"mp3")) == 0)
	{
		if(pThread->GetGuessResultFromMp3( pGuessRes ))
			bHasGuessed = true;
	}

	//���û�в²������������ļ����²���
	if(!bHasGuessed)
	{
		//���ļ��õ�"-" 2���ֶ�
		int nIndex = strName.Find( L"-");
		if(nIndex == -1)
		{
			//���ֻ��1���ֶΣ�ֱ����Ϊ����������
			pGuessRes->nResultType = 2;
			pGuessRes->strSongName = strName;
		}
		else
		{
			SStringW s1 = CStringHelper::Trim( strName.Left( nIndex ));
			SStringW s2 = CStringHelper::Trim( strName.Right( strName.GetLength() - nIndex -1 ));

			//����õ�2���ֶΣ���ͨ�������ƽӿ������õ��Ľ����ȷ���ĸ����ļ������ĸ��Ǹ�����
			if(pThread->GuessWhetherFirstArtistFromNetease( s1, s2))
			{
				pGuessRes->strArtist = s1;
				pGuessRes->strSongName = s2;		
			}
			else{
				pGuessRes->strArtist = s2;
				pGuessRes->strSongName = s1;
			}

			pGuessRes->nResultType = 1;
		}
	}

	if(!pThread->m_isForNcmID) //ԭ�ȣ����͸����������
		::SendMessage( pThread->m_hMainWnd, MSG_USER_SEARCH_WITH_GUESS_RESULT, (WPARAM)pGuessRes,0); 
	else //��չ�����͸�����ID��
		::SendMessage( pThread->m_hMainWnd, MSG_USER_SEARCH_NCM_WITH_GUESS_RESULT, (WPARAM)pGuessRes,0); 
	//MSG_USER_SEARCH_WITH_GUESS_RESULT

	pThread->m_bIsGuessing = false;

	return 0;
}
