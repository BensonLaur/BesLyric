#include "stdafx.h"
#include "HttpRequest.h"

/*
*	@brief	HttpGet����
*	@param	strUrl			��������
*	@param	strParameter	����Ĳ���
*	@param	resultContent	�ɹ����ص�����
*	@return	true -- ����ɹ�����
*/
bool CHttpRequest::Get(string strUrl, string strParameter, string& resultContent)
{
#ifdef WIN32
    //�˴�һ��Ҫ��ʼ��һ�£�����gethostbyname����һֱΪ��
    WSADATA wsa = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsa);
	
	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, false), resultContent);

	WSACleanup();
#else
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, false), resultContent);
#endif

	return bRet;
}

bool CHttpRequest::Get(string strUrl, string strParameter, wstring& resultContent)
{
	string strResult;
	bool bRet = Get(strUrl, strParameter, strResult);

	if(!bRet)
		return false;

	resultContent = ToUtf8(strResult);

	return true;
}

/*
*	@brief	HttpPost����
*	@param	strUrl			��������
*	@param	strParameter	����Ĳ���
*	@param	resultContent	�ɹ����ص�����
*	@return	true -- ����ɹ�����
*/
bool CHttpRequest::Post(string strUrl, string strParameter, string& resultContent)
{
#ifdef WIN32
    //�˴�һ��Ҫ��ʼ��һ�£�����gethostbyname����һֱΪ��
    WSADATA wsa = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsa);

	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, true), resultContent);

	WSACleanup();
#else
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, true), resultContent);
#endif

	return bRet;
}

bool CHttpRequest::Post(string strUrl, string strParameter, wstring& resultContent)
{
	string strResult;
	bool bRet = Post(strUrl, strParameter, strResult);

	if(!bRet)
		return false;

	resultContent = ToUtf8(strResult);

	return true;
}

string CHttpRequest::GetRequestHeader(string strUrl, string strParameter, bool isPost)
{
	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	string strPath = strUrl.substr( strUrl.find_first_of('/'), strUrl.size() - strUrl.find_first_of('/'));
	
	std::stringstream stream;
	if(isPost)
	{
		//POST����ʽ
		stream << "POST " << strPath;
		stream << " HTTP/1.0\r\n";
		stream << "Host: "<< strHost << "\r\n";
		stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
		stream << "Content-Type:application/x-www-form-urlencoded\r\n";
		stream << "Content-Length:" << strParameter.length()<<"\r\n";
		stream << "Connection:close\r\n\r\n";
		stream << strParameter.c_str();
	}
	else
	{
		stream << "GET " << strPath << "?" << strParameter;
		stream << " HTTP/1.0\r\n";
		stream << "Host: " << strHost << "\r\n";
		stream <<"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
		stream <<"Connection:close\r\n\r\n";
	}
	return stream.str();
}

/*
*	@brief	Http socket ����
*	@param	strHost			�����������
*	@param	strRequest		����ľ�������
*	@param	resultContent	�ɹ����ص�����
*	@return	true -- ����ɹ�����
*/
bool CHttpRequest::SocketRequest(string strHost, string strRequest, string& resultContent)
{
	resultContent = string("");

	//��������
	int sockfd;
	struct sockaddr_in address;
	struct hostent *server;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	server = gethostbyname(strHost.c_str());
	if(server == nullptr)
		return false;  //��ȡʧ�ܣ�������������������
	memcpy((char *)&address.sin_addr.s_addr,(char*)server->h_addr, server->h_length);

	if(-1 == connect(sockfd,(struct sockaddr *)&address,sizeof(address))){
		return false; //����ʧ��
	}

	//��������

	byte Temp[HTTP_DATA_BLOCK_SIZE];
	std::size_t nOriginCount = 0;
	PBYTE pBuffer = nullptr;
	int nRecieve= 0;
	int nTotalRecieved = 0;

#ifdef WIN32
	send(sockfd, strRequest.c_str(),strRequest.size(),0);

	while(nRecieve = recv(sockfd, (char*)Temp, HTTP_DATA_BLOCK_SIZE,0))
	{
		assert( nRecieve != -1);
		nTotalRecieved += nRecieve;
		if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved+1)) //�ڴ����ʧ��
		{
			if(nOriginCount != 0) //���֮ǰ�������ڴ棬��Ҫ�ͷ�
				delete pBuffer;	
			return false;
		}

		//���µõ������ݸ���׷�ӵ� pBuffer ����
		copy(Temp + 0, Temp + nRecieve, pBuffer + nOriginCount);

		//���� nOriginCount Ϊ�´� Realloc ��׼��
		nOriginCount = nTotalRecieved;
	}

	closesocket(sockfd);

#else
	write(sockfd,strRequest.c_str(),strRequest.size());

	while(nRecieve = read(sockfd, (char*)Temp, NET_DATA_BLOCK_SIZE))  
	{
		assert( nRecieve != -1);
		nTotalRecieved += nRecieve;
		if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved+1)) //�ڴ����ʧ��
		{
			if(nOriginCount != 0) //���֮ǰ�������ڴ棬��Ҫ�ͷ�
				delete pBuffer;	
			return false;
		}

		//���µõ������ݸ���׷�ӵ� pBuffer ����
		copy(Temp + 0, Temp + nRecieve, pBuffer + nOriginCount);

		//���� nOriginCount Ϊ�´� Realloc ��׼��
		nOriginCount = nTotalRecieved;
	}
	close(sockfd);
#endif

	pBuffer[nTotalRecieved] = '\0';
	resultContent = (char*)pBuffer;

	if(pBuffer)
		delete pBuffer;
	
	return true;
}


//���ڽ���õ�����ת��Ϊutf-8���ֽ��ַ���
wstring CHttpRequest::ToUtf8(string strAscii)
{
	wstring wstrResult;

	int nRet=MultiByteToWideChar(CP_UTF8,0,strAscii.c_str(),strAscii.size(),NULL,0); 
    if(nRet>0)
    {
        wchar_t *pBuf=new wchar_t[nRet+1];
        MultiByteToWideChar(CP_UTF8,0,strAscii.c_str(),strAscii.size(),pBuf,nRet+1);
		pBuf[nRet] = '\0'; //������Ƕ���һ���ַ����ض�
        wstrResult = wstring(pBuf);
        delete []pBuf;
    }

	return wstrResult;
}

/*
*	@brief	���������·����µ��ڴ�
*	@param	pPByte				�ڴ�ָ���ָ��
*	@param	nOldSize			֮ǰ���ڴ��С�� Ϊ0ʱ����ʾ�´����ڴ�
*	@param	nNewSize			�·�����ڴ��С(Ϊ0������false)
*	@return	true -- �ɹ�����ָ��Ҫ���ڴ�
*/
bool CHttpRequest::Realloc(PBYTE* pPByte, std::size_t nOldSize,  std::size_t nNewSize)
{
	if(nNewSize == 0)
		return false;
	
	PBYTE pNewBuffer = new BYTE[nNewSize];
	memset(pNewBuffer, 0, nNewSize);

	if(!pNewBuffer)
		return false;

	if(nOldSize != 0)
	{
		for(std::size_t i=0; i < nOldSize && i< nNewSize; i++)
		{
			pNewBuffer[i] = (*pPByte)[i];
		}

		delete *pPByte;
	}
	
	*pPByte = pNewBuffer;

	return true;
};