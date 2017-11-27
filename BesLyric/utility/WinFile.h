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

#pragma once
#include "stdafx.h"
#include <string>
#include <Windows.h> 
#include <vector>
#include "WinFile.h"
using namespace std;


/*
*   @brief	�ļ���ȡ�֧࣬��Windows���ֱ���
*					Ӧ��RAII˼�룬�����ļ��ļ�����Դ
*/
class File{

public:
	File():m_pf(NULL),m_lpszPathFile(NULL),m_lpszMode(NULL){}
	
	//��ȡ�ļ���ʱ��֧��ascii\unicode little endian\ unicode big endian \utf-8 �����ʽ
	//д����ļ�ʱ��ֱ���ɺ��� wideCharToMultiChar ��ascii ��ʽ��
	//mode : ��r����w��
	File(LPCTSTR pathFile,LPCTSTR mode);

	inline BOOL isValidFile(){return m_pf!=NULL;}

	~File(){
		//����Ѵ��ڣ����ͷ���Դ
		if(m_pf) fclose(m_pf);
	}
private:
	//���������Ƿ���UTF-8 ��Bom��ʽ  
	bool IsUTF8WithNoBom(const void* pBuffer, long size);   

public:
	LPCTSTR m_lpszPathFile;		/* �ļ�·�������ִ�*/
	LPCTSTR m_lpszMode;			/* ���ļ���ģʽ */

	FILE *m_pf;					/* ��ŵ�ǰ���ļ���ָ��*/
	ENCODING_TYPE::encoding_type m_encodingType;	/* ����ļ������ʽ */
};


/*
*	@brief	�����ļ��Ķ�д����
*/
class FileOperator
{
public:
	static bool ReadAllText(const string file, string& fileContent);

	static bool ReadAllLines(const string file, OUT vector<string> *lines);

	//��ȡ�ļ������У�֧��4��windows notepad ���������ļ����Զ�ȥ�����п���
	static bool ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines);
	static bool ReadAllLinesW(File& encodingFile,  OUT vector<SStringW> *lines);

	//��ascii ���뱣��
	static bool SaveToFile(const string file, string& fileContent);
	
	//�� UTF-8 ����д���ļ�
	static bool WriteToUtf8File(const wstring file, wstring fileContent);

	static bool WriteToUtf8File(const wstring file, vector<SStringW> lines);
};


/*
*	@biref	�����ļ�����������·�����ж�·���Ƿ�Ϊ�ļ��е�
*/
class FileHelper
{
public:
	/* ���Ӧ�ó���ǰ��·�� */
	static wstring GetCurrentDirectoryStr();

	/**
	*   @brief ����ļ����Ƿ����Ҫ���ʽ��������������ϵĸ�ʽ��
	*	@param  format  ֧�ֵļ���ʽ		��ͨ�ļ���ʽ���� *.txt�� *.mp3  ("*." �Ǳ���ģ��Һ�׺����������һ���ַ�)
	*										�ļ��и�ʽ��..
	*			toChecked ������·���ַ���
	*	@return TRUE ����Ҫ��
	*	@note
	*/
	static bool CheckPathName(LPCTSTR format, LPCTSTR toChecked);

	/*
	*   @brief �жϴ��ݵ�·���Ƿ�Ϊ·��
	*/
	static bool IsDirectory(SStringW path);


	/*
	*   @brief ��ѯ�ļ��Ƿ����
	*/
	static bool  CheckFileExist(const wstring &strPath);

	/*
	*   @brief ��ѯ�ļ����Ƿ����
	*/
	static bool  CheckFolderExist(const wstring &strPath);

	/**
	*	@brief �ָ�·����
	*	@note	·��������ȫ·������ C:\\document\\desktop\\test.pan
	*				Ҳ���Բ��ǣ�  �� ..\..\test\test.pan
	*/
	static void SplitPath(const wstring& strPathName, OUT wstring *strDrive = NULL, OUT wstring *strDirectory = NULL, OUT wstring* strName = NULL, OUT wstring* strExt = NULL);
};

  
