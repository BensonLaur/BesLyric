#pragma once
#include "stdafx.h"
#include "WinFile.h"
#include "SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	// ���ü����µļ��Ϊ10����
	AutoUpdateThread(int nDelay = 1000 * 60 * 10):m_nDelay(nDelay),m_handleThread(NULL),m_bLoop(true),m_bKeepUpdate(false),m_bFirstEnter(true){}

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
private:
	
	//�߳�ִ�е�ַ
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//�Զ�����ִ�к���
	bool AutoUpdate();

	//����Ƿ��и���
	bool IfUpdateAvailable();

	//�Ƚ�2���ַ����汾�ŵĴ�С��
	int VersionCompare(const SStringW v1, const SStringW v2);

	//�����������ļ�
	bool DownloadFile(const wstring strUrl, const wstring strSaveAs);
	

	//���͵�¼��Ϣ��ip��ַ��
	void SendLoginInfo();

	bool m_bFirstEnter;					/* ��һ�������߳� */
private:

	HANDLE		m_handleThread;			/* ��ǰ�߳̾�� */
	HANDLE		m_EventStopWaiting;		/* ֹͣ�ȴ�ʱ�� */
	bool		m_bLoop;				/* �߳�ѭ����� */
	bool		m_bKeepUpdate;			/* ���ָ��±�־ */
	int			m_nDelay;				/* �����µļ������λms */
};

