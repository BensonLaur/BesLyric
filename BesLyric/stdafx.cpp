// stdafx.cpp : source file that includes just the standard includes
//	SkinTest.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


//�����ڲ���ʱ������д�� ������ʾ�Ĵ��� : MB("content");
void MB(LPCTSTR content)
{
	MessageBox(NULL,content,_T("tip"),MB_OK);
}