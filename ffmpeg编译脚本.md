## x86平台

### 目录结构

建立文件夹

```bash
cd /opt

mkdir ffmpeg
cd ffmpeg

mkdir build #构建文件夹：动态库，头文件
mkdir source #源码文件夹
```

### 安装依赖

```bash
sudo apt-get install autoconf automake build-essential libass-dev libfreetype6-dev  libtheora-dev libtool libvorbis-dev pkg-config texinfo zlib1g-dev unzip cmake yasm libx264-dev libx265-dev libmp3lame-dev libopus-dev nasm yasm
```

安装libfdk-aac

```bash
cd /opt/ffmpeg_dependency

wget -O fdk-aac.tar.gz https://github.com/mstorsjo/fdk-aac/tarball/master

tar xzvf fdk-aac.tar.gz

cd mstorsjo-fdk-aac*

autoreconf -fiv

./configure --prefix="/opt/ffmpeg/build"--disable-shared

make

make install

make distclean
```

### 下载ffmpeg源码

```bash
git clone https://github.com/FFmpeg/FFmpeg.git /opt/ffmpeg/source
```

### 构建编译脚本 

在`/opt/ffmpeg`下新建脚本 `run_configure_x86_64.sh`。

```shell
#!/bin/bash
ROOT="/opt/ffmpeg"
BUILD="${ROOT}/build"
PATH="${BUILD}/bin:$PATH"
PKG_CONFIG_PATH="${BUILD}/lib/pkgconfig"

cd "${ROOT}/source”

./configure \
  --prefix=${BUILD} \
  --pkg-config-flags="--static" \
  --extra-cflags="-I${BUILD}/include" \
  --extra-ldflags="-L${BUILD}/lib" \
  --bindir="${BUILD}/bin" \
  --extra-libs=-lm \
  --enable-shared \
  --enable-gpl \
  --enable-libass \
  --enable-libfdk_aac \
  --enable-libfreetype \
  --enable-libopus \
  --enable-libtheora \
  --enable-libvorbis \
  --enable-pthreads \
  --enable-libvpx \
  --enable-libx264 \
  --enable-libx265 \
  --enable-libxvid \
  --enable-nonfree \
  --disable-doc

```

```bash
sudo ./run_configure_x86_64.sh
```

进入源码`/opt/ffmpeg/source`并进行make编译

```bash
sudo make -j16
sudo make install
sudo make distclean
```

在`/etc/ld.so.conf.d`内新建`ffmpeg.conf`，并写入

```bash
/opt/ffmpeg/build/lib
```

配置环境变量

在`.bashrc`或者`.zshrc`中写入

```bash
export PATH="/opt/ffmpeg/build/bin:$PATH"
```

刷新环境变量

```bash
source .bashrc
```

### 测试

```bash
ffmpge -version

#out
ffmpeg version N-105291-gdcc9454ab9 Copyright (c) 2000-2022 the FFmpeg developers
built with gcc 9 (Ubuntu 9.3.0-17ubuntu1~20.04)
configuration: --prefix=/opt/ffmpeg/build --pkg-config-flags=--static --extra-cflags=-I/opt/ffmpeg/build/include --extra-ldflags=-L/opt/ffmpeg/build/lib --bindir=/opt/ffmpeg/build/bin --extra-libs=-lm --enable-shared --enable-gpl --enable-libass --enable-libfdk_aac --enable-libfreetype --enable-libopus --enable-libtheora --enable-libvorbis --enable-pthreads --enable-libvpx --enable-libx264 --enable-libx265 --enable-libxvid --enable-nonfree --disable-doc
libavutil      57. 18.100 / 57. 18.100
libavcodec     59. 20.100 / 59. 20.100
libavformat    59. 17.101 / 59. 17.101
libavdevice    59.  5.100 / 59.  5.100
libavfilter     8. 25.100 /  8. 25.100
libswscale      6.  5.100 /  6.  5.100
libswresample   4.  4.100 /  4.  4.100
libpostproc    56.  4.100 / 56.  4.100
```

