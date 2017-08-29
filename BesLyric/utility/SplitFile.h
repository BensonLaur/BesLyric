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
using namespace std;

//���ڷָ��ļ�������ļ�
class CSplitFile
{
public:
	/*
	*	@brief	�ָ��ļ� 
	*	@param	strSrcFile		�ļ�·��
	*	@param	strToPath		�ָ�󴢴�·��(���ԡ�\����β)
	*	@param	strToName		�ָ�󴢴���ļ���
	*	@param	nBlockSize		�ʸ���Ԫ��С
	*	@return	
	*/
	static bool SplitFile(wstring strSrcFile, wstring strToPath, wstring strToName, unsigned int nBlockSize);

	/* 
	*	@brief	�ϲ����ָ���ļ� 
	*	@param	strSplitFilePath	�ļ�����·�������ָ����ļ��� (���ԡ�\����β)
	*	@param	strFileName			�ļ���ǰ׺�����ָ����ļ���
	*	@param	strMergeToPath		�ϲ��ļ��󴢴��·�� (���ԡ�\����β)
	*	@return	
	*/
	static bool MergeFile(wstring strSplitFilePath, wstring strFileName, wstring strMergeToPath);

private:
	/* ���º����� MergeFile ����  */

	//��ȡ���ָ���ļ���
	static bool GetMaxSplitedCount(wstring strSplitFilePath,wstring strFileName, int &nMaxCount);

	//��ȡ�ļ���չ���� �ļ���С
	static bool GetMergedFileExtAndSize(wstring strMergedFile, WCHAR* strExt, int nSize,int &nSizeOfFileRecord);

	//�ϲ�Ϊͬһ���ļ�
	static bool MergeToOneFile(wstring strMergedFilePathName,wstring  strTargetFileNamePreW, int nMaxCount, int &nCumulateSize);
};