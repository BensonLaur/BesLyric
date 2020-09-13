#pragma once
#include "../Define.h"

#include <string>
#include <vector>
using namespace std;

//!���¸�����
class UpdateHelper{
public:
	/*!
		����ļ��Ƿ�Ϊ��Ч�� xml �ļ�

		\param filePath �������ļ�
		\return �����ļ��Ƿ�Ϊ��Ч�� xml �ļ�
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*!
		��ø����ļ�����

		\param updateItemFile �洢��������Ϣ��xml
		\return �����Ƿ��ȡ�ɹ�
	*/
	bool GetUpdateItem(const wstring& updateItemFile, vector<UpdateItem>& updateItems);

	//���������Լ죬�����ǰ���е����ݺ͸���xml��ͬ��ɾ������xml�ļ�
	void UpdateItemSelfCheck();

	//�������Ŀ¼�µ� etc/ Ŀ¼
	wstring GetEtcDir();

	//ȷ�����·��Ŀ¼����
	bool MakeSureRelativeLocalExist(const wstring& basePath, const wstring& relativePath);
	
	//��ȡ�ļ��� md5 ��
	static bool GetFileMd5(wstring filePath, string& strMd5);
};