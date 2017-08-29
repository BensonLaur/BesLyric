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
* @file       PageResult.h
* 
* Describe    CPageResult�࣬�鿴��ʽ����Ԥ�����ҳ��
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"
#include "PageMaking.h"

class CMainDlg;							//Ƕ�׶��壬������
class CPageResult
{
	friend class CMainDlg;	
	friend class CPageMaking;

public:
	CPageResult();
	~CPageResult(){};

	void Init(SHostWnd *pMainWnd);		//��ʼ������ҳ��
	CMainDlg* M();					//�����������ʵ������
	
	//����ҳ�水ť����Ӧ����

	void OnBtnSelectMusic2();		//����·����ѡ��
	void OnBtnSelectLyric2();	
	void OnBtnLoad2();
	void OnBtnEndPreview();
	void OnBtnStartPlaying();		//��ʼ���Ÿ��
	void OnBtnManualAdjust();		//�ֶ����ļ�����ʱ��
	
	void OnBtnSoundOpen2();			//������
	void OnBtnSoundClose2();		//����

	void OnSliderPos2(EventArgs *pEvt);//��������slider λ�õı仯

protected:
	//�ڶ���ҳ��(����Ԥ��)���ص������ذ�ť�����º��״̬
	void backToInit_2();	

	//�ڹ���Ԥ����ҳ�棬ִ�и����ʾ m_nCurLine
	void scrollToLyricCurLine();

private:
	
	//�жϵڶ���ҳ��(����Ԥ��) �� ����·���Ƿ�ѡ����� */
	bool isPathReady_2();

	/* ��õ�ǰδ��ʼ���ĵ�һ��·�� ��PATH_STATE_2 �ж�Ӧ���±�;����ʼ�����򷵻�-1 */
	int getPathNotReady_2();
	
	//�߳�ִ�е�ַ(��ʽת���߳�)
	static DWORD WINAPI ThreadConvertProc(LPVOID pParam);

	//��Ϣ
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_select_music_2 , OnBtnSelectMusic2)
		EVENT_ID_COMMAND(R.id.btn_select_lyric_2 , OnBtnSelectLyric2)
		EVENT_ID_COMMAND(R.id.btn_load_2 , OnBtnLoad2)
		EVENT_ID_COMMAND(R.id.btn_end_preview , OnBtnEndPreview)
		EVENT_ID_COMMAND(R.id.btn_start_playing , OnBtnStartPlaying)
		EVENT_ID_COMMAND(R.id.btn_manual_adjust , OnBtnManualAdjust)
		
		EVENT_ID_COMMAND(R.id.btn_sound_open_2 , OnBtnSoundOpen2)
		EVENT_ID_COMMAND(R.id.btn_sound_close_2 , OnBtnSoundClose2)
		EVENT_NAME_HANDLER(R.name.slider_sound_2, EVT_SLIDER_POS, OnSliderPos2)

	EVENT_MAP_BREAK()

	
private:
	
	//����Ԥ��ҳ�� ��·�� ��ѡ��״̬
	PATH_STATE PATH_STATE_2[2];
	
	SEdit	*m_EditMusic;
	SEdit	*m_EditLyric;
	
	SStatic *m_txtMusic;
	SStatic *m_txtLyric;
	
	SStatic *P2_Line1 ;		/* Ԥ��ҳ���һ�и�ʿؼ� */
	SStatic *P2_Line2 ;		/* Ԥ��ҳ��ڶ��и�ʿؼ� */
	SStatic *P2_Line3 ;		/* Ԥ��ҳ������и�ʿؼ� */
	SWindow *m_wndHighlight;
	
	SStatic *emptyTip2;		/* Ԥ��ҳ�������ʾ�ؼ� */

	SStatic* m_txtTime;
	SProgress* m_ProgressTime;
	
	SButton *m_btnStart;
	SButton *m_btnAdjust;
	SButton *m_btnEndPreview;
	SButton *m_btnLoad;

	SHostWnd *m_pMainWnd;		/* ������ָ�� */
	
};