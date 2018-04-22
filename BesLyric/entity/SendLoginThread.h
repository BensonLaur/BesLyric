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
* @file       AutoUpdateThread.h
* 
* Describe    ����һ���̣߳����ڷ��͵�¼��Ϣ
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  SendLoginThread : public Singleton<SendLoginThread>
{
public:
	SendLoginThread():m_handleThread(NULL){}

	//��ʼ�߳�
	bool Start(BOOL bAnonymity);

private:
	
	//�߳�ִ�е�ַ
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//���͵�¼��Ϣ��ip��ַ��
	void SendLoginInfo(BOOL bAnonymity);

	//�������������IP�ַ���
	inline bool CatchIPStr(const wstring &line, OUT wstring& ip);
	

private:

	HANDLE		m_handleThread;			/* ��ǰ�߳̾�� */
	BOOL		m_bAnonymity;			/* �߳�ѭ����� */
};

