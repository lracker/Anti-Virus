#include "pch.h"
#include "CSocket.h"
#include <ws2tcpip.h>

CSocket::CSocket()
{

}

void CSocket::connect()
{
	// 初始化套接字环境
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 创建套接字
	m_Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 配置服务器地址和端口
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	// 端口为6666
	addr.sin_port = htons(6666);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	// 连接服务器
	::connect(m_Server, (sockaddr*)&addr, sizeof(addr));
}

void CSocket::Send(DataType Type, const char* data)
{
	int nSize;
	if (data == NULL)
		nSize = 0;
	else 
		nSize = strlen(data);
	SENDDATAPACK SendDataPack = { Type,nSize };
	// 先发送八个字节的结构体给服务器
	::send(m_Server, (const char*)&SendDataPack, sizeof(SENDDATAPACK), 0);
	// 再发送数据过去
	if(nSize)
		::send(m_Server, data, nSize, 0);
}

RECVDATASTRUCT* CSocket::Recv()
{
	RECVDATASTRUCT head = { 0 };
	if (::recv(m_Server, (char*)&head, sizeof(head) - 1, 0) != sizeof(head) - 1)
		return NULL;
	RECVDATASTRUCT* pBuff = (RECVDATASTRUCT*)malloc(head.nSize + sizeof(head));
	memset(pBuff, 0, head.nSize + sizeof(head));
	memcpy_s(pBuff, sizeof(head) - 1, &head, sizeof(head) - 1);  // 将head头部复制进去
	if (::recv(m_Server, pBuff->data, head.nSize, 0) != head.nSize)  // 把数据部分也复制
	{
		free(pBuff);
		pBuff = NULL;
		return NULL;
	}
	return pBuff;
}

void CSocket::Close()
{
	shutdown(m_Server, SD_BOTH);
}
