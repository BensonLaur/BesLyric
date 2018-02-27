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
* @file       SearchIDThread.h
* 
* Describe    ����һ���̣߳���������ID
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  CSearchIDThread : public Singleton<CSearchIDThread>
{
public:
	CSearchIDThread():m_handleThreadSearch(NULL),m_bIsSearching(false){}

	//��ʼ�߳�
	bool Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist);

	bool IsSearching(){return m_bIsSearching;}
private:

	// �߳�ִ�е�ַ
	static DWORD WINAPI ProcSearch(LPVOID pParam);

private:

	HANDLE		m_handleThreadSearch;	/* ��ǰ�߳̾�� */	
	HWND		m_hMainWnd;				/* �����ھ�� */
	
	bool m_bIsSearching; //����Ƿ�����������

	SStringW	m_strMusicName;			
	SStringW	m_strMusicArtist;
};

