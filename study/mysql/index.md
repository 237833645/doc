# MYSQL

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

- 后台服务启动

```bash
brew services start mysql
```

- 非后台登陆

```bash
mysql.server start
```

- root账号免密登陆

```bash
mysql -uroot
```

- 重启mysql服务

```bash
brew services restart mysql
```

- 本地登陆mysql root + 密码

```bash
mysql -uroot -p121212   ---> 121212是root设置的密码
```

- 创建账号及登陆密码

- 修改mysql数据相关文件路径


## mysql数据库使用

### 添加

### 删除

### 修改

### 查找
