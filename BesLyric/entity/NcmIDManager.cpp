
#include "stdafx.h"
#include "NcmIDManager.h"
#include "..\utility\WinFile.h"
#include "..\lib\xml\tinyxml2.h"
#include "..\utility\Downloader.h"
using namespace tinyxml2;

SINGLETON_C_D_0(CNcmIDManager)

	
static const wstring XML_PAIR = L"pair";			//ncm id ��
static const wstring XML_FILE_NAME = L"fileName";	//�ļ���
static const wstring XML_ID = L"id";				//ID

//�������ݶ�
bool CNcmIDManager::SaveDataPairs()
{
	wstring xmlStr = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += L"<ncmid>\n";

	for(auto iter= m_mapNcmID.begin(); iter != m_mapNcmID.end(); iter++)
		xmlStr += L"\t<"+ XML_PAIR +L" " + XML_FILE_NAME +L"=\""+	iter->first +L"\" " + XML_ID +L"=\""+	iter->second +L"\""+  L" />\n";
	
	xmlStr += L"</ncmid>\n";
	
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" +NCM_ID_FILE_NAME;
	
	return FileOperator::WriteToUtf8File( wstrPath,xmlStr);
}

//�������ݶ�
bool CNcmIDManager::LoadDataPairs()
{
	//���ļ���������
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + NCM_ID_FILE_NAME;
	string strPath = S_CW2A(SStringW(wstrPath.c_str()));
	if(FileHelper::CheckFileExist(wstrPath))
	{
		//��ȡXML�ļ�
		tinyxml2::XMLDocument doc;
		doc.LoadFile(strPath.c_str());
		if(doc.Error())
		{
			return false;
		}

		m_mapNcmID.clear();


		//��
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			const char* szName = ele->Name();
			const char* szfileName = ele->Attribute(S_CW2A(SStringW(XML_FILE_NAME.c_str()).GetBuffer(1)));
			const char* szID = ele->Attribute(S_CW2A(SStringW(XML_ID.c_str()).GetBuffer(1)));
			
			wstring wStrName = S_CA2W(SStringA(szfileName));
			wstring wStrID = S_CA2W(SStringA(szID));
			
			WCHAR wszName[MAX_BUFFER_SIZE] = {0};
			WCHAR wszID[MAX_BUFFER_SIZE] = {0};

			//�����ļ���������utf-8���棬��Ҫת��
			::MultiByteToWideChar(CP_UTF8,0,szfileName, MAX_BUFFER_SIZE/2, wszName, MAX_BUFFER_SIZE);
			//�����ļ���������utf-8���棬��Ҫת��
			::MultiByteToWideChar(CP_UTF8,0,szID, MAX_BUFFER_SIZE/2, wszID, MAX_BUFFER_SIZE);
			
			pair<wstring, wstring> p;
			p.first = wszName;
			p.second = wszID;
			m_mapNcmID.insert(p);

			//��һ�ֵܽ��
			ele = ele->NextSiblingElement();
		}
	}
	else
		SaveDataPairs();

	//��������Ŀ���У���ʱ�򣬶����ݽ��м򵥵�У�飬���´���
	map< wstring, wstring>	mapNcmIDTemp;
	for(auto iter = m_mapNcmID.begin(); iter != m_mapNcmID.end(); iter++)
	{
		bool bValid = false;
		if(!CheckIDValidity( iter->second, bValid))
			return true;//��������ʧ���ˣ��������

		if(bValid)
		{
			mapNcmIDTemp.insert(*iter);
		}
	}

	if(mapNcmIDTemp.size() != m_mapNcmID.size())
	{
		//�Ѿ�ɸ����һЩ�����´洢
		m_mapNcmID = mapNcmIDTemp;
		SaveDataPairs();
	}

	return true;
}

//����ID
bool CNcmIDManager::FindID( wstring fileName,OUT wstring& id)
{
	auto iter = m_mapNcmID.find(fileName);
	if(iter != m_mapNcmID.end())
	{
		id = iter->second;

		return true;
	}
	else
		return false;
}

//����ID��
void CNcmIDManager::InsertNcmIDPair( wstring fileName, wstring id)
{
	wstring tempID;
	if(FindID(fileName,tempID))
	{
		if(tempID != id)
			UpdateID(fileName, id);
	}
	else
	{
		pair<wstring,wstring> p(fileName, id);
		m_mapNcmID.insert(p);
	}

}


//���ID�Ƿ���Ч
//���� false Ϊ��ѯʧ�ܣ���������ȣ�
bool CNcmIDManager::CheckIDValidity(wstring id, OUT bool& bValid)
{
	wstring strContent = L"";
	wstring strLink = L"http://music.163.com/song/media/outer/url?id="+ id +L".mp3";
	if(!CDownloader::DownloadString( strLink, strContent, 500))
	{
		return false;
	}
	else
	{
		auto index = strContent.find(L"<!DOCTYPE html>");  //������2����ҳ���ص�html��ǩ
		auto index2 = strContent.find(L"<html>");
		if(index != wstring::npos || index2 != wstring::npos ) //���ش��а�����ҳ��ر�ǩ����Ϊ���ظ���ʧ�ܣ���ΪID��Ч
			bValid = false;
		else
			bValid = true;

		return true;
	}
}


//����ID
bool CNcmIDManager::UpdateID( wstring fileName, wstring id)
{
	auto iter = m_mapNcmID.find(fileName);
	if(iter != m_mapNcmID.end())
	{
		iter->second = id;

		return true;
	}
	else
		return false;
}
