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
* @file       PageMaking.h
* 
* Describe    CPageMaking�࣬�������ҳ��
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"

class CMainDlg;							//Ƕ�׶��壬������
class CPageMaking
{
	friend class CMainDlg;	

public:
	CPageMaking();
	~CPageMaking(){};

	void Init(SHostWnd *pMainWnd);		//��ʼ������ҳ��
	CMainDlg* M();						//�����������ʵ������
	
	//����ҳ�水ť����Ӧ����

	void OnBtnSelectMusic1(LPCWSTR pFilePath = NULL);		//����·����ѡ��	
	void OnBtnSelectLyric1(LPCWSTR pFilePath = NULL);
	void OnBtnSelectOutput(LPCWSTR pDirPath = NULL);
	void OnBtnLoad1();				//������ʣ����� (��ʼ�� LyricMaker)
	void OnBtnRestart();			//��Ҫ����
	void OnBtnStartMaking();		//��ʼ����
	void OnBtnPreview();
	void OnBtnOpenOutput();			//�����ɵ��ļ�

	void OnBtnSoundOpen1();			//������
	void OnBtnSoundClose1();		//����
	
	void OnSliderPos1(EventArgs *pEvt);//��������slider λ�õı仯
	
protected:
	//��һ��ҳ��(�������)���ص������ذ�ť�����º��״̬
	void backToInit_1();						/* �ص�������ɺ��״̬���˺�����Ϊ�˸��ö�������Ϊһ��������ʹ�� */
	
private:
	//�жϵ�һ��ҳ��(�������) �� ����·���Ƿ�ѡ����� */
	bool isPathReady_1();
	/* ��õ�ǰδ��ʼ���ĵ�һ��·�� ��PATH_STATE_1 �ж�Ӧ���±�;����ʼ�����򷵻�-1 */
	int getPathNotReady_1();

	//�߳�ִ�е�ַ(��ʽת���߳�)
	static DWORD WINAPI ThreadConvertProc(LPVOID pParam);


	//��Ϣ
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_select_music_1 , OnBtnSelectMusic1)
		EVENT_ID_COMMAND(R.id.btn_select_lyric_1 , OnBtnSelectLyric1)
		EVENT_ID_COMMAND(R.id.btn_select_output  , OnBtnSelectOutput)
		EVENT_ID_COMMAND(R.id.btn_load_1 , OnBtnLoad1)
		EVENT_ID_COMMAND(R.id.btn_start_making , OnBtnStartMaking)
		EVENT_ID_COMMAND(R.id.btn_restart , OnBtnRestart)
		EVENT_ID_COMMAND(R.id.btn_preview , OnBtnPreview)
		EVENT_ID_COMMAND(R.id.btn_open_output , OnBtnOpenOutput)
		
		EVENT_ID_COMMAND(R.id.btn_sound_open_1 , OnBtnSoundOpen1)
		EVENT_ID_COMMAND(R.id.btn_sound_close_1 , OnBtnSoundClose1)
		EVENT_NAME_HANDLER(R.name.slider_sound_1, EVT_SLIDER_POS, OnSliderPos1)

	EVENT_MAP_BREAK()

private:

	//�������ҳ�� ��·�� ��ѡ��״̬
	PATH_STATE PATH_STATE_1[3] ;
	
	SEdit	*m_EditMusic;
	SEdit	*m_EditLyric;
	SEdit	*m_EditOutPath;

	SStatic *m_txtMusic;
	SStatic *m_txtLyric;
	
	SStatic *P1_Line1 ;		/* ����ҳ���һ�и�ʿؼ� */
	SStatic *P1_Line2 ;		/* ����ҳ��ڶ��и�ʿؼ� */
	SStatic *P1_Line3 ;		/* ����ҳ������и�ʿؼ� */
	SStatic *emptyTip1;		/* ����ҳ�������ʾ�ؼ� */
	SWindow *m_wndHighlight;

	SStatic* m_txtTime;
	SProgress* m_ProgressTime;

	SButton *m_btnStart;
	SButton *m_btnPreview;
	SButton *m_btnOpenOutput;
	SButton *m_btnLoad;
	SButton *m_btnRestart;


	SHostWnd *m_pMainWnd;		/* ������ָ�� */
};