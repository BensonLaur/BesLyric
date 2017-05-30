#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
using namespace std;

void CSettingPage::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ȡ�ؼ�ָ��
	btn_modify_shift_time = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_modify_setting_shift_time);
	SASSERT( btn_modify_shift_time != NULL);
	edit_time_shift = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_time_shift);
	SASSERT( edit_time_shift != NULL);
		
	//��ʼ�����޸İ�ť
	if(btn_modify_shift_time)
		btn_modify_shift_time->EnableWindow(FALSE, FALSE);

	//���ļ���ȡ��ȡʱ�䣬��ʾ��edit_time_shift �༭����
	LoadShiftTime();
}

//���ļ���ȡ��ȡʱ�䣬��ʾ��edit_time_shift �༭����
void CSettingPage::LoadShiftTime()
{
	//���exe�ļ�Ŀ¼
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	string strPath(S_CW2A(SStringW(exeFullPath)).GetBuffer(1));
	strPath = strPath.substr(0, strPath.find_last_of("\\")+1);

	//�õ�����·��
	strPath += SETTING_FILE_NAME;

	fstream storageFile;
	storageFile.open(strPath, ios::in);
	if(storageFile)//�ɹ����ļ�
	{
		char msTime[100] = {'\0'};
		storageFile.getline(msTime, 100-1);
	
		m_nTimeShift = atoi(msTime);
	}
	else
	{
		m_nTimeShift = 0;
		//�����µ��ļ���д��ƫ��ʱ��
		storageFile.close();
		storageFile.open(strPath, ios::out);
		if(storageFile){
			storageFile << m_nTimeShift;
			storageFile.close();
		}
	}
	
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
}

//ƫ��ʱ��༭��֪ͨ��Ӧ
void CSettingPage::OnTimeShiftEditNotify(EventArgs *pEvt)
{
	EventRENotify* Evt = sobj_cast<EventRENotify>(pEvt);
	if(Evt)
	{
		if(Evt->iNotify==EN_CHANGE )
		{
			string strTime = SStringA(S_CW2A(edit_time_shift->GetWindowTextW())).GetBuffer(1);
			int nTime = atoi(strTime.c_str());
			if(nTime != m_nTimeShift) //��ͬ����ǰ�����ֵ
			{
				//�������޸İ�ť
				if(btn_modify_shift_time)
					btn_modify_shift_time->EnableWindow(TRUE, TRUE);
			}
			else
			{
				//�����޸İ�ť
				if(btn_modify_shift_time)
					btn_modify_shift_time->EnableWindow(FALSE, TRUE);
			}
		}

	}
}

//�޸�ƫ��ʱ��
void CSettingPage::OnBtnModifyShiftTime()
{
	string strTime = SStringA(S_CW2A(edit_time_shift->GetWindowTextW())).GetBuffer(1);
	int nTime = atoi(strTime.c_str());
	m_nTimeShift = nTime;
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));

	//���浽�����ļ���
	//���exe�ļ�Ŀ¼
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	string strPath(S_CW2A(SStringW(exeFullPath)).GetBuffer(1));
	strPath = strPath.substr(0, strPath.find_last_of("\\")+1);

	//�õ�����·��
	strPath += SETTING_FILE_NAME;
	
	fstream storageFile;
	storageFile.open(strPath, ios::out);
	if(storageFile){
			storageFile << m_nTimeShift;
			storageFile.close();
	}
}