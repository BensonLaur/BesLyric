#include "stdafx.h"
#include "StringHelper.h"
#include <vector>
using namespace std;

//ȥ��ǰ���ַ�
SStringW CStringHelper::Trim(SStringW strToTrim, SStringW strCharactorRemoved)
{
	int first_fit;
	int last_fit;
	bool bHasFindFirst;

	first_fit = last_fit = 0;
	bHasFindFirst = false;

	for(int i = 0; i != strToTrim.GetLength(); i++)
	{
		if(!bHasFindFirst)
		{
			if(!IsCharInString(strToTrim.GetAt(i),strCharactorRemoved) )
			{
				first_fit = i;
				bHasFindFirst = true;
			}
		}
		else 
		{
			if(!IsCharInString(strToTrim.GetAt(i),strCharactorRemoved))
			{
				last_fit = i;
			}
		}
	}

	if(!bHasFindFirst && last_fit == 0)  //û���κ�ƥ���ַ������
		return L"";

	if(bHasFindFirst && last_fit == 0)	//ֻ�����1���ַ��Ƿ��ϵ������last_fit Ϊ0 ����Ҫ����
		last_fit = strToTrim.GetLength() - 1;

	strToTrim = strToTrim.Left( last_fit + 1).Right( last_fit + 1 - first_fit);

	return strToTrim;
}


wstring CStringHelper::Trim(wstring strToTrim, wstring strCharactorRemoved)
{
	SStringW strToTrim2 = SStringW(strToTrim.c_str());
	SStringW strCharactorRemoved2 = SStringW(strCharactorRemoved.c_str());
	SStringW strReturn = Trim(strToTrim2, strCharactorRemoved2);
	return wstring(strReturn.GetBuffer(1));
}

//���Trim֮��ĳ���
int CStringHelper::GetTrimLength(SStringW strToTrim, SStringW strCharactorRemoved)
{
	return Trim(strToTrim,strCharactorRemoved).GetLength();
}

int CStringHelper::GetTrimLength(wstring strToTrim, wstring strCharactorRemoved)
{
	return Trim(strToTrim,strCharactorRemoved).size();
}


//���ַ����ָ����
void CStringHelper::SplitStringByToLines(SStringW strToSplted,WCHAR splitChar, vector<SStringW>& vecLines)
{
	int pos = 0;
	
	do{
		pos = strToSplted.Find( splitChar);
		if(pos != -1)
		{
			vecLines.push_back(strToSplted.Left(pos+1));
			strToSplted = strToSplted.Right( strToSplted.GetLength() - pos -1);
		}
		else
		{
			vecLines.push_back(strToSplted);
			break;
		}

	}while(true);
}


//�ַ��Ƿ����ַ����ڳ���
bool CStringHelper::IsCharInString(WCHAR ch, SStringW str)
{
	for(int i=0; i<str.GetLength(); i++)
	{
		if(ch == str[i])
			return true;
	}

	return false;
}