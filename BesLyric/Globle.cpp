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
#include "stdafx.h"
#include "Globle.h"
#include <time.h>
#include <direct.h>
#include <fstream>
using namespace std;

SINGLETON_C_D_0(CGloble)

	
bool CGloble::bDeleted = false;//�Ѿ�ɾ��

//��ó����·��
string CGloble::GetPragramDir()
{
	char exeFullPath[MAX_PATH]; // Full path  
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);

	string strPath(exeFullPath);
	strPath = strPath.substr(0, strPath.find_last_of("\\"));
	return strPath;
}

//������·����Ӧ����ʵ·��
string CGloble::GetRelativePath(string strRelativePath)
{
	return GetPragramDir() + strRelativePath;
}


//׷�Ӽ�¼
void CGloble::AppendLog(string info)
{
	string logPath = GetRelativePath("\\������Ϣ.txt");

	ofstream   oflog(logPath, ios::app);

	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "[%Y-%m-%d %H:%M:%S]", localtime(&timep));

	info = tmp + info;

	oflog << info << endl;

	oflog.close();
}

//ɾ������LOG
void CGloble::DeleteLogFile()
{
	if (!CGloble::bDeleted)
	{
		string logPath = GetRelativePath("\\������Ϣ.txt");

		remove(logPath.c_str());

		CGloble::bDeleted = true;
	}
}



