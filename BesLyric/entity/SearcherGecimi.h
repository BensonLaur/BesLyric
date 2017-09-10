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
* @file       SearcherGecimi.h
* 
* @Describe   �������������Ҫ�ĳ���ӿڣ� 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "LrcHandler.h"
#include "..\utility\Downloader.h"
#include "..\lib\json\json.h"
#include <unknown/obj-ref-impl.hpp>

class SearcherGecimi: public TObjRefImpl<ISearcher>
{
public:
	//�������
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo);
    
private:
	//�� ȥ���������ظ���ļ�������þ�����Ϣ���� vecLyricInfo ��
	void GetLyricInfoFromLinks(vector<string>& vecLyricLink, vector<LyricInfo>& vecLyricInfo);

	//�Ӹ���ı������ȡ ��� ��Ϣ
	void GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo);

	//�� ����Ի�ȡ��json���ݻ�ȡ�������
	bool GetLyricLinkFromJson(wstring strLyricJson, vector<string>& vecLyricLink);


	/*
		@brief	�� ����Ի�ȡ��json���ݻ�ȡ����������� m_vecLyricLink��
		@note
		����Ի�ȡ�ĸ�ʵĽṹΪ��
		{
		   "code" : 0,
		   "count" : 6,
		   "result" : [
			  {
				 "aid" : 3179315,
				 "artist_id" : 11807,
				 "lrc" : "http://s.gecimi.com/lrc/388/38847/3884774.lrc",
				 "sid" : 3884774,
				 "song" : "���ǵĸ�"
			  },
			  {
			  ...
			  }
			]
		}
	*/
	bool AnalyseJson(Json::Value & value);

private:
	vector<string>	m_vecLyricLink;		//��Ż�ȡ���ĸ������
};
