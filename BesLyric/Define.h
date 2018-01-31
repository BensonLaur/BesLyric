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
#include <vector>
#include <string>
#include "Globle.h"
using namespace std;

/* �궨�� */
#ifdef _DEBUG

#define UPDATE_LOOP  0			/* �Ƿ�������ʱ���³��� */

#else

#define UPDATE_LOOP  1

#endif


#define OPEN_DEBUG_LOG 0					//�Ƿ�򿪵���

#define G  (CGloble::GetInstance())


#if OPEN_DEBUG_LOG == 1
#define DLOG(info)			G->AppendLog(info)
#define DELETE_LOG()		;
#else 
#define DLOG(info)			;
#define DELETE_LOG()		G->DeleteLogFile();
#endif



#define RET_SUCCEEDED	0
#define MAX_BUFFER_SIZE	 (260 * 2)

#define MSG_USER_MAKING_START_BUTTON	WM_USER+1
#define MSG_USER_PLAYING_START_BUTTON	WM_USER+2

#define MSG_USER_UPDATE_CHECK_PROGRESS	WM_USER+3
#define MSG_USER_CLOSE_CHECK_PROGRESS	WM_USER+4

#define MSG_USER_DROP_FILE				WM_USER+5

#define MSG_USER_SHOW_LYRIC_RESULT		WM_USER+6
#define MSG_USER_SEARCH_WITH_GUESS_RESULT  WM_USER+7

/* �ṹ���� */

//���������ҳ�� �� ����Ԥ�� ҳ���У���Ҫѡ������·�������·�������·������Ϣ
//�ýṹ���ڴ洢 ·���� �Լ� �Ƿ��ѱ�ѡ���״̬
typedef struct _PATH_STATE
{	
	TCHAR nameOfPath[_MAX_PATH];
	bool isInited;
}PATH_STATE; 

namespace ENCODING_TYPE{
	enum encoding_type			//�ļ���������
	{
		ASCII,						
		UNICODE_LITTLE_ENDIAN,
		UNICODE_BIG_ENDIAN,
		UTF_8,
		UTF_8_NO_BOM,
		OTHER
	};
}//Ϊ�˱��� warning (nonstandard extension used: enum 'ENCODING_TYPE' used in qualified name) ������������ռ�

//�����ȡ�ĵ��������Ϣ
struct LyricInfo
{
	wstring strPlaneText;
	wstring strLabelText;
	wstring strSong;
	wstring strArtist;
	wstring strLyricFrom;
};

struct LyricSearchResult
{
	bool bCurrentSearchDone;						//��Ǳ�����ʰ�Ƿ����
	bool bAppendToList;								//�Ƿ�Ϊ׷�ӵ��б�false �������
	vector<LyricInfo> vecLyricInfoTotal;			//���л�õĸ��
	bool bShowUnexpectedResultTip;					//�Ƿ���ʾ����������ʾ
	SStringW strUnexpectedResultTip;				//����������ʾ
};

//�����Ƹ�����Ϣ�ṹ�������ڴ洢�������������������ݣ�
typedef struct _SONG_INFO{
	int nID;
	wstring strArtists;
	wstring strSong;
} SONGINFO;


struct SongInfoGuessResult
{
	int nResultType;   //1 ��ʾ��õ��˸������͸�����  2 ��ʾֻ��ȡ������  3��ʾû�л�ȡ���κβ²���
	SStringW strSongName;		//������
	SStringW strArtist;			//������Ϣ
};


/* ȫ�ֱ������� */
/* �汾��ʽ˵����X.Y.Z  
X��ʾ���汾���ܹ����޸�ʱ����
Y��ʾ�ΰ汾���ϴ��޸�ʱ����
Z��ʾ�޸ĺţ�С����ʱ����
*/
static const wstring VERSION_NUMBER = L"2.1.9";		//�汾�ţ�ע��ÿ�θ��İ汾��ʱ��Ҫ����2����1�������1���� BesLyric.rc �е�Version��

static const wstring  LINK_VERSION_LOG= L"http://files.cnblogs.com/files/BensonLaur/versionLog.zip";			//���ӣ�ָ��汾��־�ļ�
static const wstring  LINK_LAST_VERSION_INFO= L"http://files.cnblogs.com/files/BensonLaur/lastVersion.zip";		//���ӣ�ָ�����汾��Ϣ���ļ�
static const wstring  LINK_LAST_EXE= L"http://files.cnblogs.com/files/BensonLaur/BesLyricExe.zip";				//���ӣ�ָ�����µ� EXE�ļ�

static const wstring LINK_SEND_LOGIN = L"http://beslyric.320.io/BesBlog/beslyric/login.action";

static const wstring FILE_NAME_LAST_VERSION_INFO = L"version";					//�ļ������� LINK_LAST_VERSION_INFO ��������������ļ�
static const wstring FILE_NAME_LAST_EXE_TEMP = L"BesLyric";						//�ļ������� LINK_LAST_EXE ��������������ļ�
static const wstring SETTING_FILE_NAME = L"setting";							//�ļ�������������

static const wstring TEMP_WAV_FLODER_NAME = L"wav";								//�ļ������ƣ������ʱת���õ���wav�ļ�

static const wstring SERVER_FILE_EXTENTION_W = L".zip";							//�����ϴ��� cnblog���������ļ���չ��
static const string SERVER_FILE_EXTENTION_A = ".zip";	

static const wstring LINK_SERVER_PATH = L"http://files.cnblogs.com/files/BensonLaur/";		//���ӣ���������ַ

static const wstring LINK_DOWNLOAD_SERVER = L"http://ovfwclhwl.bkt.clouddn.com/";			//���ӣ���������ַ2


