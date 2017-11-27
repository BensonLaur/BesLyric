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
* @file       WinDialog.h
* 
* Describe   �Ի��������Ķ���
*/


#pragma once
#include "stdafx.h"
#include <windows.h>
#include <Shlobj.h>

//�������ļ�ÿһ�е�����ַ���
#define MAX_CHAR_COUNT_OF_LINE 1000
#define MAX_WCHAR_COUNT_OF_LINE MAX_CHAR_COUNT_OF_LINE/2

//��дGetOpenFileNameʱ�õ���ϵͳʹ�ÿؼ�����ԴID
#define  ID_COMBO_ADDR 0x47c
#define  ID_LEFT_TOOBAR 0x4A0

//ʹ��GetOpenFileName���ļ���ʱ �����滻 m_ofn.lpfnHook �Ļص����� 
UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );


/*
*	@brief ���ڴ��ļ� �� �ļ���(��ʽһ)
*/
class CBrowseDlg
{
	public:
		CBrowseDlg(void);
		~CBrowseDlg(void);

	protected:
		WCHAR m_pszDirPath[MAX_PATH]; //ѡ���Ŀ¼
		WCHAR m_pszFilePath[MAX_PATH];  //ѡ���е��ļ���ַ

	public:
		BOOL DoDirBrowse(HWND hwndOwner, LPCTSTR pszDisplayName, BOOL bAddNewFolder, const WCHAR *szDefaultDir = NULL);  //�ļ������
		BOOL DoFileBrowse(HWND hWnd, LPCTSTR pFilter, const WCHAR *pInitialDir);  //�ļ�ѡ����

		LPTSTR GetDirPath();  //��ȡĿ¼·��
		LPTSTR GetFilePath();  //��ȡ�ļ�·��

private:
		//�ο� http://blog.csdn.net/shuilan0066/article/details/7302904
		static  int CALLBACK  BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);

};


/*
*	@brief ���ڴ��ļ� �� �ļ��У� �Լ������ļ�(��ʽ��)
*/
class CFileDialogEx
{
public:

    OPENFILENAME m_ofn;
    BOOL m_bOpenFileDialog;            // TRUE for file open, FALSE for file save
    TCHAR m_szFileTitle[_MAX_FNAME];   // contains file title after return
    TCHAR m_szFileName[_MAX_PATH];     // contains full path name after return

	//���캯��
    CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszFileName = NULL,
		LPCTSTR lpszFileDir = NULL,
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFilter = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
        HWND hWndParent = NULL,
		BOOL bFloder = FALSE);

	//��ѡ�񴰿�
    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow());

public:
	/**
	*   @brief ����ļ����Ƿ����Ҫ���ʽ��������������ϵĸ�ʽ��
	*	@param  format  ֧�ֵļ���ʽ		��ͨ�ļ���ʽ���� *.txt�� *.mp3  ("*." �Ǳ���ģ��Һ�׺����������һ���ַ�)
	*										�ļ��и�ʽ��..
	*			toChecked ������·���ַ���
	*	@return TRUE ����Ҫ��
	*	@note	
	*/
	static BOOL checkPathName(LPCTSTR format,LPCTSTR toChecked);

	//�ж��ļ����Ƿ����
	static BOOL IsFloderExist(const wstring &strPath);
};

