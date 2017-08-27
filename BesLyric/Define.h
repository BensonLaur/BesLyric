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

#define UPDATE_LOOP  0			/* �Ƿ�������ʱ���³��� */

#else

#define UPDATE_LOOP  1

#endif

#define MAX_BUFFER_SIZE	 (260 * 2)


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
static const wstring VERSION_NUMBER = L"2.0.1";		//�汾�ţ�ע��ÿ�θ��İ汾��ʱ��Ҫ����2����1�������1���� BesLyric.rc �е�Version��

static const wstring  LINK_VERSION_LOG= L"http://files.cnblogs.com/files/BensonLaur/versionLog.zip";			//���ӣ�ָ��汾��־�ļ�
static const wstring  LINK_LAST_VERSION_INFO= L"http://files.cnblogs.com/files/BensonLaur/lastVersion.zip";		//���ӣ�ָ�����汾��Ϣ���ļ�
static const wstring  LINK_LAST_EXE= L"http://files.cnblogs.com/files/BensonLaur/BesLyricExe.zip";				//���ӣ�ָ�����µ� EXE�ļ�

static const wstring LINK_SEND_LOGIN = L"http://beslyric.320.io/BesBlog/beslyric/login.action";

static const wstring FILE_NAME_LAST_VERSION_INFO = L"version";					//�ļ������� LINK_LAST_VERSION_INFO ��������������ļ�
static const wstring FILE_NAME_LAST_EXE_TEMP = L"BesLyric";						//�ļ������� LINK_LAST_EXE ��������������ļ�
static const wstring SETTING_FILE_NAME = L"setting";							//�ļ�������������

