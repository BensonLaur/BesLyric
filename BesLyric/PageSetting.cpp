#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
#include "FileHelper.h"
#include "WinFile.h"
#include "lib\tinyxml2.h"
#include <sstream>
#include "AutoUpdateThread.h"
using namespace std;
using namespace tinyxml2;

static const string XML_TRUE = "1";
static const string XML_FALSE = "0";
static const string XML_SHIFT_TIME = "shiftTime";							//ʱ����ƫ����
static const string XML_DEFAULF_MUSIC_PATH = "defaultMusicPath";			//Ĭ������·��
static const string XML_DEFAULF_LYRIC_PATH = "defaultLyricPath";			//Ĭ�ϸ��·��
static const string XML_DEFAULF_OUTPUT_PATH = "defaultOutputPath";			//Ĭ�����·��
static const string XML_AUTO_UPDATE = "autoUpdate";							//�Ƿ��Զ�����


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
	
	SASSERT( btn_modify_shift_time != NULL);
	SASSERT( edit_time_shift != NULL);
	SASSERT( text_default_music_path != NULL);
	SASSERT( text_default_lyric_path != NULL);
	SASSERT( text_default_output_path != NULL);
	SASSERT( check_auto_update != NULL);

	
	//��ʼ����������
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
		
	//��ʼ�����޸İ�ť
	btn_modify_shift_time->EnableWindow(FALSE, FALSE);
	
	text_default_music_path->SetWindowTextW(S_CA2W(SStringA(m_default_music_path.c_str())));
	text_default_lyric_path->SetWindowTextW(S_CA2W(SStringA(m_default_lyric_path.c_str())));
	text_default_output_path->SetWindowTextW(S_CA2W(SStringA(m_default_output_path.c_str())));

	//��ʼ������check �� ��Ч������
	check_auto_update->SetCheck(m_check_auto_update);
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}


//��������
void CSettingPage::SaveSetting()
{
	string a;
	stringstream ss(a);  //����һ����
	ss << m_nTimeShift;  //��ֵ����������

	string xmlStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += "<setting>\n";
	xmlStr += "\t<"+XML_SHIFT_TIME+" value=\""+	ss.str() +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_MUSIC_PATH+" value=\""+	m_default_music_path +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_LYRIC_PATH+" value=\""+	m_default_lyric_path +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_OUTPUT_PATH+" value=\""+ m_default_output_path +"\"/>\n";
	xmlStr += "\t<"+XML_AUTO_UPDATE+		" value=\""+ (m_check_auto_update? XML_TRUE : XML_FALSE) +"\"/>\n";
	xmlStr += "</setting>\n";

	FileOperator::SaveToFile(FileHelper::GetCurrentDirectoryStr() + SETTING_FILE_NAME, xmlStr);
}

//��������
void CSettingPage::LoadSetting()
{
	//�Ƚ���Ĭ�ϳ�ʼ������ʼ��Ĭ������
	m_nTimeShift = 300;			
	m_default_music_path = "";
	m_default_lyric_path = "";
	m_default_output_path = "";
	m_check_auto_update = TRUE;	

	//���ļ���������
	string strSettingPath = FileHelper::GetCurrentDirectoryStr() + SETTING_FILE_NAME;
	if(FileHelper::CheckFileExist(strSettingPath))
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
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		string strName = ("");
		while(ele)
		{
			strName = ele->Name();
			string value = ele->Attribute("value");
			if (XML_SHIFT_TIME == strName)
			{
				//ʱ����ƫ����
				m_nTimeShift = atoi(value.c_str());
			}
			else if (XML_DEFAULF_MUSIC_PATH == strName)
			{
				//Ĭ������·��
				m_default_music_path = value;
			}
			else if ( XML_DEFAULF_LYRIC_PATH == strName)
			{
				//Ĭ�ϸ��·��
				m_default_lyric_path = value;
			}
			else if (XML_DEFAULF_OUTPUT_PATH == strName)
			{
				//Ĭ�����·��
				m_default_output_path = value;
			}
			else if (XML_AUTO_UPDATE == strName)
			{
				//�Ƿ��Զ�����
				m_check_auto_update = (atoi(value.c_str())? TRUE: FALSE);
			}

			//��һ�ֵܽ��
			ele = ele->NextSiblingElement();
		}
	}
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
}

//ѡ��Ĭ������·��
void CSettingPage::OnBtnSelectDefaultMusicPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� �������ļ��� Ĭ��·��"), TRUE);
	if(bRet == TRUE)
	{
		m_default_music_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_music_path)->SetWindowTextW(S_CA2W(SStringA(m_default_music_path.c_str())));
	}
}

//ѡ��Ĭ�ϸ��·��
void CSettingPage::OnBtnSelectDefaultLyricPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ������ļ��� Ĭ��·��"), TRUE);
	if(bRet == TRUE)
	{
		m_default_lyric_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_lyric_path)->SetWindowTextW(S_CA2W(SStringA(m_default_lyric_path.c_str())));
	}
	
}
	
//ѡ��Ĭ��LRC������·��
void CSettingPage::OnBtnSelectDefaultOutputPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("ѡ�� ��LRC����ļ��� Ĭ�����·��"), TRUE);
	if(bRet == TRUE)
	{
		m_default_output_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_output_path)->SetWindowTextW(S_CA2W(SStringA(m_default_output_path.c_str())));
	}
}
			
//�Զ�����check �ı�״̬ʱ
void CSettingPage::OnCheckAutoUpdateChanged()
{
	m_check_auto_update = check_auto_update->IsChecked();
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}