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
* Describe    ����һ���̣߳����ڼ�����
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	// ���ü����µļ��Ϊ10����
	AutoUpdateThread(int nDelay = 1000 * 60 * 10):m_nDelay(nDelay),m_handleThread(NULL),m_bLoop(true),m_bKeepUpdate(false){}

	//��ʼ�߳�
	bool Start();

	//ֹͣ�߳�
	void Stop();
	
	//�����Ƿ����������
	void SetBKeepUpdate(BOOL bValue)
	{
		m_bKeepUpdate = (bValue ? true : false);
		if(m_bKeepUpdate)
			//���� m_EventStopWaiting Ϊ���źţ��Խ��� ThreadProc �е�ѭ���ĵȴ�
			SetEvent(m_EventStopWaiting);
	}
	
	//�Ƚ�2���ַ����汾�ŵĴ�С��
	static int VersionCompare(const SStringW v1, const SStringW v2);
private:
	
	//�߳�ִ�е�ַ
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//�Զ�����ִ�к���
	bool AutoUpdate(const SStringW& strVersionNew);

	//����Ƿ��и���
	bool IfUpdateAvailable(SStringW& versionNew);


private:

	HANDLE		m_handleThread;			/* ��ǰ�߳̾�� */
	HANDLE		m_EventStopWaiting;		/* ֹͣ�ȴ�ʱ�� */
	bool		m_bLoop;				/* �߳�ѭ����� */
	bool		m_bKeepUpdate;			/* ���ָ��±�־ */
	int			m_nDelay;				/* �����µļ������λms */
};

