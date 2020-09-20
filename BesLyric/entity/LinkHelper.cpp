#include "stdafx.h"

#include<algorithm>
#include<ctime>

#include "LinkHelper.h"
#include "UpdateHelper.h"

#include "../utility/Downloader.h"
#include "../utility/WinFile.h"

#include "..\lib\md5\md5.h"
#include "..\lib\xml\tinyxml2.h"
using namespace tinyxml2;

/*!
	�ӷ�������������

	\return �����Ƿ��ȡ�ɹ�
*/
bool LinkHelper::UpdateLinkFromServer()
{
	Enter();
	
	bool getValidUpdate = false;
	if(HasFinishedUpdate())
		getValidUpdate = true;
	else
	{
		//�ӷ�������ȡ��������
		vector<wstring> candidates;
		candidates.push_back(L"https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml");
		candidates.push_back(L"http://bensonlaur.com/files/beslyric/Beslyric-Link.xml");
	
		UpdateHelper updateHelper;
		wstring etcDir = updateHelper.GetEtcDir();
		if(!FileHelper::IsFloderExistW(etcDir))
			_wmkdir(etcDir.c_str());

		wstring xml = updateHelper.GetEtcDir()+ L"dynamicLink.xml";
		for(size_t i = 0; i < candidates.size(); ++i)
		{
			if(!CDownloader::DownloadFile(candidates[i], xml))
				continue; //����ʧ��

			if(!IsValidXml(xml))
				continue; //�������������ݲ���

			if(!GetLinkUpdate(xml))
				continue; //xml��������������һ��

			//�ɹ���ȡ����������
			getValidUpdate = true;
			break;
		}
	}

	Leave();

	return getValidUpdate;
}

/*
	������е� ffmpeg �������ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
*/
vector<LinkValue> LinkHelper::GetAllLinksFFmpeg()
{
	vector<LinkValue> links = GetLinksFFmpegRaw();
	links.insert(links.end(),linksFFmpegUpdate.begin(), linksFFmpegUpdate.end());

	//�����㷨 
	//srand����Ҫ����#include<ctime>ͷ�ļ� 
	srand((unsigned int)time(0));
	random_shuffle(links.begin(),links.end());

	return links;
}

/*
	������е� ip ��ȡ���ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
*/
vector<LinkValue> LinkHelper::GetAllLinksIp()
{
	vector<LinkValue> links = GetLinksIpRaw();
	links.insert(links.end(),linksIpUpdate.begin(), linksIpUpdate.end());

	//�����㷨 
	//srand����Ҫ����#include<ctime>ͷ�ļ� 
	srand((unsigned int)time(0));
	random_shuffle(links.begin(),links.end());

	return links;
}

/*!
	����ļ��Ƿ�Ϊ��Ч�� xml �ļ�

	\param filePath �������ļ�
	\return �����ļ��Ƿ�Ϊ��Ч�� xml �ļ�
*/

bool LinkHelper::IsValidXml(const std::wstring& filePath)
{
	//��ȡXML�ļ�
	tinyxml2::XMLDocument doc;
	FILE* fp = 0;
	errno_t err = _tfopen_s(&fp, filePath.c_str(), L"rb");
	if (!fp || err)
	{
		return false;
	}
	doc.LoadFile(fp);
	fclose(fp);

	if(doc.Error())
	{
		return false;
	}

	return true;
}
	
/*
	��ó����Դ��� ffmpeg ��������
*/
vector<LinkValue> LinkHelper::GetLinksFFmpegRaw()
{
	if(linksFFmpegRaw.empty())
	{
		linksFFmpegRaw.push_back(LinkValue(L"gitlab",L"https://gitlab.com/BensonLaur/resource-provider-gitlab/-/raw/master/beslyric/ffmpeg-3.4.1.exe"));
		linksFFmpegRaw.push_back(LinkValue(L"bitbucket",L"https://bitbucket.org/bensonlaur/resource-provider-bitbucket/raw/bcd1a44f30893427a5f78243e4548f9afbb82c9c/beslyric/ffmpeg-3.4.1.exe"));
	}

	return linksFFmpegRaw;
}

/*
	��ó����Դ��� ip ���ػ�ȡ
*/
vector<LinkValue> LinkHelper::GetLinksIpRaw()
{
	if(linksIpRaw.empty())
	{
		linksIpRaw.push_back(LinkValue(L">(\\d+\\.\\d+\\.\\d+\\.\\d+)<",L"https://whatismyipaddress.com/"));
	}

	return linksIpRaw;
}
	
/*!
	��ø����ļ�����

	\param updateItemFile �洢��������Ϣ��xml
	\return �����Ƿ��ȡ�ɹ�
*/
bool LinkHelper::GetLinkUpdate(const wstring& updateItemFile)
{
	if(FileHelper::CheckFileExist(updateItemFile))
	{
		//��ȡXML�ļ�
		tinyxml2::XMLDocument doc;
		FILE* fp = 0;
		errno_t err = _tfopen_s(&fp, updateItemFile.c_str(), L"rb");
		if (!fp || err)
		{
			return false;
		}
		doc.LoadFile(fp);
		fclose(fp);

		if(doc.Error())
		{
			return false;
		}
		
		linksFFmpegUpdate.clear();

		//��
		XMLElement *pRoot = doc.RootElement();
		if(pRoot)
		{
			XMLElement* ele = pRoot->FirstChildElement();
			while(ele)
			{
				const char* eName = ele->Name();

				const char* szValue = ele->Attribute("value");
				const char* szLink = ele->Attribute("link");
			
				if(eName && szValue && szLink)
				{
					wstring wStrValue = S_CA2W(SStringA(szValue),CP_UTF8);
					wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);

					if(eName == string("ffmpeg"))
						linksFFmpegUpdate.push_back(LinkValue(wStrValue, wStrLink));
				
					if(eName == string("ipSource"))
						linksIpUpdate.push_back(LinkValue(wStrValue, wStrLink));
				}

				//��һ�ֵܽ��
				ele = ele->NextSiblingElement();
			}
		}
		else
			return false;
	}

	return true;
}

//�ж��Ƿ�����˸���
bool LinkHelper::HasFinishedUpdate()
{
	if(!linksFFmpegUpdate.empty() || !linksIpUpdate.empty())
		return true;
	else
		return false;
}
