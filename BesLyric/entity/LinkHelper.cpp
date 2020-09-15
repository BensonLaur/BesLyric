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
	//�ӷ�������ȡ��������
	vector<wstring> candidates;
	candidates.push_back(L"https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml");
	candidates.push_back(L"http://bensonlaur.com/files/beslyric/Beslyric-Link.xml");
	
	UpdateHelper updateHelper;
	wstring etcDir = updateHelper.GetEtcDir();
	if(!FileHelper::IsFloderExistW(etcDir))
		_wmkdir(etcDir.c_str());

	wstring xml = updateHelper.GetEtcDir()+ L"dynamicLink.xml";
	bool getValidUpdate = false;
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

	return getValidUpdate;
}

/*
	������е� ffmpeg �������ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
*/
vector<NamedLink> LinkHelper::GetAllLinksFFmpeg()
{
	vector<NamedLink> links = GetLinksFFmpegRaw();
	links.insert(links.end(),linksFFmpegUpdate.begin(), linksFFmpegUpdate.end());

	//�����㷨 
	//srand����Ҫ����#include<ctime>ͷ�ļ� 
	srand((unsigned int)time(0));
	random_shuffle(links.begin(),links.end());

	return links;
}

/*
	��ó����Դ��� ffmpeg ��������
*/
vector<NamedLink> LinkHelper::GetLinksFFmpegRaw()
{
	if(linksFFmpegRaw.empty())
	{
		linksFFmpegRaw.push_back(NamedLink(L"gitlab",L"https://gitlab.com/BensonLaur/resource-provider-gitlab/-/raw/master/beslyric/ffmpeg-3.4.1.exe"));
		linksFFmpegRaw.push_back(NamedLink(L"bitbucket",L"https://bitbucket.org/bensonlaur/resource-provider-bitbucket/raw/bcd1a44f30893427a5f78243e4548f9afbb82c9c/beslyric/ffmpeg-3.4.1.exe"));
	}

	return linksFFmpegRaw;
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
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			const char* eName = ele->Name();

			const char* szName = ele->Attribute("name");
			const char* szLink = ele->Attribute("link");
			
			if(eName && szName && szLink)
			{
				wstring wStrName = S_CA2W(SStringA(szName),CP_UTF8);
				wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);

				if(eName == string("ffmpeg"))
					linksFFmpegUpdate.push_back(NamedLink(wStrName, wStrLink));
			}

			//��һ�ֵܽ��
			ele = ele->NextSiblingElement();
		}
	}

	return true;
}
