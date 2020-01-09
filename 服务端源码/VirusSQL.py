import pymysql


class Sql(object):
    # 构造函数，主要用于连接指定的数据库
    def __init__(self):
        try:
            # 获取一个连接对象，获取的时候可能会产生异常
            self.connect = pymysql.connect(host="127.0.0.1",port=3306,user="root",password="11111111A!root",
                                           cursorclass=pymysql.cursors.DictCursor)
            # 创建指针
            self.cursor=self.connect.cursor(cursor=pymysql.cursors.DictCursor)
        except Exception as e:
            print(e)

    def createDBTable(self):
        # 创建数据库
        self.cursor.execute("CREATE DATABASE IF NOT EXISTS VIRUS")
        # 连接VIRUS数据库
        self.cursor.execute("use VIRUS")
        # 创建一个名为file_md5的表
        sql = """CREATE TABLE IF NOT EXISTS `file_md5`(
                 `md5` varchar(50) not null,
                 `name` varchar(50) not null)
                 """
        self.cursor.execute(sql)


if __name__ == '__main__':
    sql = Sql()