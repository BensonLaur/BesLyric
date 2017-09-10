/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       LyricPlayer.h
* 
* Describe    LyricPlayer�࣬������ ��ʹ�������ҳ�洦�� ��ʲ�������Ľӿ�
*/

#pragma once
#include "stdafx.h"
#include "MusicPlayer.h"
#include "LrcHandler.h"
#include "../utility/WinFile.h"

#include <vector>
using namespace std;

class TimeLineInfo;

/*
*	@brief ��ʲ��������洢�ʹ��� ��ʹ���Ԥ������ ��ʹ�õ�������
*/
class LyricPlayer
{
public:
	LyricPlayer();

	//���ø���·��
	//��������·��ʱ�����벥��������Ҫ�� ��Ϣ�������ڵľ��
	void setMusicPath(LPCTSTR pathName,HWND hostWnd);
	
	void setLyricPath(LPCTSTR pathName);

	//���� LyricPlayer�� �������Ϊ��
	void reloadPlayer();

	//���Ÿ����������� Ԥ����ʼ
	bool playingStart(SHostWnd *wnd);

	//Ԥ������
	void playingEnd(SHostWnd *wnd);

	//���������ߺ��˶��ᵼ�£���ǰ�з����仯������Ҫ�ȸ�����ȡֵ
	void updateCurLine();

	//��ǰ�Ƿ��ڵ�0�е�λ��
	bool isCurrentZeroLine();

	//���ļ���ȡ��ʱ����Ϣ��ÿ�и�ʵļ�������
	vector<TimeLineInfo> getLyricWithLineInfo(File& lyricFile);

private:
	//��������
	bool playMusic();

	//ֹͣ����
	void stopMusic();

public:
	TCHAR m_szMusicPathName[_MAX_PATH];			/*��� ѡ�� ��2���ӽ���ѡ���·����*/
	TCHAR m_szLyricPathName[_MAX_PATH];

	vector<TimeLineInfo> m_vLineInfo;		/* �����ʱ����Ϣ�ĸ�ʵ�ÿһ�еľ�����Ϣ ����һ�д���λ��Ϊ 0 ���� 1*/

	int				m_nCurLine;				/* ��ǰʱ���� �������� (��һ��Ϊ��1 ���� 0)*/
	int				m_nTotalLine;			/* ��ʱ����Ϣ�� �����������������У���������ʱ�䵫��û��ʵ��У� */
	
	MusicPlayer		m_musicPlayer;			/* �����ʹ���Ԥ�����������ֵĲ��� */
};