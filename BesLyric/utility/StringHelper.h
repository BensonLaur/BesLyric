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
#include <vector>
using namespace std;

//�����ַ�����ز���
class CStringHelper
{
public:
	/*
	*	@brief	ȥ��ǰ���ַ�
	*	@param	strToTrim				�������ַ���
	*	@param	strCharactorRemoved		ȥ�����ַ�
	*	@return	
	*/
	static SStringW Trim(SStringW strToTrim, SStringW strCharactorRemoved = L" \t");
	static wstring Trim(wstring strToTrim, wstring strCharactorRemoved = L" \t");

	//���Trim֮��ĳ���
	static int GetTrimLength(SStringW strToTrim, SStringW strCharactorRemoved = L" \t");
	static int GetTrimLength(wstring strToTrim, wstring strCharactorRemoved = L" \t");

	//���ַ����ָ����
	static void SplitStringByToLines(SStringW strToSplted,WCHAR splitChar, vector<SStringW>& vecLines);

private:
	//�ַ��Ƿ����ַ����ڳ���
	static bool IsCharInString(WCHAR ch, SStringW str);
};