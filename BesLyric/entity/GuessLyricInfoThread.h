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
* @file       GuessLyricInfoThread.h
* 
* Describe    ����һ���̣߳����ڸ��ݸ����ļ��Լ����Ƶ���Ϣ���²��ʵĸ������͸�����Ϣ
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  CGuessLyricInfoThread : public Singleton<CGuessLyricInfoThread>
{
public:
	// �������������
	CGuessLyricInfoThread():m_handleThreadGuess(NULL),m_bIsGuessing(false){}

	//��ʼ�߳�
	bool Start(HWND hMainWnd, SStringW& strMusicPath);

private:
	//��mp3�ļ��л�ȡ�²���
	bool GetGuessResultFromMp3(SongInfoGuessResult* pGuessRes );

	//�������ƽӿڷ���ǰ���ַ����Ƿ�Ϊ����
	bool GuessWhetherFirstArtistFromNetease(SStringW& strFirst, SStringW& strSecond);

	// �߳�ִ�е�ַ
	static DWORD WINAPI ProcGuessLyricInfo(LPVOID pParam);

private:

	HANDLE		m_handleThreadGuess;	/* ��ǰ�߳̾�� */	
	HWND		m_hMainWnd;				/* �����ھ�� */

	bool m_bIsGuessing;				//���ڲ²��������

	SStringW	m_strMusicPath;
	SStringW	m_strMusicName;			
	SStringW	m_strMusicArtist;
};

