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
		for(auto iter = allLines.begin();iter!=allLines.end();  iter++)
		{
			TimeLineInfo newInfo(iter->GetBuffer(1));
			lines.push_back(newInfo);
		}
	}

	return lines;
}