/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// LyricPlyer.cpp :  ʵ��  LyricPlayer�� �Ľӿ�	
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LyricPlayer.h"
#include "../utility/FileHelper.h"
#include "../utility/WinFile.h"
#include <algorithm>
#include <mmsystem.h> 
#pragma comment (lib, "winmm.lib")

//���캯��
LyricPlayer::LyricPlayer():
		 m_nCurLine(0),	
		 m_nTotalLine(0)
{
	m_szMusicPathName[0]= _T('\0');		
	m_szLyricPathName[0]= _T('\0');
}

//���ø���·��
void LyricPlayer::setMusicPath(LPCTSTR pathName, HWND hostWnd)
{
	_tcscpy(m_szMusicPathName,pathName);
	m_musicPlayer.init(m_szMusicPathName,hostWnd);
}
	
void LyricPlayer::setLyricPath(LPCTSTR pathName)
{
	_tcscpy(m_szLyricPathName,pathName);
}

//���� LyricPlayer�� �������Ϊ��
void LyricPlayer::reloadPlayer()
{
	m_vLineInfo.clear();
}

//���Ÿ����������� Ԥ����ʼ
bool LyricPlayer::playingStart(SHostWnd *wnd)
{
	//���»�����������¼������
	m_nTotalLine = this->m_vLineInfo.size();
	m_nCurLine = 1;	//��ֵ��Ϊ1����ʱ��ѭ������Ҫ

	//�������Ϊ1�����Timer������ҳ�����ʾ
	wnd->SetTimer(102,1);

	return playMusic();
}

//Ԥ������
void LyricPlayer::playingEnd(SHostWnd *wnd)
{
	//�ر�Timer
	wnd->KillTimer(102);
	stopMusic();
}

//��������
bool LyricPlayer::playMusic()
{
	if(m_musicPlayer.openTest() == false)
	{
		return false;
	}

	m_musicPlayer.openStart();
	
	return true;
}

//ֹͣ����
void LyricPlayer::stopMusic()
{
	m_musicPlayer.closeStop();
}

//���������ߺ��˶��ᵼ�£���ǰ�з����仯������Ҫ�ȸ�����ȡֵ
void LyricPlayer::updateCurLine()
{
	//���ݵ�ǰ�������ŵ�λ�ã��жϵ�ǰ����һ��
	int pos = this->m_musicPlayer.getPosition();
	auto i= m_vLineInfo.begin();
	int j;
	for(j=1; i != m_vLineInfo.end(); i++,j++)
		if( i->m_nmSesonds > pos)//���ʱ�� ���ڵ�ǰ����λ��ʱ
		{
			//ȡǰһ��λ�ã���Ϊ m_nCurLine ��ֵ
			this->m_nCurLine = j-1; 
			break;
		}
}

//��ǰ�Ƿ��ڵ�0�е�λ��
bool LyricPlayer::isCurrentZeroLine()
{
	bool isZero;

	//���ݵ�ǰ�������ŵ�λ�ã��жϵ�ǰ����һ��
	int pos = this->m_musicPlayer.getPosition();
	if( pos < m_vLineInfo.begin()->m_nmSesonds )
		isZero = true;
	else
		isZero = false;
	
	return isZero;
}

//���ļ���ȡ��ʱ����Ϣ��ÿ�и�ʵļ�������
vector<TimeLineInfo> LyricPlayer::getLyricWithLineInfo(File& lyricFile)
{
	vector<TimeLineInfo> lines;

	
	vector<SStringW> allLines;
	if(true == FileOperator::ReadAllLinesW(lyricFile, &allLines))
	{
		LrcProcessor lrcProcessor( allLines);

		if(lrcProcessor.IsLrcFileValid())
		{
			lines = lrcProcessor.GetNeteaseLrc();

			//�������ԭ����lrc�ļ�����Ϊ�û����ܿ�������Ԥ��LRC����ֱ�ӽ������Ʋ�֧�ֵĸ���ϴ���ȥ
			lrcProcessor.GenerateNeteaseLrcFile(SStringW(lyricFile.m_lpszPathFile));
		}
	}

	//vector<SStringW> allLines;
	//if(true == FileOperator::ReadAllLinesW(lyricFile, &allLines))
	//{
	//	for(auto iter = allLines.begin();iter!=allLines.end();  iter++)
	//	{
	//		TimeLineInfo newInfo(iter->GetBuffer(1));
	//		lines.push_back(newInfo);
	//	}
	//}

	return lines;
}



//ʹ�ö�ȡ�ĵ��ļ��������г�ʼ��Lrc������
LrcProcessor::LrcProcessor(vector<SStringW> vecLines, bool bDealOffset)
{
	m_bDealOffset = bDealOffset;
	m_strTitle = L"";
	m_strArtist = L"";
	m_strAlbum = L"";
	m_strEditor = L"";
	m_nOffset = 0;

	m_bIsLrcFileValid = true;

	//��������������
	for(auto iter = vecLines.begin(); iter != vecLines.end(); iter++)
	{
		if( false == DealOneLine( *iter ) )
		{
			m_bIsLrcFileValid = false;	
			return;
		}
	}

	//�����и�ʽ�������
	sort(m_vecNeteaseLrc.begin(), m_vecNeteaseLrc.end());

	//ȥ���ظ��Ŀ��У��������ֶ������ʱ��������һ�����е�ʱ��
	bool isLastLineEmpty = false;
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); )
	{
		if(iter->m_strLine.GetLength() == 0)
		{
			if(isLastLineEmpty)
			{
				iter = m_vecNeteaseLrc.erase(iter);
			}
			else
			{
				iter++;
				isLastLineEmpty = true;
			}
		}
		else
		{
			iter++;
			isLastLineEmpty = false;
		}
	}

	//�������ʱ�����ƫ��
	if(m_bDealOffset)
	{
		for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
		{
			iter->m_nmSesonds = iter->m_nmSesonds + m_nOffset < 0 ? 0 : iter->m_nmSesonds + m_nOffset;
		}
	}
}

//��ǰ����ļ��Ƿ���Ч
bool LrcProcessor::IsLrcFileValid()
{
	return m_bIsLrcFileValid;
}

//�������������֧�ֵĸ�ʽ
vector<TimeLineInfo> LrcProcessor::GetNeteaseLrc()
{
	return m_vecNeteaseLrc;
}

//����Lrc�ļ�
bool LrcProcessor::GenerateNeteaseLrcFile(SStringW strFilePath)
{
	vector<SStringW> vecLines;

	WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
	{
		int ms = iter->m_nmSesonds;

		szTimeBuf[0] = L'\0';
		int minutes = ms/60000;
		ms = ms%60000;
		double seconds = ms*1.0/1000;
		_stprintf(szTimeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);

		SStringW newLine = szTimeBuf;
		newLine.Append( iter->m_strLine );
		newLine.Append(L"\n");

		vecLines.push_back(newLine);
	}

	return FileOperator::WriteToUtf8File( strFilePath.GetBuffer(1), vecLines);
}

//����1��
bool LrcProcessor::DealOneLine(SStringW strLine)
{
	int nPos1 = -1;
	int nPos2 = -1;
	if( -1 != strLine.Find(L"[ti:"))//����
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strTitle = strLine.Left(nPos2);
		m_strTitle = m_strTitle.Right(m_strTitle.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[ar:"))//������
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strArtist = strLine.Left(nPos2);
		m_strArtist = m_strArtist.Right(m_strArtist.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[al:"))//ר��
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strAlbum = strLine.Left(nPos2);
		m_strAlbum = m_strAlbum.Right(m_strAlbum.GetLength()-1 - nPos1 );
	}
	else if( -1 != strLine.Find(L"[by:"))//���������
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strEditor = strLine.Left(nPos2);
		m_strEditor = m_strEditor.Right(m_strEditor.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[offset:"))
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		SStringW m_strOffset = strLine.Left(nPos2);
		m_strOffset = m_strOffset.Right(m_strOffset.GetLength()-1 - nPos1);
		m_nOffset = _wtoi( m_strOffset.GetBuffer(1));
	}
	else //���������Ϊ�Ǳ��ʱ�����
	{
		wstring sLine = strLine.GetBuffer(1);
		wstring sTimeLabel = L"";
		wstring sLyric = L"";
		int nPosColon = -1;
		sLyric = sLine.substr( sLine.find_last_of(L']') + 1);
		do{
			nPos1 = sLine.find_first_of(L'[');
			nPosColon =  sLine.find_first_of(L':');
			nPos2 =  sLine.find_first_of(L']');
			if(nPos1 ==0 && nPos1 < nPosColon && nPosColon < nPos2) // strLineͷ����ʱ���ǩ
			{
				sTimeLabel = sLine.substr(nPos1, nPos2+1);

				TimeLineInfo timeInfoLine(SStringW( (sTimeLabel + sLyric).c_str()));

				m_vecNeteaseLrc.push_back(timeInfoLine); //���һ�е������Ƹ��

				sLine = sLine.substr(nPos2+1); //ȥ��ǰ���ǩ
			}
			else
				break;
		}
		while( true);
	}

	return true;
}

//�����ʱ������Ƚ�
bool LrcProcessor::CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2)
{
	return L1.m_nmSesonds < L2.m_nmSesonds;
}

//bool m_bDealOffset;						/* ��ʾ�Ƿ���LRC�ļ��е�ʱ��ƫ�� */
//SStringW	m_strTitle;					/* �������� */
//SStringW	m_strArtist;				/* ������ */
//SStringW	m_strAlbum;					/* ר�� */
//SStringW	m_strEditor;				/* �༭���� */
//int			m_nOffset;					/* ʱ��ƫ������Ϊ������ʾ������ǰ */

//vector<TimeLineInfo> m_vecNeteaseLrc;	/* �������������Ƶĸ����Ϣ */

//bool	m_bIsLrcFileValid;				/* ��ʾ����ļ��Ƿ���Ч */
