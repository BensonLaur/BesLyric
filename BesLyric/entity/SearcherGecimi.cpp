#include "stdafx.h"
#include "SearcherGecimi.h"

bool SearcherGecimi::SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo)
{
	wstring strSearchUrl = L"http://gecimi.com/api/lyric";

	strSearchUrl += ( strSong.GetLength() == 0 ? L"" : (wstring(L"/") +  strSong.GetBuffer(0)));
	strSearchUrl += ( strArtist.GetLength() == 0 ? L"" : (wstring(L"/") +  strArtist.GetBuffer(0)));

	wstring strSaveBuffer;
	if(!CDownloader::DownloadString( strSearchUrl, strSaveBuffer))
	{
		m_strLastResult = L"��������ʧ�ܣ��޷���ȡ����";
		return false;
	}

	vector<string>	vecLyricLink;

	//�� ����Ի�ȡ��json���ݻ�ȡ�������
	if(!GetLyricLinkFromJson(strSaveBuffer, vecLyricLink))
	{
		m_strLastResult = L"��������ݸ�ʽ�쳣���޷���������";
		return false;
	}

	//�� ȥ���������ظ���ļ�������þ�����Ϣ���� vecLyricInfo ��
	vecLyricInfo.clear();
	GetLyricInfoFromLinks(vecLyricLink, vecLyricInfo);

	if(vecLyricLink.size() > vecLyricInfo.size())
		m_strLastResult.Format(L"����ѯ��%d�����ݣ��ɹ�����%d������",vecLyricLink.size(),vecLyricInfo.size());
	else
		m_strLastResult.Format(L"�ɹ�����ѯ����%d������",vecLyricInfo.size());

	return true;
}

//�� ȥ���������ظ���ļ�������þ�����Ϣ���� vecLyricInfo ��
void SearcherGecimi::GetLyricInfoFromLinks(vector<string>& vecLyricLink, vector<LyricInfo>& vecLyricInfo)
{
	for(auto iter = vecLyricLink.begin(); iter != vecLyricLink.end(); iter++)
	{
		SStringW strlyricLink = S_CA2W( SStringA( iter->c_str()));
		wstring strLyricBuffer;
		if(!CDownloader::DownloadString( wstring(strlyricLink.GetBuffer(1)),strLyricBuffer))
			continue;
		
		//�Ӹ���ı������ȡ ��� ��Ϣ
		LyricInfo lyricInfo;
		GetOneLyricInfoFromLyricBuffer(strLyricBuffer, lyricInfo);
		
		vecLyricInfo.push_back(lyricInfo);
	}
}

//�Ӹ���ı������ȡ ��� ��Ϣ
void SearcherGecimi::GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo)
{
	LrcProcessor processor( SStringW(strLyricBuffer.c_str()));
	vector<TimeLineInfo> vecTimeLineInfo = processor.GetNeteaseLrc();

	lyricInfo.strSong = processor.m_strTitle.GetBuffer(1);
	lyricInfo.strArtist = processor.m_strArtist.GetBuffer(1);
	lyricInfo.strLyricFrom = L"�����";
	
	wstring strPlaneText = L"";
	wstring strLabelText = L"";;

	WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
	for(auto iter = vecTimeLineInfo.begin(); iter != vecTimeLineInfo.end(); iter++)
	{
		strPlaneText += iter->m_strLine;
		strLabelText += iter->m_strTimeLine;
	}
	
	lyricInfo.strPlaneText = strPlaneText;
	lyricInfo.strLabelText = strLabelText;
}

//�� ����Ի�ȡ��json���ݻ�ȡ�������
bool SearcherGecimi::GetLyricLinkFromJson(wstring strLyricJson, vector<string>& vecLyricLink)
{
	m_vecLyricLink.clear();
	vecLyricLink.clear();

	string strJson = S_CW2A(SStringW(strLyricJson.c_str())).GetBuffer(1);

	JSONCPP_STRING input = strJson;
	Json::Features features;
	bool parseOnly = true;
	Json::Value root;

	Json::Reader reader(features);
	bool parsingSuccessful = reader.parse(input.data(), input.data() + input.size(), root); //��������
	if (!parsingSuccessful) {
		return false;
	}

	//�� ����Ի�ȡ��json���ݻ�ȡ����������� m_vecLyricLink��
	if(!AnalyseJson(root)) 
		return false;

	vecLyricLink = m_vecLyricLink;

	return true;
}


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
bool SearcherGecimi::AnalyseJson(Json::Value & value)
{
	if(value.type() == Json::objectValue) //{}
	{
		Json::Value::Members members(value.getMemberNames());

		for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
		{
			//�ڵ����� code��count��result �����Գ�Ա
			//�����ȡresult����
			const JSONCPP_STRING name = *it;
			if(name == "result")
			{
				Json::Value resultArray = value[name];
				if(resultArray.type() == Json::arrayValue) //[]
				{
					Json::ArrayIndex size = resultArray.size();
					for (Json::ArrayIndex index = 0; index < size; ++index) 
					{
						Json::Value oneLyric = resultArray[index];

						if(oneLyric.type() == Json::objectValue)//{}
						{
							//ÿһ�������Ϣ���� aid��song��lrc �����Գ�Ա
							//�����ȡlrc  ����
							Json::Value::Members members(oneLyric.getMemberNames());

							for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
							{
								const JSONCPP_STRING member = *it;
								if(member == "lrc")
								{
									if(oneLyric[member].type() == Json::stringValue)
									{
										m_vecLyricLink.push_back(oneLyric[member].asString()); //��ø������
									}
									else
										return false;
								}
							}
						}//if(oneLyric.type() == Json::objectValue)
						else
							return false;
					}
				}//if(resultArray.type() == Json::arrayValue)
				else
					return false;
			}//if(name == "result")
		}
	}
	else
		return false;

	return true;
}

