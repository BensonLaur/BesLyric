#pragma once
#include "../Define.h"
#include "../utility/SSingleton.h"

#include <string>
#include <vector>
using namespace std;


class NamedLink{
public:
	NamedLink( const wstring& _name,const wstring& _link):link(_link),name(_name){}
public:
	wstring link;
	wstring name;
};

//!���Ӹ�����
class LinkHelper: public Singleton<LinkHelper>{

public:	
	/*!
		�ӷ�������������

		\return �����Ƿ��ȡ�ɹ�
	*/
	bool UpdateLinkFromServer();

	/*
		������е� ffmpeg �������ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
	*/
	vector<NamedLink> GetAllLinksFFmpeg();

private:
	/*
		��ó����Դ��� ffmpeg ��������
	*/
	vector<NamedLink> GetLinksFFmpegRaw();

	/*!
		����ļ��Ƿ�Ϊ��Ч�� xml �ļ�

		\param filePath �������ļ�
		\return �����ļ��Ƿ�Ϊ��Ч�� xml �ļ�
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*!
		��ø��µ���������

		\param updateItemFile �洢��������Ϣ��xml
		\return �����Ƿ��ȡ�ɹ�
	*/
	bool GetLinkUpdate(const wstring& updateItemFile);

private:
	vector<NamedLink> linksFFmpegRaw;

	vector<NamedLink> linksFFmpegUpdate;
};