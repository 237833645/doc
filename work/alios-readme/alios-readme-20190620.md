### <center>阿里文档汇总</center>

##### 阿里文档地址

<https://developer.alios.cn/#/md%2Fdevelopercenter%2F%E5%A6%82%E4%BD%95%E8%8E%B7%E5%BE%97%E5%92%8C%E4%BD%BF%E7%94%A8%E6%BA%90%E7%A0%81%2F%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97%2F%E6%90%AD%E5%BB%BA%E7%BC%96%E8%AF%91%E7%8E%AF%E5%A2%83.md?docversion=0>

##### 代码库下载地址

-   远程下载repo地址
> repo init -u ssh://gerrit-custom2.yunos.com:29418/repo/yunos/tianmu -b  rel_watch_s01_r2_hht_yunospick

-   本地git下载地址
> git clone git@192.168.1.174:rel_watch_s01_r2_hht_yunospick/base.git

##### 代码编译指令
- 配置环境依赖
 - 操作系统:  ubuntu12.04
 - 编译器：   gcc&g++ 4.6
 - 配置编译依赖库:  
 > cd xmake/ ; ./xmake.sh

    命令行 选择 300
 - 安装nodejs：
 > cd xmake/ ; ./setup_node.sh

- 编译前代码遍历环境配置 (工程名：alilo_a1 版本类型：user )
    >cd xmake && ./mk_yunos.sh alilo_a1 user config --enable-cntr-rt=no --enable-cntr-cvg=no --enable-cntr-hal=no --enable-unified-surface=yes --with-permission=no --enable-jsaot=on --enable-closed-source


-   编译 (终端未关闭前提下，都可以使用下面的指令来编译而不需要每次都配置‘编译前代码遍历环境配置’)
    source xdirs && xmake


-   编译后生成img目录


    prebuilts/images/工程名/

##### 软件下载方式

-   展讯平台下载

    使用展讯ResearchDownload工具下载 pac包地址在编译后生成img目录下面。
    同展讯下载方式
