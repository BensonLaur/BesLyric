#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "lib\xml\tinyxml2.h"
#include <sstream>
#include "entity\AutoUpdateThread.h"
#include "entity\CheckIntegrityThread.h"
#include "entity\SendLoginThread.h"
using namespace std;
using namespace tinyxml2;

static const wstring XML_TRUE = L"1";
static const wstring XML_FALSE = L"0";
static const wstring XML_SHIFT_TIME = L"shiftTime";							//ʱ����ƫ����
static const wstring XML_DEFAULF_MUSIC_PATH = L"defaultMusicPath";			//Ĭ������·��
static const wstring XML_DEFAULF_LYRIC_PATH = L"defaultLyricPath";			//Ĭ�ϸ��·��
static const wstring XML_DEFAULF_OUTPUT_PATH = L"defaultOutputPath";		//Ĭ�����·��
static const wstring XML_AUTO_UPDATE = L"autoUpdate";						//�Ƿ��Զ�����
static const wstring XML_ANONYMITY = L"anonymity";							//�Ƿ�����


void CSettingPage::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ȡ�ؼ�ָ��
	btn_modify_shift_time = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_modify_setting_shift_time);
	edit_time_shift = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_time_shift);

	text_default_music_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_music_path);	
	text_default_lyric_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_lyric_path);	
	text_default_output_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_output_path);		
	check_auto_update = m_pMainWnd->FindChildByID2<SCheckBox>(R.id.check_auto_update);	
	check_anonymity = m_pMainWnd->FindChildByID2<SCheckBox>(R.id.check_anonymity);

	SASSERT( btn_modify_shift_time != NULL);
	SASSERT( edit_time_shift != NULL);
	SASSERT( text_default_music_path != NULL);
	SASSERT( text_default_lyric_path != NULL);
	SASSERT( text_default_output_path != NULL);
	SASSERT( check_auto_update != NULL);
	SASSERT( check_anonymity != NULL);

	
	//��ʼ����������
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
		
	//��ʼ�����޸İ�ť
	btn_modify_shift_time->EnableWindow(FALSE, FALSE);
	
	text_default_music_path->SetWindowTextW(SStringW(m_default_music_path.c_str()));
	text_default_lyric_path->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));
	text_default_output_path->SetWindowTextW(SStringW(m_default_output_path.c_str()));

	//��ʼ������check �� ��Ч������
	check_auto_update->SetCheck(m_check_auto_update);
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);

	check_anonymity->SetCheck(m_check_anonymity);
	SendLoginThread::getSingleton().Start(m_check_anonymity);
}


//��������
void CSettingPage::SaveSetting()
{
	wstring a;
	wstringstream ss(a);  //����һ����
	ss << m_nTimeShift;  //��ֵ����������

	wstring xmlStr = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += L"<setting>\n";
	xmlStr += L"\t<"+XML_SHIFT_TIME+L" value=\""+	ss.str() +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_MUSIC_PATH+L" value=\""+	m_default_music_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_LYRIC_PATH+L" value=\""+	m_default_lyric_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_OUTPUT_PATH+L" value=\""+ m_default_output_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_AUTO_UPDATE+		L" value=\""+ (m_check_auto_update? XML_TRUE : XML_FALSE) +L"\"/>\n";
	xmlStr += L"\t<"+XML_ANONYMITY+		L" value=\""+ (m_check_anonymity? XML_TRUE : XML_FALSE) +L"\"/>\n";
	
	xmlStr += L"</setting>\n";
	
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" +SETTING_FILE_NAME;
	FileOperator::WriteToUtf8File( wstrPath,xmlStr);
}

//��������
void CSettingPage::LoadSetting()
{
	//�Ƚ���Ĭ�ϳ�ʼ������ʼ��Ĭ������
	m_nTimeShift = 300;			
	m_default_music_path = L"";
	m_default_lyric_path = L"";
	m_default_output_path = L"";
	m_check_auto_update = TRUE;	
	m_check_anonymity = FALSE;	

	//���ļ���������
	wstring wstrSettingPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + SETTING_FILE_NAME;
	string strSettingPath = S_CW2A(SStringW(wstrSettingPath.c_str()));
	if(FileHelper::CheckFileExist(wstrSettingPath))
	{
		//��ȡXML�ļ�
		tinyxml2::XMLDocument doc;
		doc.LoadFile(strSettingPath.c_str());
		if(doc.Error())
		{
			return;
		}

		//��
		XMLElement *pRoot = doc.RootElement();
		if(pRoot)
		{
			XMLElement* ele = pRoot->FirstChildElement();
			while(ele)
			{
				const char* szName = ele->Name();
				const char* szValue = ele->Attribute("value");
			
				wstring wStrName = S_CA2W(SStringA(szName));
				WCHAR wszValue[MAX_BUFFER_SIZE] = {0};
				if(szValue)
				{
					//�����ļ���������utf-8���棬��Ҫת��
					::MultiByteToWideChar(CP_UTF8,0,szValue, MAX_BUFFER_SIZE/2, wszValue, MAX_BUFFER_SIZE);
				}

				if (XML_SHIFT_TIME == wStrName)
				{
					//ʱ����ƫ����
					m_nTimeShift = _wtoi(wszValue);
				}
				else if (XML_DEFAULF_MUSIC_PATH == wStrName)
				{
					//Ĭ������·��
					m_default_music_path = wszValue;
				}
				else if ( XML_DEFAULF_LYRIC_PATH == wStrName)
				{
					//Ĭ�ϸ��·��
					m_default_lyric_path = wszValue;
				}
				else if (XML_DEFAULF_OUTPUT_PATH == wStrName)
				{
					//Ĭ�����·��
					m_default_output_path = wszValue;
				}
				else if (XML_AUTO_UPDATE == wStrName)
				{
					//�Ƿ��Զ�����
					m_check_auto_update = (_wtoi(wszValue)? TRUE: FALSE);
				}
				else if(XML_ANONYMITY == wStrName)
				{
					//�Ƿ�����
					m_check_anonymity = (_wtoi(wszValue)? TRUE: FALSE);
				}

				//��һ�ֵܽ��
				ele = ele->NextSiblingElement();
			}
		}
		else
			return;
	}
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
}

//ѡ��Ĭ������·��
void CSettingPage::OnBtnSelectDefaultMusicPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� �������ļ��� Ĭ��·��"), TRUE, m_default_music_path.c_str());
	if(bRet == TRUE)
	{
		m_default_music_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_music_path)->SetWindowTextW(SStringW(m_default_music_path.c_str()));
	}
}

//ѡ��Ĭ�ϸ��·��
void CSettingPage::OnBtnSelectDefaultLyricPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ������ļ��� Ĭ��·��"), TRUE, m_default_lyric_path.c_str());
	if(bRet == TRUE)
	{
		m_default_lyric_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//���Ӧ����ʾtext
		
		m_pMainWnd->FindChildByID(R.id.edit_origin_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//����ѡ��ҳ���Դ��ʱ���·��
		
	}
	
}
	
//ѡ��Ĭ��LRC������·��
void CSettingPage::OnBtnSelectDefaultOutputPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��LRC����ļ��� Ĭ�����·��"), TRUE, m_default_output_path.c_str());
	if(bRet == TRUE)
	{
		m_default_output_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_output_path)->SetWindowTextW(SStringW(m_default_output_path.c_str()));//���Ӧ����ʾtext
		
		m_pMainWnd->FindChildByID(R.id.edit_lrc_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//����ѡ��ҳ���Դ��ʱ���·��
	}
}
			
//�Զ�����check �ı�״̬ʱ
void CSettingPage::OnCheckAutoUpdateChanged()
{
	m_check_auto_update = check_auto_update->IsChecked();
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}
		
//������¼ѡ�� �ı�״̬ʱ
void CSettingPage::OnCheckAnonymityChanged()
{
	m_check_anonymity = check_anonymity->IsChecked();
}


//������������
void CSettingPage::OnBtnCheckIntegrity()
{
	CCheckIntegrityThread::getSingleton().Start(true);
}