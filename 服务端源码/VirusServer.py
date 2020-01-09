import socket
from enum import IntEnum
import VirusSQL
import threading
import struct


class DataType(IntEnum):
    SYS    = -1 # 服务器发过去的消息
    UPDATE = 0  # 更新消息
    UPLOAD = 1  # 提交样本消息


class Handler:
    def __init__(self):
        self.user_id = None
        self.args = None
        self.socket = None

    # 更新消息
    def UPDATE(self):
        # 查询数据库
        sql = "select * from file_md5"
        _db.cursor.execute(sql)
        result = _db.cursor.fetchall()
        data = str(len(result))
        for i in result:
            data += '\n'
            data += str(i['md5'])
            data += '\n'
            data += str(i['name'])
        # 拼接好以后就发送给客户端
        sendtoclientsocket(self.socket, DataType.SYS, data)

    def UPLOAD(self):
        sql = "INSERT INTO file_md5(md5, name)VALUES('%s','%s');"%(self.args[0],self.args[1])
        _db.cursor.execute(sql)
        _db.cursor.execute('commit')

# 发送给客户端的
def sendtoclientsocket(client, Type, strData):
    data = strData.encode('gb2312')
    size = len(data)
    strFormat = 'ii%ds' % size
    rawData = struct.pack(strFormat, Type.value, size, data)
    client.send(rawData)

# 从客户端那里接收信息
def recvfromclient(client):
    while True:
        # 接收数据包前八个
        datapack = client.recv(8)
        if len(datapack) == 0:
            raise Exception("客户端断开连接")
        Type, size = struct.unpack("ii", datapack)
        if size != 0:
            data = client.recv(1024).decode('gb2312').split('\n')
        else:
            data = "NULL"
        return Type, data


class Socket:
    def __init__(self):
        # 创建套接字
        self.Socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 绑定端口
        self.Socket.bind(("127.0.0.1", 6666))
        # 监听端口
        self.Socket.listen(socket.SOMAXCONN)
        # 创建数据库和表，如果存在了则不再创建
        global _db  # 创建一个全局对象
        _db = VirusSQL.Sql()
        _db.createDBTable()

    # 从客户端接收信息
    def recvclientData(self, handler):
        Type, args = None, None
        while True:
            try:
                # 接收数据
                Type, args = recvfromclient(handler.socket)
            except Exception as e:
                del _clientDict[handler.user_id]
                break
            try:
                Type = DataType(Type)
            except:
                sendtoclientsocket(handler.socket, DataType.SYS, "无效的请求")
            # 获取函数
            func = getattr(handler, Type.name)
            handler.args = args
            func()

    # 运行
    def run(self):
        # 服务器连接并创建线程接收信息
        while True:
            client, addr = self.Socket.accept()
            h = Handler()
            h.socket = client
            print('接入新的客户端:', addr)
            thread = threading.Thread(target=self.recvclientData, args=(h,)).start()


_clientDict = {}

if __name__ == '__main__':
    socket = Socket()
    socket.run()
