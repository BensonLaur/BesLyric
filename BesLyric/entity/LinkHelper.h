#pragma once
#include <Windows.h>
#include "../Define.h"
#include "../utility/SSingleton.h"
#include "helper\SCriticalSection.h"

#include <string>
#include <vector>
using namespace std;


class LinkValue{
public:
	LinkValue( const wstring& _value,const wstring& _link):link(_link),value(_value){}
public:
	wstring link;
	wstring value;
};

//!���Ӹ�����
class LinkHelper: public Singleton<LinkHelper>, public SCriticalSection{

public:	
	/*!
		�ӷ�������������

		\return �����Ƿ��ȡ�ɹ�
	*/
	bool UpdateLinkFromServer();

	/*
		������е� ffmpeg �������ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
	*/
	vector<LinkValue> GetAllLinksFFmpeg();
	
	/*
		������е� ip ��ȡ���ӣ�Ϊ�˼��ٵ�һ���������ɣ����������˳��
	*/
	vector<LinkValue> GetAllLinksIp();

private:
	
	/*!
		����ļ��Ƿ�Ϊ��Ч�� xml �ļ�

		\param filePath �������ļ�
		\return �����ļ��Ƿ�Ϊ��Ч�� xml �ļ�
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*
		��ó����Դ��� ffmpeg ��������
	*/
	vector<LinkValue> GetLinksFFmpegRaw();
	
	/*
		��ó����Դ��� ip ���ػ�ȡ
	*/
	vector<LinkValue> GetLinksIpRaw();
	
	/*!
		��ø��µ���������

		\param updateItemFile �洢��������Ϣ��xml
		\return �����Ƿ��ȡ�ɹ�
	*/
	bool GetLinkUpdate(const wstring& updateItemFile);

	//�ж��Ƿ�����˸���
	bool HasFinishedUpdate();
	
private:
	vector<LinkValue> linksFFmpegRaw;
	vector<LinkValue> linksFFmpegUpdate;
	
	vector<LinkValue> linksIpRaw;
	vector<LinkValue> linksIpUpdate;
};