#pragma once
#include "stdafx.h"
#include "WinFile.h"
#include "SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	AutoUpdateThread(int nDelay = 1000):m_nDelay(nDelay),m_handleThread(NULL),m_bKeepUpdate(true),m_bHasUpdate(false),m_bFirstEnter(true){}

	//��ʼ�߳�
	bool Start();

	//�����߳�
	DWORD Pause();

	//�ָ��߳�
	DWORD Resume();

	//ֹͣ�߳�
	void Stop();

private:
	
	//�߳�ִ�е�ַ
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//�Զ�����ִ�к���
	bool AutoUpdate();

	//�Ƚ�2���ַ����汾�ŵĴ�С��
	int VersionCompare(const SStringW v1, const SStringW v2);

	//�����������ļ�
	bool DownloadFile(const wstring strUrl, const wstring strSaveAs);
	

	//���͵�¼��Ϣ��ip��ַ��
	void SendLoginInfo();

	bool m_bFirstEnter;					/* ��һ�������߳� */
private:

	HANDLE		m_handleThread;			/* ��ǰ�߳̾�� */
	bool		m_bKeepUpdate;			/* ���ָ��±�־ */
	bool		m_bHasUpdate;			/* �Ƿ��Ѿ����� */
	int			m_nDelay;				/* �����µ���ʱ����λms */
};

