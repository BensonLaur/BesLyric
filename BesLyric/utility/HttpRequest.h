/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "stdafx.h"

#include <WinSock2.h>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

//���ڷ�Http����
class CHttpRequest
{
#define HTTP_DATA_BLOCK_SIZE 1024*10

public:
	/*
	*	@brief	HttpGet����
	*	@param	strUrl			��������
	*	@param	strParameter	����Ĳ���
	*	@param	resultContent	�ɹ����ص�����
	*	@return	true -- ����ɹ�����
	*/
	bool Get(string strUrl, string strParameter, string& resultContent);
	bool Get(string strUrl, string strParameter, wstring& resultContent);

	/*
	*	@brief	HttpPost����
	*	@param	strUrl			��������
	*	@param	strParameter	����Ĳ���
	*	@param	resultContent	�ɹ����ص�����
	*	@return	true -- ����ɹ�����
	*/
	bool Post(string strUrl, string strParameter, string& resultContent);
	bool Post(string strUrl, string strParameter, wstring& resultContent);

private:
	string GetRequestHeader(string strUrl, string strParameter, bool isPost);

	/*
	*	@brief	Http socket ����
	*	@param	strHost			�����������
	*	@param	strRequest		����ľ�������
	*	@param	resultContent	�ɹ����ص�����
	*	@return	true -- ����ɹ�����
	*/
	bool SocketRequest(string strHost, string strRequest, string& resultContent);

	//���ڽ���õ�����ת��Ϊutf-8���ֽ��ַ���
	wstring ToUtf8(string strAscii);

	/*
	*	@brief	���������·����µ��ڴ�
	*	@param	pPByte				�ڴ�ָ���ָ��
	*	@param	nOldSize			֮ǰ���ڴ��С�� Ϊ0ʱ����ʾ�´����ڴ�
	*	@param	nNewSize			�·�����ڴ��С(Ϊ0������false)
	*	@return	true -- �ɹ�����ָ��Ҫ���ڴ�
	*/
	static bool Realloc(PBYTE* pPByte, std::size_t nOldSize,  std::size_t nNewSize);
};