#include "stdafx.h"
#include "winfile.h"
#include "WinDialog.h"
#include "StringHelper.h"

#include <fstream> 
using namespace std;


/* �ļ��� */

File::File(LPCTSTR pathFile,LPCTSTR mode)
{
	m_lpszPathFile = NULL;
	m_lpszMode = NULL;
	m_pf = NULL;
	m_encodingType = ENCODING_TYPE::OTHER;
	char *szBufferTest = NULL;
	int nTestLen = 0;

	if(_tcscmp(mode,_T("r")) == 0)
	{
		//��ȡ�ļ���ǰ�����ֽڣ��жϱ���
		unsigned int len = 0;
		char *buf = NULL;
		byte firstByte = '\0';
		byte secondByte = '\0';
		ifstream in(pathFile);
		if(in)
		{
			in.seekg(0,ios::end);
			len = (unsigned int) in.tellg();
			buf = new char[len];
			in.seekg(0,ios::beg);
			in.read(buf,len);

			if(len < 2)//�ļ�����̫�٣���ȡʧ��
				return ;
			
			firstByte = buf[0];
			secondByte = buf[1];

			//��ȡ1000���ֽڣ��������ļ����ȣ���Ϊ�������� utf-8 ��Bom �� ascii �ṩ��������
			nTestLen = len < 1000? len : 1000;
			szBufferTest = new char[nTestLen];
			
			in.clear();				//clear �� seekg ����Ч
			in.seekg(0,ios::beg);
			in.read(szBufferTest,nTestLen);

			in.close();
			delete buf;
		}
		else return;//���ļ�ʧ��

		if(firstByte == 0xef && secondByte == 0xbb)
			m_encodingType = ENCODING_TYPE::UTF_8;
		else if(firstByte == 0xff && secondByte == 0xfe)
			m_encodingType = ENCODING_TYPE::UNICODE_LITTLE_ENDIAN;
		else if(firstByte == 0xfe && secondByte == 0xff)
			m_encodingType = ENCODING_TYPE::UNICODE_BIG_ENDIAN;
		else 
		{
			//�����Ƿ��� utf-8 ��bom��ʽ
			if(IsUTF8WithNoBom(szBufferTest, nTestLen))
				m_encodingType = ENCODING_TYPE::UTF_8_NO_BOM;
			else
				m_encodingType = ENCODING_TYPE::ASCII;	//Ĭ��ascii ����������룬��ascii ��ʽ����
		}

		if(szBufferTest)
			delete szBufferTest;

		//���ݲ�ͬ�ı��룬���ļ�
		if(m_encodingType == ENCODING_TYPE::UTF_8 || m_encodingType == ENCODING_TYPE::UTF_8_NO_BOM)
			m_pf = _tfopen(pathFile, _T("r,ccs=utf-8"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN)
			m_pf = _tfopen(pathFile, _T("r,ccs=UNICODE"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN)
		{	
			//UNICODE_BIG_ENDIAN ��֧��_tfopen(pathFile, _T("r,ccs=UNICODE")); ��ʽ��ȡ

			//ת��ΪUNICODE_LITTLE_ENDIAN�ı���Ȼ���ٶ�ȡ

			//ʹ��c��ʽ��ȡ�ļ�
			int len = 0;
			FILE *in = _tfopen(pathFile,_T("rb"));
			if(in)
			{
				fseek(in, 0, SEEK_END);
				len = ftell(in);
				buf = new char[len];
				fseek(in, 0, SEEK_SET);
				fread(buf, sizeof(char), len, in);

				WCHAR *pWC;
				pWC = reinterpret_cast<WCHAR*>(buf);

				int index = 0;
				while(index < len/2)  //��ÿһ�����ֽڽ����ֽ���
				{
					WCHAR temp = *pWC >> 8;
					temp |= *pWC << 8;
					*pWC = temp;
					pWC++;
					index++;
				}

				//��c����ʽд����ʱ�ļ�
				FILE* tFile = _tfopen(_T("temp_unicode_little_endian.txt"),_T("wb"));
				if(tFile)
				{
					fwrite(buf, sizeof(char), len, tFile);
					fclose(tFile);
				}
				else return;//д���ļ�ʧ��
			}
			else return;//���ļ�ʧ��
			
			m_pf = _tfopen(_T("temp_unicode_little_endian.txt"), _T("r,ccs=UNICODE"));
		}
		else //m_encodingType == ENCODING_TYPE::ASCII
			m_pf = _tfopen(pathFile,mode);

	}
	else //if(_tcscmp(mode,_T("r"))
		m_pf = _tfopen(pathFile,mode);


	m_lpszPathFile = pathFile;
	m_lpszMode = mode;
}

//���������Ƿ���UTF-8 ��Bom��ʽ 
bool File::IsUTF8WithNoBom(const void* pBuffer, long size)
{
	//�ο� http://blog.csdn.net/bladeandmaster88/article/details/54767487

	bool IsUTF8 = true;     
    unsigned char* start = (unsigned char*)pBuffer;     
    unsigned char* end = (unsigned char*)pBuffer + size;     
    while (start < end)     
    {     
        if (*start < 0x80) // (10000000): ֵС��0x80��ΪASCII�ַ�     
        {     
            start++;     
        }     
        else if (*start < (0xC0)) // (11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�     
        {     
            IsUTF8 = false;     
            break;     
        }     
        else if (*start < (0xE0)) // (11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�     
        {     
            if (start >= end - 1)      
                break;     
            if ((start[1] & (0xC0)) != 0x80)     
            {     
                IsUTF8 = false;     
                break;     
            }     
            start += 2;     
        }      
        else if (*start < (0xF0)) // (11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�     
        {     
            if (start >= end - 2)      
                break;     
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)     
            {     
                IsUTF8 = false;     
                break;     
            }     
            start += 3;     
        }      
        else    
        {     
            IsUTF8 = false;     
            break;     
        }     
    }     
    return IsUTF8;     
}



/* FileOperator */

bool FileOperator::ReadAllText(const string file, string& fileContent)
{
	ifstream in(file, ios_base::in);
	string temp = "";
	if (in)
	{
		while (!in.eof())
		{
			getline(in, temp);
			temp += "\n";
			fileContent += temp;
		}
		return true;
	}
	else
		return false;
}

bool FileOperator::ReadAllLines(const string file, OUT vector<string> *lines)
{
	ifstream in(file, ios_base::in);
	string temp = "";
	if (in)
	{
		while (!in.eof())
		{
			getline(in, temp);
			lines->push_back(temp);
		}
		return true;
	}
	else
		return false;
}

bool FileOperator::ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines)
{
	File encodingFile( file.c_str(), L"r");
	if(!encodingFile.isValidFile())
		return false;

	return ReadAllLinesW(encodingFile, lines);
}

bool FileOperator::ReadAllLinesW(File& encodingFile,  OUT vector<SStringW> *lines)
{
	if(!encodingFile.isValidFile())
		return false;
	
	if(encodingFile.m_encodingType == ENCODING_TYPE::UTF_8 || encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN 
		|| encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN || encodingFile.m_encodingType == ENCODING_TYPE::UTF_8_NO_BOM)
	{
		wchar_t line[MAX_WCHAR_COUNT_OF_LINE+1];
	
		// ���ļ��ж�ȡ��ʣ������ǿ��м��뵽 maker.m_vLyricOrigin ������
		while(fgetws(line,MAX_WCHAR_COUNT_OF_LINE,encodingFile.m_pf))
		{
			line[MAX_WCHAR_COUNT_OF_LINE]='\0';//��֤�����һ���ַ����ض��ַ���

			//�ɱ䳤�ַ�ת��Ϊ unicode ���ֽ��ַ�
			//MultiByteToWideChar(CP_ACP,MB_COMPOSITE,_line,strlen(_line)+1,line,MAX_WCHAR_COUNT_OF_LINE+1);

			SStringW aLine(line);
			aLine.Trim('\n');
			aLine.Trim(' ');
			aLine.Trim('\t');
			if(aLine.GetLength() ==0) 
				continue;

			lines->push_back(aLine);
		}
	}
	else //if(encodingFile.m_encodingType == ENCODING_TYPE::ASCII  �� ����
	{
		char _line[MAX_CHAR_COUNT_OF_LINE+1]; 
		wchar_t line[MAX_WCHAR_COUNT_OF_LINE+1];
	
		// ���ļ��ж�ȡ��ʣ������ǿ��м��뵽 maker.m_vLyricOrigin ������
		while(fgets(_line,MAX_CHAR_COUNT_OF_LINE,encodingFile.m_pf))
		{
			_line[MAX_CHAR_COUNT_OF_LINE]='\0';//��֤�����һ���ַ����ض��ַ���

			//�ɱ䳤�ַ�ת��Ϊ unicode ���ֽ��ַ�
			MultiByteToWideChar(CP_ACP,MB_COMPOSITE,_line,strlen(_line)+1,line,MAX_WCHAR_COUNT_OF_LINE+1);
			SStringT aLine(line);
			aLine.Trim('\n');
			aLine.Trim(' ');
			aLine.Trim('\t');
			if(aLine.GetLength() ==0) 
				continue;

			lines->push_back(aLine);
		}
	}

	return true;

}


bool FileOperator::SaveToFile(const string file, string& fileContent)
{
	ofstream out(file, ios_base::out);
	if (out)
	{
		out << fileContent;
		return true;
	}
	else
		return false;
}


bool FileOperator::WriteToUtf8File(const wstring file, wstring fileContent)
{
	vector<SStringW> lines;

	do{
		auto pos = fileContent.find_first_of(L"\n");
		if(pos != wstring::npos)
		{
			wstring line = fileContent.substr(0,pos+1);
			SStringW wline = line.c_str();
			lines.push_back(wline);

			fileContent = fileContent.substr(pos+1);
		}
		else
		{
			SStringW wline = fileContent.c_str();
			lines.push_back(wline);
			break;
		}
	}while(true);

	return WriteToUtf8File(file, lines);
}

bool FileOperator::WriteToUtf8File(const wstring file, vector<SStringW> lines)
{
	File outFile(file.c_str(),_T("w")); 

	if(!outFile.isValidFile())
		return false;

	//��д�� UTF-8 BOM ͷ
	fputc(0xef, outFile.m_pf);  
	fputc(0xbb, outFile.m_pf); 
	fputc(0xbf, outFile.m_pf); 

	string line;
	for(auto i=lines.begin(); i != lines.end(); i++)
	{
		//ȥ��һ�е��п��ܴ��ڵ�\r, ��Ϊ fputs ���Զ��� \n ǰ���\r�������ȥ���������βΪ \r\r\n
		SStringW _line = CStringHelper::Trim(*i,L" \t\r\n");
		_line += L"\n";

		int nRet = WideCharToMultiByte(CP_UTF8,0,_line,-1,NULL,0,NULL,NULL); 
		if(nRet>0)
		{
			char *pBuf=new char[nRet+1];
			int ret = WideCharToMultiByte(CP_UTF8,  0,_line,-1,pBuf,nRet,NULL,NULL);
			line = string(pBuf);
			if(ret == 0)
			{
				delete []pBuf;
				return false;
			}
			else
				delete []pBuf;
		}
		else
			return false;

		fputs(line.c_str(),outFile.m_pf);
	}

	return true;
}



/* FileHelper */
wstring FileHelper::GetCurrentDirectoryStr()
{
	wchar_t exeFullPath[MAX_PATH]; // Full path   

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);

	wstring strPath(exeFullPath);
	strPath = strPath.substr(0, strPath.find_last_of(L"\\")+1);
	return strPath;
}

bool FileHelper::CheckPathName(LPCTSTR format, LPCTSTR toChecked)
{
	int i;
	bool isFloder = false;
	//TODO���쳣�׳�����
	int len = _tcslen(format);
	if (_tcscmp(format, _T("..")) == 0)
	{
		isFloder = true;
	}
	else if (len < 3 || format[0] != _T('*') || format[1] != _T('.'))
		return false;  //TODO���쳣


	//��ȡ����� ������·���ַ��� toChecked ����Ϣ
	TCHAR pathName[_MAX_PATH];
	TCHAR ext[_MAX_EXT];

	int lenPathName = 0, pos = -1;

	_tcscpy(pathName, toChecked);
	lenPathName = _tcslen(pathName);	//�õ�·���ܳ�
	if (!lenPathName)
		return false;

	//�õ�·�������һ����.����λ������pos��
	for (i = 0; i< lenPathName; i++)
	{
		if (_T('.') == pathName[i])
			pos = i;
	}

	if (isFloder) //����ļ�������
	{
		return IsDirectory(pathName);
	}
	else //�����ͨ��׺������
	{
		_tcscpy(ext, &pathName[pos + 1]);  //�õ�·���ĺ�׺����������.����
		if (_tcscmp(&format[2], ext) == 0)	//�� �����ṩ�ĺ�׺�Ա�
			return true;
		else
			return false;
	}
}

bool FileHelper::IsDirectory(SStringW path)
{
	//����panelFloder�Ƿ����
	WIN32_FIND_DATA  FindFileData;
	bool bValue = false;
	HANDLE hFind = FindFirstFile(path, &FindFileData);
	if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValue = true;
	}
	FindClose(hFind);

	return bValue;
}

bool  FileHelper::CheckFileExist(const wstring &strPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(strPath.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

bool  FileHelper::CheckFolderExist(const wstring &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);

	return rValue;
}

void FileHelper::SplitPath(const wstring& strPathName, OUT wstring *pstrDrive, OUT wstring *pstrDirectory, OUT wstring* pstrName, OUT wstring* pstrExt)
{
	wstring _strDrive = L"";
	wstring _strDirectory = L"";
	wstring _strName = L"";
	wstring _strExt = L"";

	//�ֺ�λ��
	auto posColon = strPathName.find_first_of(L':');
	
	if (posColon != string::npos)
		_strDrive = strPathName.substr(0, posColon);

	//���һ���ָ�����λ��
	auto posLastSep = strPathName.find_last_of(L"/\\");
	if (posLastSep != string::npos)
	{
		_strDirectory = strPathName.substr(0, posLastSep +1);

		posLastSep++;
	}
	else
		posLastSep = 0;

	//�ַ�����β
	auto posEnd = strPathName.size();

	//Ѱ����չ����"."λ��
	auto posDot = strPathName.find_last_of(L'.');
	if (posDot != string::npos)
	{
		//���⴦��·���к��� . ����������� ..\\test, ����ֻ�е� ��õġ�.��λ�ô���  posLastSep ʱ�� ��õġ�.��λ�ò�����չ����"."λ��
		if (posDot > posLastSep)
			_strExt = strPathName.substr(posDot, posEnd - posDot);
		else
			posDot = posEnd;
	}
	else
		posDot = posEnd;


	_strName = strPathName.substr(posLastSep, posDot - posLastSep);

	if (pstrDrive != nullptr)
		*pstrDrive = _strDrive;
	if (pstrDirectory != nullptr)
		*pstrDirectory = _strDirectory;
	if (pstrName != nullptr)
		*pstrName = _strName;
	if (pstrExt != nullptr)
		*pstrExt = _strExt;
}

