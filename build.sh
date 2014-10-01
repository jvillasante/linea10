#!/bin/bash -ex

if [ "$#" -ne 2 ]; then
  echo "Usage: ./build modo(debug|release) plataforma(target|host)"
  exit 
fi

if [ "$1" = "debug" ]; then
  configs="debug"
elif [ "$1" = "release" ]; then
  configs="release"
else
  echo "Usage: ./build modo(debug|release) plataforma(target|host)"
  exit 
fi

if [ "$2" = "target" ]; then
  defines="TARGET"
  spec="qws/linux-arm-gnueabi-g++"
  qmake="/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/qmake"
  export PATH=/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin:/opt/toolchain/arm-2009q3/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:$PATH
elif [ "$2" = "host" ]; then
  defines="HOST"
  spec="linux-g++"
  qmake="/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/HOST/bin/qmake"
  export PATH=/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/HOST/bin:/opt/toolchain/arm-2009q3/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:$PATH
else
  echo "Usage: ./build modo(debug|release) plataforma(target|host)"
  exit 
fi

# tempo
$qmake $PWD/SRC/tempo/tempo.pro -r -spec $spec \
  CONFIG+=$configs DEFINES+=$defines
make

# updater
$qmake $PWD/SRC/updater/updater.pro -r -spec $spec \
  CONFIG+=$configs DEFINES+=$defines
make

# synchronizer
$qmake $PWD/SRC/synchronizer/sychronizer.pro -r -spec $spec \
  CONFIG+=$configs DEFINES+=$defines
make

# backup
$qmake $PWD/SRC/backup/backup.pro -r -spec $spec \
  CONFIG+=$configs DEFINES+=$defines
make

