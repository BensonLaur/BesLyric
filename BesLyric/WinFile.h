#pragma once
#include "stdafx.h"
#include <string>
#include <Windows.h> 
#include <vector>
using namespace std;

/*
*	@brief	�����ļ��Ķ�д����
*/
class FileOperator
{
public:
	static bool ReadAllText(const string file, string& fileContent);

	static bool ReadAllLines(const string file, OUT vector<string> *lines);

	static bool ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines);

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
	static void SplitPath(const string& strPathName, OUT string *strDrive = NULL, OUT string *strDirectory = NULL, OUT string* strName = NULL, OUT string* strExt = NULL);
};

  
