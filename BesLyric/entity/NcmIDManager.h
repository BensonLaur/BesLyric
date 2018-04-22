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
* @file       NcmIDManager.h
* 
* @Describe   ���� ncm �ļ����� ��������id ��ƥ���߼� 
*/

#pragma once
#include "stdafx.h"
#include <string>
#include "..\lib\ZSingleton.hpp"
#include <map>
using namespace std;

class CNcmIDManager
{
	SINGLETON_0(CNcmIDManager)


public:
	bool SaveDataPairs();					//�������ݶ�
	bool LoadDataPairs();					//�������ݶ�

	bool FindID( wstring fileName,OUT wstring& id);			//����ID
	void InsertNcmIDPair( wstring fileName, wstring id);	//����ID��

	//���ID�Ƿ���Ч
	//���� false Ϊ��ѯʧ�ܣ���������ȣ�
	bool CheckIDValidity(wstring id, OUT bool& bValid);				

private:
	//����ID
	bool UpdateID( wstring fileName, wstring id);

private:
	map< wstring, wstring>	m_mapNcmID;	/* ����ncm�ļ��������ӦID��ƥ���ϵ */
};
