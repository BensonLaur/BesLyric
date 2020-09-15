#include "stdafx.h"
#include "UpdateHelper.h"

#include "../utility/WinFile.h"

#include "..\lib\md5\md5.h"
#include "..\lib\xml\tinyxml2.h"
using namespace tinyxml2;

/*!
	����ļ��Ƿ�Ϊ��Ч�� xml �ļ�

	\param filePath �������ļ�
	\return �����ļ��Ƿ�Ϊ��Ч�� xml �ļ�
*/
bool UpdateHelper::IsValidXml(const std::wstring& filePath)
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
bool UpdateHelper::GetUpdateItem(const wstring& updateItemFile, vector<UpdateItem>& updateItems)
{
	updateItems.clear();

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

		//��
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			UpdateItem item;
			const char* szName = ele->Attribute("name");
			const char* szLink = ele->Attribute("link");
			const char* szLocal = ele->Attribute("local");
			const char* szMd5 = ele->Attribute("md5");
			
			wstring wStrName = S_CA2W(SStringA(szName),CP_UTF8);
			wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);
			wstring wStrLocal = S_CA2W(SStringA(szLocal),CP_UTF8);
			string strMd5 = szMd5;

			item.fileName = wStrName;
			item.link = wStrLink;
			item.local = wStrLocal; 
			item.md5 = strMd5;

			updateItems.push_back(item);

			//��һ�ֵܽ��
			ele = ele->NextSiblingElement();
		}
	}



	return true;
}

//���������Լ죬�����ǰ���е����ݺ͸���xml��ͬ��ɾ������xml�ļ�
void UpdateHelper::UpdateItemSelfCheck()
{
	vector<UpdateItem> updateItems;
	if(!GetUpdateItem(GetEtcDir() + L"update", updateItems))			
	{
		return;
	}

	//����ļ����
	bool deleteUpdateXml = false;
	for(auto iter = updateItems.begin(); iter != updateItems.end(); iter++)
	{
		wstring taget = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
		
		//���㱾��md5 
		string md5;
		GetFileMd5(taget, md5);

		if(md5 != iter->md5)
		{
			deleteUpdateXml = true;
			break;
		}
	}

	if(deleteUpdateXml)
		DeleteFile((GetEtcDir() + L"update").c_str());
}

//�������Ŀ¼�µ� etc/ Ŀ¼
wstring UpdateHelper::GetEtcDir()
{
	return FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\";
}

//ȷ�����·��Ŀ¼����
bool UpdateHelper::MakeSureRelativeLocalExist(const wstring& basePath, const wstring& relativePath)
{
	wstring dir = basePath + relativePath;

	return FileHelper::MakeSureTargetDirExist(dir);
}

//��ȡ�ļ��� md5 ��
bool UpdateHelper::GetFileMd5(const wstring& filePath, string& strMd5)
{
	//�������ļ��򷵻�
	if(!FileHelper::CheckFileExist(filePath))
		return false;

	MD5_CTX md5 = MD5_CTX();
    char md5Str[33];
    md5.GetFileMd5(md5Str, filePath.c_str());

	strMd5 = md5Str;

	return true;
}


//�ж��ļ�md5�Ƿ�����Ч�� ffmpeg �� md5
bool UpdateHelper::IsValidFFmpegMd5(const string& strMd5)
{
	// 2.3.0 ����°汾 ffmpeg Ϊ���� xp �� ffmpeg,Ϊ�˾��û�����Ӱ�죨�������أ�
	//���ж��Ƿ��Ǿɵ� ffmpeg �� MD5, ���ж��µ� ffmpeg �� MD5

	const string MD5_FFMPEG_old = "949ed6af96c53ba9e1477ded35281db5";
	const string MD5_FFMPEG_new = "979fc6e7ff2c74108353c114bf86e2bb";

	if( MD5_FFMPEG_old == strMd5)
		return true;

	return MD5_FFMPEG_new == strMd5;
}
