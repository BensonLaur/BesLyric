/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

#include "stdafx.h"
#include "FileHelper.h"
#include <fstream>
using namespace std;

File::File(LPCTSTR pathFile,LPCTSTR mode)
{
	m_lpszPathFile = NULL;
	m_lpszMode = NULL;
	m_pf = NULL;
	m_encodingType = ENCODING_TYPE::OTHER;

	if(_tcscmp(mode,_T("r")) == 0)
	{
		//��ȡ�ļ���ǰ�����ֽڣ��жϱ���
		int len = 0;
		char *buf = NULL;
		byte firstByte = '\0';
		byte secondByte = '\0';
		ifstream in(pathFile);
		if(in)
		{
			in.seekg(0,ios::end);
			len = in.tellg();
			buf = new char[len];
			in.seekg(0,SEEK_SET);
			in.read(buf,len);

			if(len < 2)//�ļ�����̫�٣���ȡʧ��
				return ;
			
			firstByte = buf[0];
			secondByte = buf[1];

			in.close();
			delete buf;
		}
		else return;//���ļ�ʧ��

		if(firstByte == 0xef && secondByte == 0xbb)
			m_encodingType = ENCODING_TYPE::UTF_8;
		else if(firstByte == 0xff && secondByte == 0xfe)
			m_encodingType = ENCODING_TYPE::UNICODE_LITTLE_ENDIAN;
		else if(firstByte == 0xfe && secondByte == 0xff)
			m_encodingType = ENCODING_TYPE::UNICODE_BIG_ENDIAN;
		else 
			m_encodingType = ENCODING_TYPE::ASCII;	//Ĭ��ascii ����������룬��ascii ��ʽ����

		//���ݲ�ͬ�ı��룬���ļ�
		if(m_encodingType == ENCODING_TYPE::UTF_8)
			m_pf = _tfopen(pathFile, _T("r,ccs=utf-8"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN)
			m_pf = _tfopen(pathFile, _T("r,ccs=UNICODE"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN)
		{	
			//UNICODE_BIG_ENDIAN ��֧��_tfopen(pathFile, _T("r,ccs=UNICODE")); ��ʽ��ȡ

			//ת��ΪUNICODE_LITTLE_ENDIAN�ı���Ȼ���ٶ�ȡ

			//ʹ��c��ʽ��ȡ�ļ�
			int len = 0;
			FILE *in = _tfopen(pathFile,_T("rb"));
			if(in)
			{
				fseek(in, 0, SEEK_END);
				len = ftell(in);
				buf = new char[len];
				fseek(in, 0, SEEK_SET);
				fread(buf, sizeof(char), len, in);

				WCHAR *pWC;
				pWC = reinterpret_cast<WCHAR*>(buf);

				int index = 0;
				while(index < len/2)  //��ÿһ�����ֽڽ����ֽ���
				{
					WCHAR temp = *pWC >> 8;
					temp |= *pWC << 8;
					*pWC = temp;
					pWC++;
					index++;
				}

				//��c����ʽд����ʱ�ļ�
				FILE* tFile = _tfopen(_T("temp_unicode_little_endian.txt"),_T("wb"));
				if(tFile)
				{
					fwrite(buf, sizeof(char), len, tFile);
					fclose(tFile);
				}
				else return;//д���ļ�ʧ��
			}
			else return;//���ļ�ʧ��
			
			m_pf = _tfopen(_T("temp_unicode_little_endian.txt"), _T("r,ccs=UNICODE"));
		}
		else //m_encodingType == ENCODING_TYPE::ASCII
			m_pf = _tfopen(pathFile,mode);

	}
	else //if(_tcscmp(mode,_T("r"))
		m_pf = _tfopen(pathFile,mode);


	m_lpszPathFile = pathFile;
	m_lpszMode = mode;
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
