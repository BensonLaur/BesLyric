#include "stdAfx.h"
#include "SplitFile.h"
#include "WinFile.h"
#include "FileHelper.h"

bool CSplitFile::SplitFile(wstring strSrcFile, wstring strToPath, wstring strToName, unsigned int nBlockSize)
{
	if(!::FileHelper::CheckFileExist(strSrcFile))
		return false;
	
	if(!::FileHelper::CheckFolderExist(strToPath))
		return false;

	FILE *pFile = NULL;
	
	wstring strSrcExt;
	FileHelper::SplitPath(strSrcFile, NULL,NULL, NULL, &strSrcExt);

	wstring strTargetFileNamePre =  strToPath + L"\\" + strToName;
	
	errno_t ret = _wfopen_s(& pFile, strSrcFile.c_str(), L"rb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;

	//���ԭ�ļ���С
	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
	fclose(pFile);										//��ȡ�ļ���С

	//���ļ���׺д�� strTargetFileNamePre.ext ��
	pFile = NULL;
	ret = _wfopen_s(& pFile, (strTargetFileNamePre+L".ext" + SERVER_FILE_EXTENTION_W).c_str(), L"w");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;

	_fputts( (strSrcExt+L"\n").c_str(), pFile);

	//д���ļ���С
	WCHAR szBuffer[MAX_BUFFER_SIZE/2];
	_fputts(_itow(nFileSize,szBuffer,10), pFile);						//д���ļ�

	fclose(pFile);

	
	//���ָ���ļ���ֱ�д�� strTargetFileNamePre[1|2|.]��
	pFile = NULL;
	ret = _wfopen_s(& pFile, strSrcFile.c_str(), L"rb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;
	
	string strTargetFileNamePreA = S_CW2A(SStringW(strTargetFileNamePre.c_str()));

	char* pBlockData;
	pBlockData = new char[nBlockSize];

	int nCount = 1;
	int nSize ;
	while( (nSize = fread(pBlockData, sizeof(char),nBlockSize,  pFile)))
	{
		char szBuffer[MAX_BUFFER_SIZE];
		string strFileNameA = strTargetFileNamePreA+ "." + itoa(nCount++, szBuffer, 10) + SERVER_FILE_EXTENTION_A;
		
		FILE *pFileTarget;
		ret = fopen_s(& pFileTarget, strFileNameA.c_str(), "wb");
		
		if(ret!= RET_SUCCEEDED || pFileTarget == NULL)
			return false;

		fwrite( pBlockData,sizeof(char), nSize, pFileTarget);
		fclose(pFileTarget);
	}

	delete pBlockData;

	fclose(pFile);
	return true;
}

bool CSplitFile::MergeFile(wstring strSplitFilePath, wstring strFileName, wstring strMergeToPath)
{
	if(!::FileHelper::CheckFolderExist(strSplitFilePath))
		return false;
	
	if(!::FileHelper::CheckFolderExist(strMergeToPath))
		return false;

	int nMaxCount = 0;

	if(false == GetMaxSplitedCount(strSplitFilePath, strFileName, nMaxCount)) //����ļ����ָ�Ŀ���
		return false;

	if(nMaxCount == 0) 
		return false; //û�д����ļ�

	//��úϲ�����ļ�����չ��,�� �ϲ����ļ���С
	WCHAR szExt[MAX_BUFFER_SIZE/2];
	int nSizeOfFileRecord = 0;

	wstring strMergedFile = strMergeToPath +L"\\"+ strFileName +L".ext" + SERVER_FILE_EXTENTION_W;
	if(false == GetMergedFileExtAndSize(strMergedFile, szExt, MAX_BUFFER_SIZE/2, nSizeOfFileRecord))
		return false;

	//�ֱ��ȡ�������ָ���ļ�����ϳ�һ���ļ�
	int nCumulateSize = 0;
	wstring strSplitPathNamePre = strSplitFilePath +L"\\"+ strFileName;
	wstring strMergedFilePathName = strMergeToPath +L"\\"+ strFileName + szExt;

	if(!MergeToOneFile(strMergedFilePathName,strSplitPathNamePre , nMaxCount, nCumulateSize))
		return false;

	if(nCumulateSize != nSizeOfFileRecord)
		return false;

	return true;
}


bool CSplitFile::GetMaxSplitedCount(wstring strSplitFilePath,wstring strFileName, int &nMaxCount)
{
	//���Ŀ¼�����б��ָ���ļ��� �� strFileName Ϊǰ׺
	WIN32_FIND_DATAW FindFileData;
	WCHAR szFind[MAX_BUFFER_SIZE /2];
	wcscpy_s(szFind, strSplitFilePath.c_str());
	wcscat_s(szFind, L"\\*.*");//���ǵ�����

	HANDLE hFind = ::FindFirstFileW(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		return false;
	
	vector<wstring> vecFileNames;
	for (;;) //��������ļ���
	{
		vecFileNames.push_back(FindFileData.cFileName);

		if(!FindNextFileW(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);

	nMaxCount = 0;
	for(auto iterName = vecFileNames.begin(); iterName != vecFileNames.end();iterName++ )
	{
		wstring strName,strExt;
		FileHelper::SplitPath(*iterName, NULL,NULL, &strName, &strExt); 

		if(strExt == SERVER_FILE_EXTENTION_W) //�Ƿ������ļ���չ����Ϊ����Ҫ�õ����ļ�
		{
			//strName Ϊȥ����������׺���ַ���
			FileHelper::SplitPath( strName, NULL,NULL, &strName, &strExt);  //�ٷָ�һ�飬�õ��ĺ�׺ Ϊ .ext �� .1 - .n

			if(strName == strFileName)//������Ҫ������ļ���
			{
				if(strExt.size() < 2)
					return false;
				else //���� "."���ٻ���2���ַ� 
				{
					strExt = strExt.substr(1); 
					int nCount = _wtoi(strExt.c_str()); 
					if(nCount > nMaxCount )
						nMaxCount = nCount;		//��������ļ�����
				}
			}
		}
	}

	return true;
}

bool CSplitFile::GetMergedFileExtAndSize(wstring strMergedFile, WCHAR* szExt, int nSize,int &nSizeOfFileRecord)
{
	FILE *pf = NULL;
	errno_t ret = _wfopen_s(& pf, strMergedFile.c_str(), L"r");
	
	if(ret!= RET_SUCCEEDED || pf == NULL)
		return false;

	WCHAR szFileSize[MAX_BUFFER_SIZE/2];
	_fgetts( szExt, MAX_BUFFER_SIZE/2, pf); //fget���ȡ���з������º���·������ʧ��
	_fgetts( szFileSize, MAX_BUFFER_SIZE/2, pf);

	for(auto i = 0; szExt[i] != L'\0'; i++)
		if(szExt[i] == L'\n')
			szExt[i] = L'\0';

	nSizeOfFileRecord = _wtoi(szFileSize);
	fclose(pf);

	return true;
}

bool CSplitFile::MergeToOneFile(wstring strMergedFilePathName,wstring  strTargetFileNamePreW, int nMaxCount, int &nCumulateSize)
{
	FILE *pFile = NULL;
	errno_t ret = _wfopen_s(&pFile, strMergedFilePathName.c_str(), L"wb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;
	
	string strTargetFileNamePreA = S_CW2A(SStringW(strTargetFileNamePreW.c_str()));

	for(int i=1; i <= nMaxCount; i++)
	{
		//
		char szBuffer[MAX_BUFFER_SIZE];
		string strFileNameA = strTargetFileNamePreA+ "." + itoa(i, szBuffer, 10) + SERVER_FILE_EXTENTION_A;
		
		FILE *pFileSplited;
		ret = fopen_s(& pFileSplited, strFileNameA.c_str(), "rb");
		
		if(ret!= RET_SUCCEEDED || pFileSplited == NULL)
			return false;

		//�õ��������ݿ�����

		fseek(pFileSplited, 0, SEEK_END);
		int nFileSize = ftell(pFileSplited);										//��ȡ�ļ���С
		rewind(pFileSplited);
		
		char* pBlockData = new char[nFileSize];			//���������ļ��Ĵ�С����ȡ�ļ����ݿ��ָ��
		if (pBlockData == NULL)
			return false;

		if (fread(pBlockData, 1, nFileSize, pFileSplited) != nFileSize)		//�ļ�������m_pFileData��
			return false;

		fclose(pFileSplited);

		//���ֿ������д���ļ�
		fwrite(pBlockData,sizeof(char), nFileSize, pFile);
		
		nCumulateSize += nFileSize; //�ۻ��ļ���С
	}
	
	fclose(pFile);

	return true;
}


