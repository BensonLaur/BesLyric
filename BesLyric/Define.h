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
* @file       Define.h
* 
* Describe    ������ ����ʹ�õ�һЩ�ṹ
*/

#pragma once
#include "stdafx.h"
#include <string>
using namespace std;

/* �궨�� */
#ifdef _DEBUG

#define _KEEP_UPDATE  0			/* �Ƿ�������ʱ���³��� */

#else

#define _KEEP_UPDATE  1

#endif



/* �ṹ���� */

//���������ҳ�� �� ����Ԥ�� ҳ���У���Ҫѡ������·�������·�������·������Ϣ
//�ýṹ���ڴ洢 ·���� �Լ� �Ƿ��ѱ�ѡ���״̬
typedef struct _PATH_STATE
{	
	TCHAR nameOfPath[_MAX_PATH];
	bool isInited;
}PATH_STATE; 



enum ENCODING_TYPE			//�ļ���������
{
	ASCII,						
	UNICODE_LITTLE_ENDIAN,
	UNICODE_BIG_ENDIAN,
	UTF_8,
	OTHER
};


/* ȫ�ֱ������� */
static const SStringW VERSION_NUMBER = L"2.0.0";		//�汾�ţ�ע��ÿ�θ��İ汾��ʱ��Ҫ����2����1�������1���� BesLyric.rc �е�Version��

static const string SETTING_FILE_NAME = "setting";		//�����ļ���

//�������ҳ�� ��·�� ��ѡ��״̬
static PATH_STATE PATH_STATE_1[] = 
{
	{_T("�����ļ�"),false},
	{_T("����ļ�"),false},
	{_T("���·��"),false}
};

//����Ԥ��ҳ�� ��·�� ��ѡ��״̬
static PATH_STATE PATH_STATE_2[] = 
{
	{_T("�����ļ�"),false},
	{_T("����ļ�"),false}
};

