# Brew安装MYSQL

- [Brew安装MYSQL](#brew安装mysql)
  - [安装](#安装)
  - [MYSQL配置](#mysql配置)
    - [安全配置](#安全配置)
    - [配置相关操作命令汇总](#配置相关操作命令汇总)
  - [mysql数据库使用](#mysql数据库使用)
    - [MySQL 连接](#mysql-连接)
    - [MySQL 用户设置](#mysql-用户设置)
    - [MySQL 创建数据库](#mysql-创建数据库)
      - [使用 mysqladmin 创建数据库](#使用-mysqladmin-创建数据库)

参考文档:[homebrew 安装mysql后，如何配置mysql](https://blog.csdn.net/chenshuai1993/article/details/53141985?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-7.channel_param&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-7.channel_param)

参考文档:[www.runoob.com](https://www.runoob.com/mysql/mysql-tutorial.html)

## 安装

- 安装指令

```bash
brew install mysql
```

- 安装后显示信息如下

```bash
==> Installing dependencies for mysql: protobuf
==> Installing mysql dependency: protobuf
==> Pouring protobuf-3.13.0.catalina.bottle.tar.gz
==> Caveats
Emacs Lisp files have been installed to:
  /usr/local/share/emacs/site-lisp/protobuf
==> Summary
🍺  /usr/local/Cellar/protobuf/3.13.0: 266 files, 19.8MB
==> Installing mysql
==> Pouring mysql-8.0.21_1.catalina.bottle.tar.gz
==> /usr/local/Cellar/mysql/8.0.21_1/bin/mysqld --initialize-insecure --user=wangfeng --basedir=/usr/local/Cellar/mysql/8.0.21_1 --datadir=/usr/local/var/mysql --tmpdir=/tmp
==> Caveats
We've installed your MySQL database without a root password. To secure it run:
    mysql_secure_installation

MySQL is configured to only allow connections from localhost by default

To connect run:
    mysql -uroot

To have launchd start mysql now and restart at login:
  brew services start mysql
Or, if you don't want/need a background service you can just run:
  mysql.server start
==> Summary
🍺  /usr/local/Cellar/mysql/8.0.21_1: 290 files, 291.2MB
==> Caveats
==> protobuf
Emacs Lisp files have been installed to:
  /usr/local/share/emacs/site-lisp/protobuf
==> mysql
We've installed your MySQL database without a root password. To secure it run:
    mysql_secure_installation

MySQL is configured to only allow connections from localhost by default

To connect run:
    mysql -uroot

To have launchd start mysql now and restart at login:
  brew services start mysql
Or, if you don't want/need a background service you can just run:
  mysql.server start
```

## MYSQL配置

### 安全配置

- MySQL安全配置向导，设置MySQL密码

  - 指令

  ```bash
  mysql_secure_installation
  ```

  - 指令执行后交互界面

  ```bash
  Securing the MySQL server deployment.
  Connecting to MySQL using a blank password.
  VALIDATE PASSWORD COMPONENT can be used to test passwords
  and improve security. It checks the strength of password
  and allows the users to set only those passwords which are
  secure enough. Would you like to setup VALIDATE PASSWORD component?
  Press y|Y for Yes, any other key for No: ^@       ---->这里询问是否配置密码
  ```

  - 选择 'Y', 输入密码

  ```bash
  New password:
  Re-enter new password:
  ```

  - 输入密码后提示是否需要删除匿名测试账号

  ```bash
  By default, a MySQL installation has an anonymous user,
  allowing anyone to log into MySQL without having to have
  a user account created for them. This is intended only for
  testing, and to make the installation go a bit smoother.
  You should remove them before moving into a production
  environment.

  Remove anonymous users? (Press y|Y for Yes, any other key for No) :
  ```

  - 选择删除anonymous users后配置,disable root remotely login,选择'Y'
  
  ```bash
  Normally, root should only be allowed to connect from
  'localhost'. This ensures that someone cannot guess at
  the root password from the network.

  Disallow root login remotely? (Press y|Y for Yes, any other key for No) :
  ```

  - 是否删除'test'数据库,这个知识用来测试,任何人都可以访问,选择删除

  ```bash
  ... skipping.
  By default, MySQL comes with a database named 'test' that
  anyone can access. This is also intended only for testing,
  and should be removed before moving into a production
  environment.


  Remove test database and access to it? (Press y|Y for Yes, any other key for No) :
  ```

  - 是否重新加载权限表，选择'Y'
  
  ```bash
  ... skipping.
  Reloading the privilege tables will ensure that all changes
  made so far will take effect immediately.

  Reload privilege tables now? (Press y|Y for Yes, any other key for No) :
  ```

  - 配置完成

  ```bash
  ... skipping.
  All done!
  ```

### 配置相关操作命令汇总

| 描述 | 命令 |
| - | - |
| 后台启动服务 | brew services start mysql |
| 后台停止服务 | brew services stop mysql |
| 后台重启服务 | brew services restart mysql |
| 非后台启动服务 | mysql.server start |
| 非后台停止服务 | mysql.server stop |
| 非后台重启服务 | mysql.server restart |
| root登陆+密码 | mysql -uroot -p  或者  mysql -h localhost -u root -p|

## mysql数据库使用

mysql常用命令

| 描述 | 命令 |
| - | - |
| 创建数据库 | CREATE DATABASE db_name; |
| 查看编码格式 | SHOW CREATE DATABASE db_name; |
| 查看当前服务器下的数据库列表 | SHOW DATABASES; |
| 修改编码格式 | ALTER DATABASE db_name CHARACTER SET utf8; |
| 删除数据库 | DROP DATABASE db_name; |
| 选择数据库 | USE db_name; |
| 显示当前数据库 | SELECT DATABASE(); |
| 创建数据表(创建字段名) | CREATE TABLE table_name (column_name data_type,......) |
| 显示数据表列表 | SHOW TABLES [FROM db_name]; |
| 显示数据表的结构 | SHOW COLUMNS FROM tb_name; |
| 修改数据表(添加一列) | ALTER TABLE tb_name ADD column_name data_type; |
| 修改数据表(添加多列) | ALTER TABLE tb_name ADD (column_name data_type,…); |
| 修改数据表(删除列) | ALTER TABLE tb_name DROP column_name,DROP column_name,…… |
| 插入记录(创建字段下的数据) | INSERT tb_name (col_name,...) VALUES(val,...); |
| 更新记录UPDATE | UPDATE tb_name SET age = age + 5, sex = 2 WHERE username='TOM'; |
| 删除记录DELETE | DELETE FROM tb_name WHERE id=2; |
| 查找记录 | SELECT expr,... FROM tb_name; |
| 排序 | SELECT * FROM stu ORDER BY 笔试 LIMIT 0,8 |

### MySQL 连接

使用mysql二进制方式连接

您可以使用MySQL二进制方式进入到mysql命令提示符下来连接MySQL数据库。

- 实例
以下是从命令行中连接mysql服务器的简单实例：

```bash
[root@host]# mysql -u root -p
Enter password:******
```

在登录成功后会出现 mysql> 命令提示窗口，你可以在上面执行任何 SQL 语句。

以上命令执行后，登录成功输出结果如下:

```bash
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 2854760 to server version: 5.0.9

Type 'help;' or '\h' for help. Type '\c' to clear the buffer.
```

在以上实例中，我们使用了root用户登录到mysql服务器，当然你也可以使用其他mysql用户登录。

如果用户权限足够，任何用户都可以在mysql的命令提示窗口中进行SQL操作。

退出 mysql> 命令提示窗口可以使用 exit 命令，如下所示：

```bash
mysql> exit
Bye
```

### MySQL 用户设置

如果你需要添加 MySQL 用户，你只需要在 mysql 数据库中的 user 表添加新用户即可。

以下为添加用户的的实例，用户名为guest，密码为guest123，并授权用户可进行 SELECT, INSERT 和 UPDATE操作权限：

```bash

```

### MySQL 创建数据库

我们可以在登陆 MySQL 服务后，使用 create 命令创建数据库，语法如下:

```bash
CREATE DATABASE 数据库名;
```

以下命令简单的演示了创建数据库的过程，数据名为 RUNOOB:

```bash
mysql -u root -p
Enter password:******  # 登录后进入终端

mysql> create DATABASE RUNOOB;
```

#### 使用 mysqladmin 创建数据库

使用普通用户，你可能需要特定的权限来创建或者删除 MySQL 数据库。

所以我们这边使用root用户登录，root用户拥有最高权限，可以使用 mysql mysqladmin 命令来创建数据库。

以下命令简单的演示了创建数据库的过程，数据名为 RUNOOB:

```bash
mysqladmin -u root -p create RUNOOB
Enter password:******
```

以上命令执行成功后会创建 MySQL 数据库 RUNOOB。

```bash
mysql> create DATABASE RUNOOB;
Query OK, 1 row affected (0.00 sec)

mysql> SHOW DATABASES;
+--------------------+
| Database           |
+--------------------+
| information_schema |
| mysql              |
| performance_schema |
| RUNOOB             |
| sys                |
+--------------------+
5 rows in set (0.00 sec)
```
