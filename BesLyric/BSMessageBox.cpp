#include "stdafx.h"
#include "BSMessageBox.h"

//�����Զ���ͼ�����Ϣ
int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType)
{
	BSMessageBox m;
	return m.MessageBoxW(hwnd,content,tiltle,uType);
}