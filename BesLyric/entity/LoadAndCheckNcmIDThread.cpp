#include "stdafx.h"
#include "LoadAndCheckNcmIDThread.h"
#include "NcmIDManager.h"

//��ʼ�߳�
bool CLoadAndCheckNcmIDThread::Start()
{
	//�����߳�
	m_handleThread = ::CreateThread(NULL, 0, ProcLoadAndCheckNcmID, this, 0 ,NULL);

	return (m_handleThread != NULL);
}

//�����߳�
void CLoadAndCheckNcmIDThread::Stop(){}

//��⣨�����أ� �߳�ִ�е�ַ
DWORD WINAPI CLoadAndCheckNcmIDThread::ProcLoadAndCheckNcmID(LPVOID pParam)
{
	CNcmIDManager::GetInstance()->LoadAndCheckDataPairs();

	return 0;
}
