#pragma once
#include "stdafx.h"
#include "core/shostwnd.h"
#include "core/smsgloop.h"
#include "core/SHostDialog.h"
#include "entity\CheckIntegrityThread.h"

class DlgCheckIntegrity : public SHostDialog
{
	SOUI_CLASS_NAME(DlgCheckIntegrity, L"dlgparamtable")
public:
	DlgCheckIntegrity(LPCTSTR pszXmlName) :SHostDialog(pszXmlName){};

	void OnClose();
	~DlgCheckIntegrity(void){}

	//Ϊ���������߳�ͬ�����ڻ�ô��ھ��������������̲߳�������Ԥ�ȴ����¼�Handle��
	void SetEventHandleForWndHandle(HANDLE eventHandle, HWND* phWnd, CCheckIntegrityThread* pCheckThread);

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	
	//����UI��ʾ
	// wParam �ǰٷֱ�����lParam ����ʾ���ַ�����ַ
	BOOL OnUpdateCheckProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//�ر�UI��ʾ�߳�
	BOOL OnCloseProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	

protected:
	void OnCancel();//Ч���ǹرմ���

	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(R.id.btn_cancel_intigrity_check, OnCancel)
	EVENT_MAP_END()


	BEGIN_MSG_MAP_EX(DlgCreateHole)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnCancel)
		
		MESSAGE_HANDLER(MSG_USER_UPDATE_CHECK_PROGRESS, OnUpdateCheckProgress)
		MESSAGE_HANDLER(MSG_USER_CLOSE_CHECK_PROGRESS, OnCloseProgress)

		CHAIN_MSG_MAP(SHostDialog)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	SStatic*	m_txtProgressTip;
	SProgress*	m_progressChecking;

	//�����������߳�ͬ��
	HANDLE		m_EventWndInitDone;		//ȡ�ô��ھ������Ҫ���õ��¼�
	HWND*		m_pHwnd;				//��Ҫ�����Ĵ��ھ��ָ��

	CCheckIntegrityThread*	m_pCheckThread;//��������Ե��߳���
};
