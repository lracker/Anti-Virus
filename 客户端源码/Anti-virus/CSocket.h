#pragma once
#include <WinSock2.h>
#include <stdint.h>

enum DataType
{
	ERR = -2,
	SYS = -1,
	UPDATE = 0,
	UPLOAD = 1
};


/*客户端发过去的消息结构体*/
typedef struct _SENDDATAPACK
{
	DataType Type;  //数据的类型
	int nSize;		//数据的大小
}SENDDATAPACK;

#pragma pack(push,1)   //让结构体按照一个字节对齐
/*服务器发回来的消息结构体*/
typedef struct _RECVDATASTRUCT
{
	uint32_t Type;  // 数据的类型
	uint32_t nSize;		// 数据的大小
	char data[1];   // 数据，设置为1，方便后面覆盖掉的时候可以通过.data操控数据
}RECVDATASTRUCT;
#pragma pack(pop)

class CSocket
{
	SOCKET m_Server;
public:
	CSocket();
	void connect();
	void Send(DataType Type, const char* data = NULL);
	RECVDATASTRUCT* Recv();  // 将数据解包再封装起来
	void Close();

};