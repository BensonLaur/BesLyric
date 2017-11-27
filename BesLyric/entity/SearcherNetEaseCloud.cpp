#include "stdafx.h"
#include "SearcherNetEaseCloud.h"

//�������
bool SearcherNetEaseCloud::SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo)
{
	CHttpRequest httpRequest;

	wstring wstrSong = S_CW2W(strSong).GetBuffer(1);
	wstring wstrArtist = S_CW2W(strArtist).GetBuffer(1);


	string strSongUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strSong).GetBuffer(1));
	string strArtistUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strArtist).GetBuffer(1));

	wstring strRes;
	if(!httpRequest.Post( "music.163.com/api/search/get/web", 
								"csrf_token=&s="+strArtistUrl+"+"+ strSongUrl +"&type=1&offset=0&total=True&limit=20",
								strRes))
	{
		m_strLastResult = L"��������ʧ�ܣ��޷���ȡ�����������";
		return false;
	}
	
	strRes = strRes.substr( strRes.find_first_of('{'), strRes.find_last_of('}') - strRes.find_first_of('{')+1);

	//��ȡid�б�
	vector< SONGINFO > vecSongList;
	if(!GetSongListFromJson(strRes, vecSongList))
	{
		m_strLastResult = L"�����Ƹ�����ݸ�ʽ�쳣���޷���������";
		return false;
	}

	for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
	{
		if( wstring::npos == iter->strSong.find(wstrSong))//�����������ڽ�������������
			continue;

		wstring strLyricJson;
		char szID[MAX_BUFFER_SIZE];

		//��ȡid��Ӧ�ĸ��
		if(!httpRequest.Get("music.163.com/api/song/lyric",string("os=osx&id=") + _itoa( iter->nID,szID, 10) + string("&lv=-1&kv=-1&tv=-1"), strLyricJson))
		{
			m_strLastResult = L"��������ʧ�ܣ��޷���ȡ�����������";
			return false;
		}

		//��json ���ݻ�ȡһ�������ӵ�
		LyricInfo oneLyricInfo;
		strLyricJson = strLyricJson.substr( strLyricJson.find_first_of('{'), strLyricJson.find_last_of('}') - strLyricJson.find_first_of('{')+1);
		
		if(!GetOneLyricFromJson(strLyricJson, oneLyricInfo))
			continue;
		
		oneLyricInfo.strSong =  SStringW(iter->strSong.c_str()).GetBuffer(1);
		oneLyricInfo.strArtist =  SStringW(iter->strArtists.c_str()).GetBuffer(1);
		vecLyricInfo.push_back(oneLyricInfo);
	}

	return true;
}


//��ý���еĸ�����Ϣ�б�
bool SearcherNetEaseCloud::GetSongListFromJson(wstring strJsonRes, vector< SONGINFO >& vecSongList)
{
	vecSongList.clear();
	
	//utf8 ���ַ���Ϊ ���ֽ�ascii�ַ�
	string strJson = CHttpRequest::Utf8ToMultiByte(strJsonRes);

	JSONCPP_STRING input = strJson;
	Json::Features features;
	bool parseOnly = true;
	Json::Value root;

	Json::Reader reader(features);
	bool parsingSuccessful = reader.parse(input.data(), input.data() + input.size(), root); //��������
	if (!parsingSuccessful) {
		return false;
	}

	//�� ��ȡ��json���ݻ�ȡ����������� m_vecLyricLink��
	if(!AnalyseIDJson(root, vecSongList)) 
		return false;

	return true;
}

//�ӵ������json���ݻ�ø��
bool SearcherNetEaseCloud::GetOneLyricFromJson(wstring strLyricJson,  LyricInfo& oneLyricInfo)
{
	//utf8 ���ַ���Ϊ ���ֽ�ascii�ַ�
	string strJson = CHttpRequest::Utf8ToMultiByte(strLyricJson);

	JSONCPP_STRING input = strJson;
	Json::Features features;
	bool parseOnly = true;
	Json::Value root;

	Json::Reader reader(features);
	bool parsingSuccessful = reader.parse(input.data(), input.data() + input.size(), root); //��������
	if (!parsingSuccessful) {
		return false;
	}

	wstring strLyric;
	//�� ��ȡ��json���ݻ�ȡ����������� m_vecLyricLink��
	if(!AnalyseLyricJson(root, strLyric)) 
		return false;

	GetOneLyricInfoFromLyricBuffer(SStringW(strLyric.c_str()).GetBuffer(1), oneLyricInfo);
	return true;
}

//�Ӹ���ı������ȡ ��� ��Ϣ
void SearcherNetEaseCloud::GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo)
{
	LrcProcessor processor( SStringW(strLyricBuffer.c_str()));
	vector<TimeLineInfo> vecTimeLineInfo = processor.GetNeteaseLrc();

	lyricInfo.strSong = processor.m_strTitle.GetBuffer(1);
	lyricInfo.strArtist = processor.m_strArtist.GetBuffer(1);
	lyricInfo.strLyricFrom = L"����������";

	wstring strPlaneText = L"";
	wstring strLabelText = L"";;

	for(auto iter = vecTimeLineInfo.begin(); iter != vecTimeLineInfo.end(); iter++)
	{
		strPlaneText += iter->m_strLine;
		strLabelText += iter->m_strTimeLine;
	}

	lyricInfo.strPlaneText = strPlaneText;
	lyricInfo.strLabelText = strLabelText;
}


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
bool SearcherNetEaseCloud::AnalyseLyricJson(Json::Value & value, wstring& strLyric)
{
	if(value.type() == Json::objectValue) //{}
	{
		Json::Value::Members members(value.getMemberNames());

		for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
		{
			//�ڵ����� code��klyric��lrc �����Գ�Ա
			//�����ȡlrc 
			const JSONCPP_STRING name = *it;
			if(name == "lrc")
			{
				Json::Value lrc = value[name]; //���lrc �ڵ�
				if(value.type() == Json::objectValue) //{}
				{
					Json::Value::Members lrcMembers(lrc.getMemberNames());
					for (Json::Value::Members::iterator itLM = lrcMembers.begin(); itLM != lrcMembers.end();++itLM) 
					{
						//"lyric" �� version" 2����Ա��ֱ�ӻ�ȡlyric ����
						const JSONCPP_STRING name = *itLM;
						if(name == "lyric")
						{
							if(lrc[name].type() == Json::stringValue)  //����lyric �ַ�������
							{
								string str = lrc[name].asString();
								strLyric = CHttpRequest::MultiByteToUtf8(lrc[name].asString());
								return true;
							}
							else
								return false;
						}
					}
				}
				else 
					return false;
			}//if(name == "lrc")
		}
	}

	return false;
}

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
bool SearcherNetEaseCloud::AnalyseIDJson(Json::Value & value, vector< SONGINFO >& vecSongList)
{
	if(value.type() == Json::objectValue) //{}
	{
		Json::Value::Members members(value.getMemberNames());

		for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
		{
			//�ڵ����� code��result �����Գ�Ա
			//�����ȡresult����
			const JSONCPP_STRING name = *it;
			if(name == "result")
			{
				Json::Value result = value[name]; //���result �ڵ�

				Json::Value::Members members(result.getMemberNames());
				for(Json::Value::Members::iterator itRes = members.begin(); itRes != members.end();++itRes)
				{
					//result ����2����Ա��songCount��songs
					//�����ȡ songs ����
					const JSONCPP_STRING name = *itRes;
					if(name == "songs")
					{
						Json::Value songsArray = result[name];  //���songs�ڵ�
						if(songsArray.type() == Json::arrayValue) //[]
						{
							Json::ArrayIndex size = songsArray.size();
							for (Json::ArrayIndex index = 0; index < size; ++index) 
							{
								Json::Value oneSong = songsArray[index];

								if(oneSong.type() == Json::objectValue)//{}
								{
									//ÿһ������Ϣ���� id��name��artist �����Գ�Ա
									Json::Value::Members members(oneSong.getMemberNames());

									int nID;
									wstring strArtists, strSong;
									for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
									{
										const JSONCPP_STRING member = *it;
										if(member == "id")
										{
											if(oneSong[member].type() == Json::intValue)  //����id����������
											{
												nID = oneSong[member].asInt(); //��ø���id
											}
											else
												return false;
										}
										else if(member == "name")
										{
											if(oneSong[member].type() == Json::stringValue)  //����name���ַ���
											{
												wstring wstr = CHttpRequest::MultiByteToUtf8(oneSong[member].asString());
												strSong = wstr; //��ø�������
											}
											else
												return false;
										}
										else if(member =="artists")
										{
											Json::Value artistArray = oneSong[member];  //���artists�ڵ�
											if(artistArray.type() == Json::arrayValue) //[]
											{
												int count = 0; //��¼���ָ���
												Json::ArrayIndex size = artistArray.size();
												for (Json::ArrayIndex index = 0; index < size; ++index) //��������artist
												{
													Json::Value oneArtist = artistArray[index];

													if(oneArtist.type() == Json::objectValue)//{}
													{
														//ÿһ������������ img1v1Url��name �����Գ�Ա
														//�����ȡ name, ׷����һ��  strArtists �ַ�����
														Json::Value::Members artistMembers(oneArtist.getMemberNames());
														for (Json::Value::Members::iterator itA = members.begin(); itA != members.end();++itA) 
														{
															const JSONCPP_STRING member = *itA;
															if(member == "name")
															{
																if(count++ > 0)
																{
																	strArtists += L"��";
																}
																wstring wstr = CHttpRequest::MultiByteToUtf8(oneArtist[member].asString());
																strArtists += wstr;
																break;
															}
														}
													}
													else
														return false;
												}
											}else 
												return false;
										}//oneSong ��������Ա������
									}
									SONGINFO songInfo;
									songInfo.nID = nID;
									songInfo.strArtists = strArtists;
									songInfo.strSong = strSong;
									vecSongList.push_back(songInfo);  //����һ�׸��ռ�����Ϣ����

								}//if(oneLyric.type() == Json::objectValue)
								else
									return false;
							}
						}//if(resultArray.type() == Json::arrayValue)
						else
							return false;
					}//if(name == "songs")
				}
			}//if(name == "result")
		}
	}
	else
		return false;

	return true;
}
