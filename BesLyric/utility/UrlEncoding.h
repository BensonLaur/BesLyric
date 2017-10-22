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

//�����ַ�����ز���
//�ο����ӣ� http://blog.csdn.net/evense/article/details/51289635
class CUrlEncodinig
{
public:  
    void UTF_8ToGB2312(string &pOut, char *pText, int pLen);//utf_8תΪgb2312  
    void GB2312ToUTF_8(string& pOut,char *pText, int pLen); //gb2312 תutf_8  
    string UrlGB2312(char * str);                           //urlgb2312����  
    string UrlUTF8(char * str);                             //urlutf8 ����  
    string UrlUTF8Decode(string str);                  //urlutf8����  
    string UrlGB2312Decode(string str);                //urlgb2312����  
  
private:  
    void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer);  
    void UTF_8ToUnicode(WCHAR* pOut,char *pText);  
    void UnicodeToUTF_8(char* pOut,WCHAR* pText);  
    void UnicodeToGB2312(char* pOut,WCHAR uData);  
    char CharToInt(char ch);  
    char StrToBin(char *str);  
};