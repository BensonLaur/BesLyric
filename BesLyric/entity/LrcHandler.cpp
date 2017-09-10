// LyricHandler.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LrcHandler.h"
#include "../utility/WinDialog.h"
#include "../utility/WinFile.h"
#include "../utility/StringHelper.h"
#include <algorithm>
#include <ctype.h>


TimeLineInfo::TimeLineInfo(SStringT timeLine)
{
	//��ʼ����Ļ�����Ա����Ϣ
	m_strTimeLine = timeLine;
	int pos = m_strTimeLine.Find(_T(']'));
	SStringT timeStr = m_strTimeLine.Left(pos+1);

	m_strLine = m_strTimeLine.Right(m_strTimeLine.GetLength()-pos-1);
	m_nmSesonds = TimeStringToMSecond(timeStr,timeStr.GetLength());

	if(m_strLine.GetLength()==0)
		m_bIsEmptyLine = true;
	else
		m_bIsEmptyLine = false;
}


//��ʱ���ǩ�ַ����õ���Ӧ�ĺ���ʱ��
int TimeLineInfo::TimeStringToMSecond(LPCTSTR timeStr, int length)
{
	//TODO���쳣�׳�����

	TCHAR szMinute[5];	//����
	TCHAR szSecond[10];	//��

	int i,j;
	//�õ�: ��λ��
	int pos1 = -1,pos3 = length-1;
	for(i=0; i<length; i++)
	{
		if(_T(':') == timeStr[i])
			pos1 = i;
	}

	//�õ�����ʱ��ε��ַ���
	for(j=0,i=1; i < pos1; i++,j++)
		szMinute[j] = timeStr[i];
	szMinute[j] = _T('\0');

	for(j=0, i = pos1+1; i<pos3; i++, j++)
		szSecond[j] = timeStr[i];
	szSecond[j] = _T('\0');

	int millisecond = _wtoi(szMinute)  * 60000 + (int)(_wtof(szSecond) * 1000 );
	return millisecond;
}

//�����޷���ʮ���ƴ���Ӧ�����֣�ʮ���ƴ������� 023��12��04 ����ʽ����ֵΪ0��999��
int TimeLineInfo::DecStrToDecimal(LPCTSTR timeStr)
{
	int bit = _tcslen(timeStr);
	int result = 0;
	for(int i=0; i< bit; i++)
	{
		result *= 10;
		result += timeStr[i]-_T('0');
	}
	return result;
}


//ʹ�ö�ȡ�ĵ��ļ��������г�ʼ��Lrc������
LrcProcessor::LrcProcessor(vector<SStringW> vecLines, bool bDealOffset)
{
	ProcessData(vecLines, bDealOffset);
}


//ʹ������LRC�ı����ݳ�ʼ��Lrc������
LrcProcessor::LrcProcessor(SStringW AllContent, bool bDealOffset)
{
	//��AllContent �ָ����
	vector<SStringW> vecLines;
	CStringHelper::SplitStringByToLines( AllContent, L'\n', vecLines);
	
	ProcessData(vecLines, bDealOffset);
}

//��ǰ����ļ��Ƿ���Ч
bool LrcProcessor::IsLrcFileValid()
{
	return m_bIsLrcFileValid;
}

//�������������֧�ֵĸ�ʽ
vector<TimeLineInfo> LrcProcessor::GetNeteaseLrc()
{
	return m_vecNeteaseLrc;
}

//����Lrc�ļ�
bool LrcProcessor::GenerateNeteaseLrcFile(SStringW strFilePath)
{
	vector<SStringW> vecLines;

	WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
	{
		int ms = iter->m_nmSesonds;

		szTimeBuf[0] = L'\0';
		int minutes = ms/60000;
		ms = ms%60000;
		double seconds = ms*1.0/1000;
		_stprintf(szTimeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);

		SStringW newLine = szTimeBuf;
		newLine.Append( iter->m_strLine );
		newLine.Append(L"\n");

		vecLines.push_back(newLine);
	}

	return FileOperator::WriteToUtf8File( strFilePath.GetBuffer(1), vecLines);
}


//���� vector ������
void LrcProcessor::ProcessData(const vector<SStringW>& vecLines, bool bDealOffset)
{
	m_bDealOffset = bDealOffset;
	m_strTitle = L"";
	m_strArtist = L"";
	m_strAlbum = L"";
	m_strEditor = L"";
	m_nOffset = 0;

	m_bIsLrcFileValid = true;

	//��������������
	for(auto iter = vecLines.begin(); iter != vecLines.end(); iter++)
	{
		if( false == DealOneLine( *iter ) )
		{
			m_bIsLrcFileValid = false;	
			return;
		}
	}

	//�����и�ʽ�������
	sort(m_vecNeteaseLrc.begin(), m_vecNeteaseLrc.end());

	//ȥ���ظ��Ŀ��У��������ֶ������ʱ��������һ�����е�ʱ��
	bool isLastLineEmpty = false;
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); )
	{
		if(iter->m_strLine.GetLength() == 0)
		{
			if(isLastLineEmpty)
			{
				iter = m_vecNeteaseLrc.erase(iter);
			}
			else
			{
				iter++;
				isLastLineEmpty = true;
			}
		}
		else
		{
			iter++;
			isLastLineEmpty = false;
		}
	}

	//�������ʱ�����ƫ��
	if(m_bDealOffset)
	{
		for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
		{
			iter->m_nmSesonds = iter->m_nmSesonds - m_nOffset < 0 ? 0 : iter->m_nmSesonds - m_nOffset;
		}
	}
}

//����1��
bool LrcProcessor::DealOneLine(const SStringW& _strLine)
{
	SStringW strLine = _strLine;
	int nPos1 = -1;
	int nPos2 = -1;
	if( -1 != strLine.Find(L"[ti:"))//����
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strTitle = strLine.Left(nPos2);
		m_strTitle = m_strTitle.Right(m_strTitle.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[ar:"))//������
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strArtist = strLine.Left(nPos2);
		m_strArtist = m_strArtist.Right(m_strArtist.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[al:"))//ר��
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strAlbum = strLine.Left(nPos2);
		m_strAlbum = m_strAlbum.Right(m_strAlbum.GetLength()-1 - nPos1 );
	}
	else if( -1 != strLine.Find(L"[by:"))//���������
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strEditor = strLine.Left(nPos2);
		m_strEditor = m_strEditor.Right(m_strEditor.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[offset:"))
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		SStringW m_strOffset = strLine.Left(nPos2);
		m_strOffset = m_strOffset.Right(m_strOffset.GetLength()-1 - nPos1);
		m_nOffset = _wtoi( m_strOffset.GetBuffer(1));
	}
	else //���������Ϊ�Ǳ��ʱ�����
	{
		wstring sLine = strLine.GetBuffer(1);
		//�����Ƿ�Ϊ��Ч��ʱ���ǩ�У��ų������Զ����ǩ����û��ʱ���ǩ�ĸ�ʣ����ʱ��[0:0.000]���ݴ���
		if(!MakeSureTimeLineValid(sLine))
			return true;

		wstring sTimeLabel = L"";
		wstring sLyric = L"";
		int nPosColon = -1;
		sLyric = sLine.substr( sLine.find_last_of(L']') + 1);
		do{
			nPos1 = sLine.find_first_of(L'[');
			nPosColon =  sLine.find_first_of(L':');
			nPos2 =  sLine.find_first_of(L']');
			if(nPos1 ==0 && nPos1 < nPosColon && nPosColon < nPos2) // strLineͷ����ʱ���ǩ
			{
				sTimeLabel = sLine.substr(nPos1, nPos2+1);

				TimeLineInfo timeInfoLine(SStringW( (sTimeLabel + sLyric).c_str()));

				m_vecNeteaseLrc.push_back(timeInfoLine); //���һ�е������Ƹ��

				sLine = sLine.substr(nPos2+1); //ȥ��ǰ���ǩ
			}
			else
				break;
		}
		while( true);
	}

	return true;
}

//�����ʱ������Ƚ�
bool LrcProcessor::CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2)
{
	return L1.m_nmSesonds < L2.m_nmSesonds;
}

//��֤ʱ������Ч���޷���֤�򷵻� false��
bool LrcProcessor::MakeSureTimeLineValid(wstring& line)
{
	wstring strLine = line;
	//�鿴�Ƿ��б�ǩ [:]
	size_t pos1 = strLine.find_first_of(L'[');
	size_t pos2 = strLine.find_first_of(L':');
	size_t pos3 = strLine.find_first_of(L']');
	
	//����û�б�ǩ����������ɸ�ʣ���� [0:0.000] ʱ��
	if(pos1 == wstring::npos && pos2 == wstring::npos && pos2 == wstring::npos )
	{
		if(CStringHelper::GetTrimLength(line, L" \t\r\n") == 0)//Ϊ�մ�
		{
			return false;
		}

		//������ʱ�� strLine ����û�� \n,���һ���س�����
		strLine = CStringHelper::Trim(strLine, L"\r\n") + L"\n";

		line = L"[0:0.000]" + strLine;
		return true;
	}

	if( pos1 == wstring::npos || pos2 == wstring::npos || pos2 == wstring::npos ||
		pos1 > pos2 || pos1 > pos3 || pos2 > pos3 ||
		pos1 +1 == pos2 || pos2+1 == pos3
		)	//�Ƿ��У��޷�����
		return false;

	//���ڱ�ǩ���Զ������Чʱ���ǩ�� �޷���������ֻ����Ч��ʱ���ǩ���ſ���
	wstring strMinute = strLine.substr( pos1+1,pos2 - pos1 -1);
	wstring strSecond = strLine.substr( pos2+1,pos3 - pos2 -1);
	bool bValid = true;
	for(auto i = strMinute.begin(); i != strMinute.end(); i++)
	{
		if(!iswdigit(*i))
		{
			bValid = false;
			return false;	//��Ч
		}
	}

	// ���� �� ������ dd.ddd �� dd ����ʽ
	bool HasMeetDot = false;
	for(auto i = strSecond.begin(); i != strSecond.end(); i++)
	{
		if(!HasMeetDot)
		{
			if(iswdigit(*i))
				continue;
			else if( *i == L'.')
			{
				HasMeetDot = true;
			}
		}
		else
		{
			if(!iswdigit(*i))
			{
				bValid = false;
				return false;	//��Ч
			}
		}
	}
		
	//������ʱ�� strLine ����û�� \n,���һ���س�����
	line = CStringHelper::Trim(strLine, L"\r\n") + L"\n";

	return true;
}
