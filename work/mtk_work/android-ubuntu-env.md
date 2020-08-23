# ubuntu source-list
14.04:
deb http://archive.ubuntu.com/ubuntu/ trusty main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ trusty-updates main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ trusty-security main restricted universe multiverse

12.04:
deb http://archive.ubuntu.com/ubuntu/ precise main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ precise-updates main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ precise-security main restricted universe multiverse

# ubuntu tools
14.04 tools:
sudo apt-get install bison g++-multilib git gperf libxml2-utils make python-networkx zlib1g-dev:i386 zip build-essential

12.04 tools:
sudo apt-get install git gnupg flex bison gperf build-essential zip curl libc6-dev libncurses5-dev:i386 x11proto-core-dev libx11-dev:i386 libreadline6-dev:i386 libgl1-mesa-glx:i386 g++-multilib mingw32 tofrodos libgl1-mesa-dev python-markdown libxml2-utils xsltproc zlib1g-dev:i386

ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 /usr/lib/i386-linux-gnu/libGL.so

# Open-JDK8
sudo add-apt-repository ppa:openjdk-r/ppa
sudo apt-get update
sudo apt-get install openjdk-8-jdk

# repo source
git clone https://gerrit-googlesource.lug.ustc.edu.cn/git-repo


# Samba config
sudo smbpasswd -a yangbingfeng

vim /etc/samba/smb.conf

[liminglong]
path=/home/liminglong
available=yes
browseable=yes
public=no
valid user = liminglong
writable=yes
