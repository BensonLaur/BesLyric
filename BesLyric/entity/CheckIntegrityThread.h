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
* @file       CheckIntegrity.h
* 
* Describe    ����һ���̣߳����ڼ����������ԣ��Զ�������Ҫ�����
*/

#pragma once
#include "stdafx.h"
#include "UpdateHelper.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  CCheckIntegrityThread : public Singleton<CCheckIntegrityThread>
{
public:
	// �������������
	CCheckIntegrityThread():m_handleThreadCheking(NULL),m_handleThreadUI(NULL),m_hCheckWnd(NULL),
		m_EventWndInitDone(NULL),m_EventUpdateDownloadDone(NULL), m_bIsChecking(false),m_bIsProcUIClose(true),m_nPercentage(0)
	{
	}

	//��ʼ�߳�
	bool Start(bool bShowPassTip);

	//�����߳�
	void Stop();
	
private:

	//��⣨�����أ� �߳�ִ�е�ַ
	static DWORD WINAPI ProcChecking(LPVOID pParam);

	//UI �߳�ִ�е�ַ
	static DWORD WINAPI ProcUI(LPVOID pParam);

	bool PreCheckWhetherNeedToShowCheckDialog();  //Ԥ�ȼ���Ƿ���Ҫ��ʾ���dialog

	//������е��ļ��Ƿ�Ϊ����
	bool CheckUpdateFile();
	
	bool DownloadUpdateFileAndMark();	//���ظ����ļ�Ȼ�����Ƿ���Ҫ����

	
	//���ffmpeg.exe �Ƿ���ڣ�������������
	bool CheckFFmpeg();

	//����UI����
	void UpdateProgressUI(int nPercentage, const WCHAR* pszTip);
	
	void RestoreProcess();
	void CloseProgressUI();

private:
	HWND		m_hCheckWnd;			/* ��ʾ�����ȴ��ڵľ�� */

	HANDLE		m_handleThreadCheking;	/* ��ǰ�߳̾�� */
	HANDLE		m_handleThreadUI;		/* UI�߳̾�� */
	
	HANDLE		m_EventWndInitDone;				/* ֹͣ�ȴ����ڳ�ʼ����� �¼�*/
	HANDLE		m_EventUpdateDownloadDone;		/* �����ļ�������� �¼�*/

	bool		m_bShowPassTip;			/* ��ʾ�ڼ��󣬽��Ϊ����ʱ��������ʾ */
	bool		m_bIsChecking;			/* ��ʾ�Ƿ����ڼ�� */

	bool		m_bIsProcUIClose;		/* Processing UI �Ƿ��Ѿ��ر� */

	int			m_nPercentage;			/* ���һ�μ�¼�Ľ��Ȱٷֱ� */
	WCHAR		m_szTip[MAX_BUFFER_SIZE];/* ���һ�μ�¼�Ľ�����Ϣ */		

	UpdateHelper updateHelper;
};

