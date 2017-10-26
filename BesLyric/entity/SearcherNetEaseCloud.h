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
* @file       SearcherNetEaseCloud.h
* 
* @Describe   ����������������������࣬ʵ��������ʽӿڣ� 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "LrcHandler.h"
#include "..\utility\Downloader.h"
#include "..\utility\HttpRequest.h"
#include "..\utility\UrlEncoding.h"
#include "..\lib\json\json.h"
#include <unknown/obj-ref-impl.hpp>

class SearcherNetEaseCloud: public TObjRefImpl<ISearcher>
{
public:
	//�������
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo);
    
	//��ý���еĸ�����Ϣ�б�
	static bool GetSongListFromJson(wstring strJsonRes, vector< SONGINFO >& vecSongList);
private:

	//�ӵ������json���ݻ�ø��
	bool GetOneLyricFromJson(wstring strLyricJson,  LyricInfo& oneLyricInfo);

	//�Ӹ���ı������ȡ ��� ��Ϣ
	void GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo);
	
	/*
		@brief	�� �����ƻ�ȡ��json���ݻ�ȡ��� ���� m_vecIdList ��
		@note
		��ȡ����Ϣ�ĽṹΪ��
		{
		     "code" : 200,
			   "klyric" : {
				  "lyric" : "..."
				  "version" : 6
			   },
			   "lrc" : {
				  "lyric" : "[00:00.00] ���� : ������\n[00:01.00] ���� : ������/�����\n[00:15.750]�Ѿ�����һ�ٱ� ��ô���������\n[00:23.140]�Ӱ��쳪����ҹ ��һֱ����ߣ�һֱ����ߣ�\n[00:29.450]�������̫Σ�� ֻ�������ȫ  �����ҽ������� �ø�ʶ�ʵ��\n[00:42.519]�����ǿ��Ļ����ѹ��ҵİ�һֱ����\n[00:46.389]�����ص���ʱ�����Ŵ���һ�У�\n[00:50.789]������HIP-HOP����ҡ���ҵİ�һֱ����\n[00:54.389]���������û����¼�����棩\n[00:56.149]����Forever Love ��ô��\n[00:58.649]���ǵĸ� ��ô��\n[01:00.279]�޹��� ��ʱ��\n[01:02.179]�ٲ������KISS GOODBYE\n[01:03.669]Ҫÿһ���ܹ���������  YE~~\n[01:10.570]�����ַֺܷϺ�\n[01:13.509]��������ȴԽ��Խ��\n[01:17.389]��ʶ�����ҵ��Ҹ����  �ö�\n[01:24.240]�ܲ��ܲ�Ҫ�и�  ���������ǵĸ�\n[01:31.139]�øж�һ���Ӷ��ǵ�\n[01:37.580]en heng~~~\n[01:40.699]wo~  o~\n[01:48.309]�Ѿ�����һ�ٱ� ��ô���������\n[01:55.158]�Ӱ��쳪����ҹ ��һֱ����ߣ�һֱ����ߣ�\n[02:02.800]�������̫Σ�� ֻ�������ȫ  �����ҽ������� �ø�ʶ�ʵ��\n[02:15.150]�����ǿ��Ļ����ѹ��ҵİ�һֱ����\n[02:19.150]�����ص���ʱ�����Ŵ���һ�У�\n[02:22.000]������HIP-HOP����ҡ���ҵİ�һֱ����\n[02:25.670]���������û����¼�����棩\n[02:28.720]����Forever Love��ô�� ���ǵĸ���ô��\n[02:32.850]�޹��� ��ʱ��\n[02:34.750]�ٲ������KISS GOODBYE\n[02:36.228]Ҫÿһ���ܹ��������� YE~~\n[02:43.130]�����ַֺܷϺ�\n[02:46.140]��������ȴԽ��Խ��\n[02:50.300]��ʶ�����ҵ��Ҹ���� �ö�\n[02:56.880]�ܲ��ܲ�Ҫ�и� ���������ǵĸ�\n[03:03.660]�øж�һ���Ӷ��ǵ�\n[03:08.370]\n[03:18.440]��һ���Ӷ��ǵã�\n[03:36.240]�����ַֺܷϺ�\n[03:39.280]��������ȴԽ��Խ��\n[03:43.860]��ʶ�����ҵ��Ҹ���� �ö�\n[03:51.400]�ܲ��ܲ�Ҫ�и� ���������ǵĸ�\n[03:57.400]�øж�һ���Ӷ��ǵ�\n",
				  "version" : 18
			   },
			   "qfy" : false,
			   "sfy" : false,
			   "sgc" : true,
			   "tlyric" : {
				  "lyric" : null,
				  "version" : 0
			   }
		}
	*/
	bool AnalyseLyricJson(Json::Value & value, wstring& strLyric);

	/*
		@brief	�� �����ƻ�ȡ��json���ݻ�ȡ���id ���� m_vecIdList ��
		@note
		��ȡ����Ϣ�ĽṹΪ��
		{
		   "code" : 200,
		   "result" : {
			  "songCount" : 3,
			  "songs" : [
				 {
					"album" : {...},
					"alias" : [],
					"artists" : [{... ,"name" : "������", ...},{...}]
					...
					"id" : 25642952,
					"mvid" : 5293365,
					"name" : "���ǵĸ�",
					...
				 }
				 ,
				 {...}
				]
			}
		}
	*/
	static bool AnalyseIDJson(Json::Value & value, vector< SONGINFO >& vecSongList);

};




