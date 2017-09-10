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
* @file       LrcHandler.h
* 
* Describe    ���崦��LRC �ļ��������
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinDialog.h"

#include <vector>
using namespace std;

/*
*	@brief ���洦��һ�и���ļ��� ����lrc�ļ�����ʱ����ĸ���ļ����ĸ�����
*/
class TimeLineInfo
{
public:
	TimeLineInfo(){}//�޲ι��캯��

	TimeLineInfo(SStringT timeLine);
	
	 bool operator < (const TimeLineInfo &m)const { return m_nmSesonds < m.m_nmSesonds; }

private:
	//��ʱ���ǩ�ַ����õ���Ӧ�ĺ���ʱ��
	int TimeStringToMSecond(LPCTSTR timeStr, int length);

	//�����޷���ʮ���ƴ���Ӧ�����֣�ʮ���ƴ������� 023��12��04 ����ʽ����ֵΪ0��999��
	int DecStrToDecimal(LPCTSTR timeStr);

public:
	SStringT m_strTimeLine;		/* ֱ�Ӵ洢 ���ļ���ȡ��һ���� */
	SStringT m_strLine;			/* �洢ȥ��ʱ����֮������� */
	int	m_nmSesonds;			/* �洢ʱ���Ƕ�Ӧ�ĺ���ʱ�� */
	bool m_bIsEmptyLine;		/* �Ƿ�Ϊ���У�ֻ��ʱ���ǣ� */
};


class LrcProcessor{
public:
	//ʹ�ö�ȡ�ĵ��ļ��������г�ʼ��Lrc������
	LrcProcessor(vector<SStringW> vecLines, bool bDealOffset = true);

	//ʹ������LRC�ı����ݳ�ʼ��Lrc������
	LrcProcessor(SStringW AllContent, bool bDealOffset = true);

	//��ǰ����ļ��Ƿ���Ч
	bool IsLrcFileValid();

	//�������������֧�ֵĸ�ʽ
	vector<TimeLineInfo> GetNeteaseLrc();

	//����Lrc�ļ�
	bool GenerateNeteaseLrcFile(SStringW strFilePath);

private:
	//���� vector ������
	void ProcessData(const vector<SStringW>& vecLines, bool bDealOffset = true);

	//����1��
	bool DealOneLine(const SStringW& strLine);

	//�����ʱ������Ƚ�
	bool CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2);

	//��֤ʱ������Ч���޷���֤�򷵻� false��
	bool MakeSureTimeLineValid(wstring& line);

public:
	SStringW	m_strTitle;					/* �������� */
	SStringW	m_strArtist;				/* ������ */
	SStringW	m_strAlbum;					/* ר�� */
	SStringW	m_strEditor;				/* �༭���� */

private:
	bool m_bDealOffset;						/* ��ʾ�Ƿ���LRC�ļ��е�ʱ��ƫ�� */
	
	int			m_nOffset;					/* ʱ��ƫ������Ϊ������ʾ������ǰ */

	vector<TimeLineInfo> m_vecNeteaseLrc;	/* �������������Ƶĸ����Ϣ */

	bool	m_bIsLrcFileValid;				/* ��ʾ����ļ��Ƿ���Ч */
};

