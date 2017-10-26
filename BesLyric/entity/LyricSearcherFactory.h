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
* @file       LyricSearcherFactory.h
* 
* @Describe   �����࣬������ͬ�� ��������� ����������ʣ� 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "SearcherGecimi.h"
#include "SearcherNetEaseCloud.h"

enum SEARCH_FROM{
	SF_NETEASE = 0,
	SF_GECIMI = 1,
	UNDEFINED		//�� UNDEFINED �������PageSearchLyric.cpp �л����UNDEFINED ǰ������������Դ
};


class CLyricSearcherFactory
{
public:
	static void CreateSearcher(SEARCH_FROM search_from,OUT CAutoRefPtr<ISearcher>& p)
	{
		_CreateSearcher(search_from, (IObjRef**)&p);
	}

private:
    static void _CreateSearcher(SEARCH_FROM search_from, IObjRef** ref)
	{
		switch(search_from)
		{
		case SF_GECIMI:
			*ref = new SearcherGecimi();
			break;
		case SF_NETEASE:
			*ref = new SearcherNetEaseCloud();
			break;
		default:
			SASSERT(false && "δ֪�ĸ����������");
			break;
		}

	}
    
};
