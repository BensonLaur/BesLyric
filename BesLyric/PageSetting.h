/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  BensonLaur

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
* @file       PageSetting.h
* 
* Describe    CSettingPage�࣬��������ҳ���ҵ���߼�
*/

#pragma once
#include "stdafx.h"

class CSettingPage
{
public:
	CSettingPage(){LoadSetting();};
	~CSettingPage(){SaveSetting();};

	void Init(SHostWnd *pMainWnd);		//��ʼ������ҳ��
	void SaveSetting();					//��������
	void LoadSetting();					//��������
	void LoadShiftTime();				//���ļ���ȡ��ȡʱ�䣬��ʾ��edit_time_shift �༭����
	void SaveShiftTime();				//�����õ�ʱ�䱣�浽�ļ���
	
	void OnTimeShiftEditNotify(EventArgs *pEvt);	//ƫ��ʱ��༭��֪ͨ��Ӧ
	void OnBtnModifyShiftTime();					//�޸�ƫ��ʱ��
	void OnBtnSelectDefaultMusicPath();				//ѡ��Ĭ������·��
	void OnBtnSelectDefaultLyricPath();				//ѡ��Ĭ�ϸ��·��
	void OnBtnSelectDefaultOutputPath();				//ѡ��Ĭ��LRC������·��

	void OnCheckAutoUpdateChanged();				//�Զ�����check �ı�״̬ʱ

	//��Ϣ
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		EVENT_NAME_COMMAND(L"btn_modify_setting_shift_time", OnBtnModifyShiftTime)
		
		EVENT_NAME_COMMAND(L"btn_select_default_music_path", OnBtnSelectDefaultMusicPath)
		EVENT_NAME_COMMAND(L"btn_select_default_lyric_path", OnBtnSelectDefaultLyricPath)
		EVENT_NAME_COMMAND(L"btn_select_default_output_path", OnBtnSelectDefaultOutputPath)

		EVENT_NAME_COMMAND(R.name.check_auto_update, OnCheckAutoUpdateChanged)
		
        EVENT_NAME_HANDLER(R.name.edit_time_shift, EVT_RE_NOTIFY, OnTimeShiftEditNotify);
		//EVENT_NAME_HANDLER(R.name.edit_time_shift, EventRENotify::EventID, OnTimeShiftEditNotify)
	EVENT_MAP_BREAK()


public:
	int m_nTimeShift;				/* ʱ����ƫ��������λΪ���룬Ϊ����ʱ��ʾ����ʱ����ǰƫ�� */
	string m_default_music_path;	/* Ĭ������·�� */
	string m_default_lyric_path;	/* Ĭ�ϸ��·�� */
	string m_default_output_path;	/* Ĭ�����·�� */
	BOOL   m_check_auto_update;		/* �Ƿ��Զ����� */

private:
	SButton* btn_modify_shift_time;		/* �޸�ʱ����ƫ��ʱ�䰴ť */
	SEdit* edit_time_shift;				/* ƫ��ʱ��༭�� */
	
	SStatic* text_default_music_path;	/* Ĭ������·�� */
	SStatic* text_default_lyric_path;	/* Ĭ�ϸ��·�� */
	SStatic* text_default_output_path;	/* Ĭ�����·�� */

	SCheckBox* check_auto_update;		/* �Ƿ��Զ����� */

	SHostWnd *m_pMainWnd;		/* ������ָ�� */
	
};