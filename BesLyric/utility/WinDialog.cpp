/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

#include "stdafx.h"
#include "WinDialog.h"
#include <Windows.h>
#include "Commdlg.h"
#include <fstream>
using namespace std;

/*  ���ڴ��ļ� �� �ļ���(��ʽһ) */
CBrowseDlg::CBrowseDlg(void)
{
	memset(m_pszDirPath, 0, MAX_PATH);
	memset(m_pszFilePath, 0, MAX_PATH);
}

CBrowseDlg::~CBrowseDlg(void)
{
}

//�ļ������
BOOL CBrowseDlg::DoDirBrowse(HWND hwndOwner, LPCTSTR pszTitle, BOOL bAddNewFolder,const WCHAR *szDefaultDir)  
{
	BROWSEINFO bi = {0};
	bi.hwndOwner = hwndOwner;  
	bi.lpszTitle = pszTitle;
	bi.ulFlags = bAddNewFolder ? BIF_NEWDIALOGSTYLE : 0;

	//����Ĭ��·��
	bi.lpfn = BrowseCallbackProc ;        //����CALLBACK����  
	bi.lParam = (LPARAM)szDefaultDir;	//����Ĭ��·��  

	PIDLIST_ABSOLUTE pItem = ::SHBrowseForFolder(&bi);  
	if (pItem != NULL)
	{
		BOOL bRet = ::SHGetPathFromIDListW(pItem, m_pszDirPath);

		LPMALLOC pMalloc;  
		if(SUCCEEDED(SHGetMalloc(&pMalloc)))//pidlָ��Ķ�������Ӧ���ͷ�
		{  
			pMalloc->Free(pItem);  
			pMalloc->Release();  
		}  

		return bRet;
	}

	return FALSE;  
}

//��ȡĿ¼
LPTSTR CBrowseDlg::GetDirPath()
{
	return m_pszDirPath;
}

//�ļ����
BOOL CBrowseDlg::DoFileBrowse(HWND hWnd, LPCTSTR pFilter, const WCHAR *pInitialDir)
{
	memset(m_pszFilePath, 0, MAX_PATH);
	OPENFILENAMEW ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = pFilter;  //_T("Exe�ļ�(*.exe)\0*.exe\0�����ļ�(*.*)\0*.*\0");
	ofn.lpstrInitialDir = pInitialDir;  //Ĭ�ϵ��ļ�·��
	ofn.lpstrFile = m_pszFilePath;  //����ļ��Ļ�����
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//��־����Ƕ�ѡҪ����OFN_ALLOWMULTISELECT  
	return GetOpenFileNameW(&ofn);
}

//��ȡ�ļ�·��
LPTSTR CBrowseDlg::GetFilePath()
{
	return m_pszFilePath;
}

int CALLBACK CBrowseDlg::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)  
{  
    switch(uMsg)  
    {  
    case BFFM_INITIALIZED:    //��ʼ����Ϣ  
        ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);   //  m_filePath Ϊ��ľ�̬����  
        break;  
    case BFFM_SELCHANGED:    //ѡ��·���仯��  
        {  
            TCHAR curr[MAX_PATH];     
            SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr);     
            ::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)curr);     
        }  
        break;  
    default:  
        break;  
    }  
    return 0;     
}



/*  ���ڴ��ļ� �� �ļ��У� �Լ������ļ�(��ʽ��) */

//���캯��
CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszFileName,LPCTSTR lpszFileDir,LPCTSTR lpszDefExt, LPCTSTR lpszFilter,
       DWORD dwFlags , HWND hWndParent, BOOL bFloder)
{
        memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
        m_szFileName[0] = _T('\0');
        m_szFileTitle[0] = _T('\0');

        m_bOpenFileDialog = bOpenFileDialog;
        m_ofn.lStructSize = sizeof(m_ofn);
        m_ofn.lpstrFile = m_szFileName;
		m_ofn.lpstrInitialDir = lpszFileDir;
        m_ofn.nMaxFile = _MAX_PATH;
        m_ofn.lpstrDefExt = lpszDefExt;
        m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
        m_ofn.nMaxFileTitle = _MAX_FNAME;
        m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING| OFN_NOCHANGEDIR;
        m_ofn.lpstrFilter = lpszFilter;
        m_ofn.hwndOwner = hWndParent;

        // setup initial file name
        if(lpszFileName != NULL)
            _tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);

		//�ļ�������
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//��Ҫʹ��NULL,��������޷����Ƶ�����
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
}

//��ѡ�񴰿�
INT_PTR CFileDialogEx::DoModal(HWND hWndParent)
{
     if(m_ofn.hwndOwner == NULL)   // set only if not specified before
         m_ofn.hwndOwner = hWndParent;

     if(m_bOpenFileDialog)
         return ::GetOpenFileName(&m_ofn);
     else
         return ::GetSaveFileName(&m_ofn);
}

//����ļ����Ƿ����Ҫ���ʽ
BOOL CFileDialogEx::checkPathName(LPCTSTR format,LPCTSTR toChecked)
{
	int i;
	bool isFloder = false;
	//TODO���쳣�׳�����
	int len = _tcslen(format);
	if(_tcscmp(format,_T(".."))==0)
	{
		isFloder = true;
	}
	else if(len < 3 || format[0]!=_T('*') || format[1]!=_T('.'))
		return FALSE;  //TODO���쳣
	

	//��ȡ����� ������·���ַ��� toChecked ����Ϣ
	TCHAR pathName[_MAX_PATH];
	TCHAR ext[_MAX_EXT];

	int lenPathName = 0, pos =-1;

	_tcscpy(pathName,toChecked);
	lenPathName = _tcslen(pathName);	//�õ�·���ܳ�
	if(!lenPathName)
		return FALSE;

	//�õ�·�������һ����.����λ������pos��
	for( i=0; i< lenPathName; i++)
	{
		if(_T('.')==pathName[i])
			pos = i;
	}

	if(isFloder) //����ļ�������
	{
		if(pos == -1)//����Ĭ���ļ��е�·���������κε�'.'
			return TRUE;
		else
			return FALSE;
	}
	else //�����ͨ��׺������
	{
		_tcscpy(ext,&pathName[pos+1]);  //�õ�·���ĺ�׺����������.����
		if(_tcscmp(&format[2],ext)==0)	//�� �����ṩ�ĺ�׺�Ա�
			return TRUE;
		else
			return FALSE;
	}
}


LONG g_lOriWndProc = NULL;
BOOL g_bReplaced = FALSE;

//���´����2���������ԣ�http://www.cnblogs.com/kocpp/p/5349467.html
/*
		//�ļ�������
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//��Ҫʹ��NULL,��������޷����Ƶ�����
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
*/

//ѡ���ļ���ʱ������HookProc: m_ofn.lpfnHook = (COMMDLGPROC)MyFolderProc; 

LRESULT  __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
    switch(uMsg)
    {
    case WM_COMMAND:
        {
            if( wParam == IDOK )
            {
                wchar_t wcDirPath[MAX_PATH] = {0};
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    GetWindowText(hComboAddr, wcDirPath, MAX_PATH);
                }
                if (!wcslen(wcDirPath))
                {
                    break;
                }
                DWORD dwAttr = GetFileAttributes(wcDirPath);
                if(dwAttr != -1 && (FILE_ATTRIBUTE_DIRECTORY & dwAttr))
                {
                    LPOPENFILENAMEW oFn = (LPOPENFILENAME)GetProp(hwnd, L"OPENFILENAME");
                    if (oFn)
                    {
                        int size = oFn->nMaxFile > MAX_PATH?MAX_PATH: oFn->nMaxFile;
                        memcpy(oFn->lpstrFile, wcDirPath, size * sizeof(wchar_t));
                        RemoveProp(hwnd, L"OPENFILENAME");
                        EndDialog(hwnd, 1);
                    }
                    else
                    {
                        EndDialog(hwnd, 0);
                    }
                }
                break;
            }
            //////////////////////////////////////////////////////////////////////////
            //��������toolbar������WM_COMMOND��Ϣ���������ߵ�toolbar��, �����OK��ť�Ե���Ͽ�
            HWND hCtrl = (HWND)lParam;
            if (hCtrl == NULL)
            {
                break;
            }
            int ctrlId = GetDlgCtrlID(hCtrl);
            if (ctrlId == ID_LEFT_TOOBAR)
            {
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    SetWindowTextW(hComboAddr, L"");
                }
            }
        }
    break;
    }    
    int i = CallWindowProc( (WNDPROC) g_lOriWndProc , hwnd, uMsg, wParam ,lParam );
    return i;
}

UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    //�ο�reactos��֪��hdlg ��һ�����صĶԻ����丸����Ϊ���ļ��Ի��� OK��CANCEL��ť�ȿؼ�����Ϣ�ڸ����ڴ���
    if(uiMsg == WM_NOTIFY)
    {
        LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;
        if (lpOfNotify->hdr.code == CDN_INITDONE )
        {
            SetPropW(GetParent(hdlg), L"OPENFILENAME", (HANDLE)(lpOfNotify->lpOFN));
            g_lOriWndProc  = ::SetWindowLongW( ::GetParent( hdlg ), GWL_WNDPROC , (LONG)_WndProc );    
        }
        if (lpOfNotify->hdr.code == CDN_SELCHANGE)
        {
            wchar_t wcDirPath[MAX_PATH] = {0};
            CommDlg_OpenSave_GetFilePathW(GetParent( hdlg ), wcDirPath, sizeof(wcDirPath));            
            HWND hComboAddr = GetDlgItem(GetParent(hdlg), ID_COMBO_ADDR);
            if (hComboAddr != NULL)
            {
                if (wcslen(wcDirPath))
                {
                    //ȥ���ļ��п�ݷ�ʽ�ĺ�׺����
                    int pathSize = wcslen(wcDirPath);
                    if (pathSize >= 4)
                    {
                        wchar_t* wcExtension = PathFindExtensionW(wcDirPath);
                        if (wcslen(wcExtension))
                        {
                            wcExtension = CharLowerW(wcExtension);
                            if(!wcscmp(wcExtension, L".lnk"))
                            {
                                wcDirPath[pathSize - 4] = L'\0';
                            }
                        }
                    }
                    SetWindowTextW(hComboAddr, wcDirPath);
                }
                else
                {
                    SetWindowTextW(hComboAddr, L"");
                }                
            }
        }
    }
    return 1;
}
