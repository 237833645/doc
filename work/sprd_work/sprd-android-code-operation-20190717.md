# 展讯Y61代码编译及打包方式

- [展讯Y61代码编译及打包方式](#展讯y61代码编译及打包方式)
  - [软件代码管理](#软件代码管理)
  - [编译工程](#编译工程)

## 软件代码管理

- 代码地址下载

```bash
git clone git@192.168.1.174:sl8521e/base.git
```

- 切换branch

```bash
git checkout -b fh_common -t origin fh_common
```

- 更新同步代码

```bash
git pull --rebase
```

## 编译工程

- 软件工程介绍

| 工程名                 | 介绍                   |
| ------------------- | -------------------- |
| sl8521e-2h10-native | nand方案（我Y61使用的是nand） |
| sl8521e-1h10-native | emcp方案               |

- 编译前拷贝展讯闭源包
  - user版本

    ```bash
    1、  cd 工程目录下
    2、  cp -rf 2h10_out/user/out    （拷贝user版本需要的包）
    3、  cp 2h10_out/y61_temp_out/* out/target/product/sl8521e_2h10/
    ```

  - userdebug版本

    ```bash
    1、  cd 工程目录下
    2、  cp -rf 2h10_out/userdebug/out    （拷贝user版本需要的包）
    3、  cp 2h10_out/y61_temp_out/* out/target/product/sl8521e_2h10/
    ```

- 编译前拷贝项目修改代码覆盖原始代码 （python copy.py 项目名）
  - 拷贝期间会提示代码差异是否要合入，是选择Y

    ```bash
    cd FH_PROJECT && python copy.py Y61
    ```

  - 拷贝完成会提示  
    >    Copy Script Exec Successed !!!!

- 编译代码
  - 切换到代码根目录
  - 配置编译需要的环境变量

    ```bash
    source build/envsetup.sh
    ```

  - 选择编译工程（ user版本lunch sl8521e_2h10_native-user ）

    ```bash
    lunch sl8521e_2h10_native-userdebug
    ```
  
  - 导入linux头文件编译依赖

    ```bash
    kheader
    ```

  - 编译 （j8是使用8核编译）

    ```bash
    make -j8 2>&1 | tee log.log
    ```

  - 打包image（ 打包user版本./make_pac_sign.sh sl8521e_2h10_native user ）

    ```bash
    ./make_pac_sign.sh sl8521e_2h10_native userdebug
    ```

  - 完成打包后会在代码根目录生产sl8521e_2h10_native_userdebug.pac这个文件
