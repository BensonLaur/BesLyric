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
* @file       Globle.h
* 
* Describe    ������ ����ʹ�õ�ȫ�ֱ����ͷ���
*/


#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lib\ZSingleton.hpp"
using namespace std;


class CGloble{

	SINGLETON_0(CGloble)

public:
	//��ó����·��
	string GetPragramDir();

	//������·����Ӧ����ʵ·��
	string GetRelativePath(string strRelativePath);

	//׷�Ӽ�¼
	void AppendLog(string info);

	//ɾ������LOG
	void DeleteLogFile();

	static bool bDeleted;//�Ѿ�ɾ��

};
