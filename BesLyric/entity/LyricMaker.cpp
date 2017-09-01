/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// LyricMaker.cpp :  ʵ��  LyricMaker�� �Ľӿ�	
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LyricMaker.h"
#include "../utility/FileHelper.h"
#include "../utility/WinFile.h"
#include "Windows.h"
#include <mmsystem.h> 
#include "BSMessageBox.h"
#pragma comment (lib, "winmm.lib")

LyricMaker::LyricMaker():
		 m_bLastLineSpace(false),
		 m_nCurLine(0),
		 m_nTotalLine(0)
{
	m_szMusicPathName[0]= _T('\0');		
	m_szLyricPathName[0]= _T('\0');
	m_szOutputPath[0]= _T('\0');
	m_szOutputPathName[0] = _T('\0');
}

void LyricMaker::Init(CSettingPage *pSettingPage)
{
	SASSERT(NULL != pSettingPage);

	//���������ڶ���
	m_pSettingPage = pSettingPage;
}

//���ø���·��
//��������·��ʱ�����벥��������Ҫ�� ��Ϣ�������ڵľ��
void LyricMaker::setMusicPath(LPCTSTR pathName, HWND hostWnd)
{
	_tcscpy(m_szMusicPathName,pathName);
	m_musicPlayer.init(m_szMusicPathName,hostWnd);
}
	
void LyricMaker::setLyricPath(LPCTSTR pathName)
{
	_tcscpy(m_szLyricPathName,pathName);
}
	
void LyricMaker::setm_szOutputPath(LPCTSTR pathName)
{
	_tcscpy(m_szOutputPath,pathName);
}

//���� LyricMaker�� �������Ϊ��, ����������ļ���
void LyricMaker::reloadMaker()
{
	m_vLyricOriginWithEmptyLine.clear();
	m_vLyricOrigin.clear();
	m_vLyricOutput.clear();

	//����������ļ���
	generateOutputFileName();
}

//������ʼ,��¼��ʼ������ʱ��
//�ɹ����ŷ���true����Ҫת����ʽ����false
bool LyricMaker::makingStart()
{
	//���»�����������¼������
	m_nTotalLine = m_vLyricOrigin.size();
	m_nCurLine = 0;

	return playMusic();
}

//Ϊ��һ�и�� ����� ����������Ҫ��� ʱ�����ʽ��д��m_vLyricOutput��
void LyricMaker::markNextLine()
{
	int ms = m_musicPlayer.getPosition();

	//�����û����õ�ʱ��ƫ�ƣ��޸ĵõ�ʵ�ʼ�¼�ĺ�����
	ms -= m_pSettingPage->m_nTimeShift;
	if(ms < 0 )
		ms = 0;

	//�õ�[00:33.490] ��ʽ��ʱ�䴮
	TCHAR timeBuf[255];
	msToLyricTimeString(ms, timeBuf);

	//�����µ�һ�м���m_vLyricOutput��
	SStringT newLine(timeBuf);
	newLine.Append(m_vLyricOriginWithEmptyLine.at(m_nCurLine-1));
	newLine.Append(SStringT(_T("\n")));
	m_vLyricOutput.push_back(newLine);

	//MB(SStringT().Format(_T("%s"),m_vLyricOutput.at(m_nCurLine-1)));

	//setLastLineSpace(false);
}

//�����һ�в��ǿհ��еĻ�,���
void LyricMaker::markSpaceLine()
{
	if(!isLastLineSpace())
	{
		m_nCurLine += 1;//����ҲҪҲ��1��
		m_nTotalLine += 1;

		int ms = m_musicPlayer.getPosition();

		//�����û����õ�ʱ��ƫ�ƣ��޸ĵõ�ʵ�ʼ�¼�ĺ�����
		ms -= m_pSettingPage->m_nTimeShift;
		if(ms < 0 )
			ms = 0;

		//�õ�[00:33.490] ��ʽ��ʱ�䴮
		TCHAR timeBuf[255];
		msToLyricTimeString(ms, timeBuf);

		//�����µ�һ�м���m_vLyricOutput�� (ֻ��ʱ��Ŀհ���)
		SStringT newLine(timeBuf);
		newLine.Append(SStringT(_T("\n")));
		m_vLyricOutput.push_back(newLine);

		// ͬʱ���� m_vLyricOriginWithEmptyLine
		int nCount = 0;
		for(auto iter = m_vLyricOriginWithEmptyLine.begin(); iter != m_vLyricOriginWithEmptyLine.end(); iter++)
		{
			if( ++nCount ==  m_nCurLine)//�������Ӧ�����ڵ��������ڴ˵�����ǰ���1�п���
			{
				m_vLyricOriginWithEmptyLine.insert(iter, SStringW(L""));
				break;
			}
		}

	}
}

//�� m_vLyricOutput д������ļ�m_szOutputPathName ��
void LyricMaker::makingEnd()
{
	bool bRet = FileOperator::WriteToUtf8File(m_szOutputPathName,m_vLyricOutput);

	if(!bRet)
	{
		BSMessageBox m;
		m.MessageBoxW(NULL,SStringT().Format(_T("�ļ�д��ʧ��:\\n��%s��\\n!��ȷ���ļ���Ч"),m_szOutputPathName),
			_T("ʧ����ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}

	//ֹͣ��������
	stopMusic();
}

//���ݵ�ǰ����λ�ã����½�������ĸ������
void LyricMaker::RecorrectLyricData()
{
	//���� �����Ѿ�����ǵ�����
	int msCurrentPos = m_musicPlayer.getPosition();

	//������� m_vLyricOriginWithEmptyLine
	m_vLyricOriginWithEmptyLine.clear();
	auto iterOriginLyric = m_vLyricOrigin.begin();

	m_nCurLine = 0; //���¼��� ��ǰ��
	for(auto iter = m_vLyricOutput.begin(); iter!= m_vLyricOutput.end(); iter++) //�����Ѿ��洢�ĸ��
	{
		WCHAR* pBuffer = iter->GetBuffer(1);
		int m,s,ms;
		m = s = ms = 0;
		WCHAR szBuffer[MAX_WCHAR_COUNT_OF_LINE]={0};
		swscanf(pBuffer, L"[%d:%d.%d]%s", &m, &s, &ms, szBuffer); //��ø�ʵ�ʱ����Ϣ
		int msTotal = m * 60 * 1000 + s * 1000 + ms;
		
		if(msTotal <= msCurrentPos)
		{
			m_nCurLine++;
			
			//������� m_vLyricOriginWithEmptyLine
			if( 0 == wcslen(szBuffer))//�������
				m_vLyricOriginWithEmptyLine.push_back(SStringW(L""));
			else
				m_vLyricOriginWithEmptyLine.push_back(*iterOriginLyric++);
		}
		else
		{
			//ɾ������������ʣ�µ�ʱ����
			while(iter!=m_vLyricOutput.end())
			{
				iter = m_vLyricOutput.erase(iter);
			}
			break;
		}
	}

	//���� ��� δ����ĸ��
	while(iterOriginLyric != m_vLyricOrigin.end())
			m_vLyricOriginWithEmptyLine.push_back(*iterOriginLyric++);

	//���¼��� m_nTotalLine
	m_nTotalLine = m_vLyricOriginWithEmptyLine.size();
}


//���ԭʼ���(��������Ŀ���) 
// nPos �ӵ�1�п�ʼ
SStringW LyricMaker::GetOriginLyricAt(std::size_t nPos)
{
	SASSERT( nPos > 0 && nPos <= m_vLyricOriginWithEmptyLine.size() && L"nPos ������Ч��Χ");

	return m_vLyricOriginWithEmptyLine[nPos-1];
}


//��õ�ǰ����� �ļ���
void LyricMaker::getOutputFileName(TCHAR* name, int lenth)
{
	_tcscpy(name,outputFileName);
}

//��õ�ǰ����� ·���ļ���
void LyricMaker::getm_szOutputPathName(TCHAR* name, int lenth)
{
	_tcscpy(name,m_szOutputPathName);
}

//�������ֲ���
void LyricMaker::stopMusic()
{
	m_musicPlayer.closeStop();
}

void LyricMaker::setLastLineSpace(bool value)
{
	this->m_bLastLineSpace = value;
}

//��һ���Ƿ�Ϊ�հ���
bool LyricMaker::isLastLineSpace()
{
	// ���� m_vLyricOutput ���ݣ������һ�������Ƿ�ֻ��ʱ���ǩ
	// �����ǣ�Ϊ�ǿ���
	bool bIsLastLineSpace = false;

	if(m_vLyricOutput.begin() != m_vLyricOutput.end())
	{
		WCHAR* pBuffer = (m_vLyricOutput.end()-1)->GetBuffer(1);
		int m,s,ms;
		WCHAR szBuffer[MAX_WCHAR_COUNT_OF_LINE]={0};
		swscanf(pBuffer, L"[%d:%d.%d]%s", &m, &s, &ms,szBuffer); //��ø�ʵ�ʱ����Ϣ
		if(wcslen(szBuffer)==0) //szBuffer û�а������и�ʣ�������������ݣ�����һ������0
			bIsLastLineSpace = true;
	}

	return bIsLastLineSpace;
}

//���ļ���ȡÿ�и�ʵļ�������
vector<SStringT> LyricMaker::getLyricOrigin(File& encodingFile)
{
	vector<SStringW> lyrics;

	FileOperator::ReadAllLinesW(encodingFile, &lyrics);
	
	return lyrics;
}


//����m_szMusicPathName ���ļ����õ�����ļ�����������outputFileName �� m_szOutputPathName��ֵ
void LyricMaker::generateOutputFileName()
{
	int len = _tcslen(m_szMusicPathName);
	if(len==0)
		return;

	// �������һ��'\\'��λ��
	int i,j;
	int pos; 
	for(i=0;i< len;i++)
		if(m_szMusicPathName[i]==_T('\\'))
			pos = i;

	//���Ƹ����� outputFileName ����������׺
	for(i=pos+1,j=0;m_szMusicPathName[i]!=_T('.');i++,j++)
	{
		outputFileName[j] = m_szMusicPathName[i];
	}
	outputFileName[j] = _T('\0');

	//����������������ļ���
	_tcscat(outputFileName,_T(".lrc"));

	//��ʼ�� m_szOutputPathName
	_tcscpy(m_szOutputPathName,m_szOutputPath);
	_tcscat(m_szOutputPathName,_T("\\"));
	_tcscat(m_szOutputPathName,outputFileName);
}

//�������ֵʱ�� ת��Ϊ���ʱ���ʽ ��[00:33.490] Look at the stars��
//�� [00:33.490] ��ʽ����� timeBuf
void LyricMaker::msToLyricTimeString(int ms, LPTSTR timeBuf)
{
	int minutes = ms/60000;
	ms = ms%60000;
	double seconds = ms*1.0/1000;
	_stprintf(timeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);
}

//��������
bool LyricMaker::playMusic()
{
	if(m_musicPlayer.openTest() == false)
	{
		return false;
	}

	m_musicPlayer.openStart();
	return true;
}

