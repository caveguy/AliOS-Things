#!/bin/sh

version=1.3.0
GITHUB_HEAD_SHA=9cf94a65537ba76dffc7325514bb04394b69125e

aosdir=${HOME}/githubsync/aos
githubdir=${HOME}/githubsync/AliOS
branch=aos${version}
branch_specific_files=${aosdir}/tools/codesync/shscripts/specific

cd ~/
if [ -f githubsync ]; then
    rm -f githubsync
fi
if [ ! -d githubsync ]; then
    mkdir githubsync
fi

if [ -d ${aosdir} ]; then
    rm -rf ${aosdir}
fi
git clone git@code.aliyun.com:keepwalking.zeng/aos.git ${aosdir}

if [ -d ${githubdir} ]; then
    cd ${githubdir}
    git status > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        cd ../
        rm -rf ${dir}
    fi
fi
if [ -d ${githubdir} ];then
    cd ${githubdir}
    git reset --hard HEAD
    git fetch
else
    cd ~/githubsync
    git clone git@github.com:alibaba/AliOS-Things.git AliOS
fi

cd ~/githubsync

cd ${aosdir}
git checkout -b ${branch} --track origin/${branch}
cd ${githubdir}
git reset --hard ${GITHUB_HEAD_SHA}
cd ~/githubsync

rm -rf ${githubdir}/*
rm -rf ${github}/.gitignore
rm -rf ${github}/.travis.yml
rm -rf ${github}/.vscode
cp -rf ${aosdir}/* ${githubdir}/
cp -rf ${aosdir}/.gitignore ${githubdir}/
cp -rf ${aosdir}/.travis.yml ${githubdir}/

#remove files from github dir
rm -rf ${githubdir}/board/armhflinux
rm -rf ${githubdir}/build/astyle
rm -rf ${githubdir}/build/astyle.sh
rm -rf ${githubdir}/build/copyright.py
rm -rf ${githubdir}/build/doxygen2md.py
rm -rf ${githubdir}/build/github_sync.sh
rm -rf ${githubdir}/build/MD.templet
rm -rf ${githubdir}/build/OpenOCD
rm -rf ${githubdir}/build/compiler/arm-none-eabi*
rm -rf ${githubdir}/platform/mcu/linux/csp/wifi/radiotap
rm -rf ${githubdir}/script
rm -rf ${githubdir}/platform/mcu/linux/csp/wifi
rm -rf ${githubdir}/platform/mcu/esp32/esptool_py
rm -rf ${githubdir}/platform/mcu/esp8266/esptool_py
rm -rf ${githubdir}/platform/arch/linux/swap.*
rm -rf ${githubdir}/example/mqttest
rm -rf ${githubdir}/bootloader

#yts tests
rm -rf ${githubdir}/test/testcase/kernel/protocols

#tools folder
rm -rf ${githubdir}/tools/*
cp -rf ${aosdir}/tools/at_adapter ${githubdir}/tools/
cp -rf ${aosdir}/tools/ci ${githubdir}/tools/
cp -rf ${aosdir}/tools/cli ${githubdir}/tools/
cp -rf ${aosdir}/tools/Doxyfile ${githubdir}/tools/
cp -rf ${aosdir}/tools/doxygen.sh ${githubdir}/tools/
mkdir -p ${githubdir}/tools/prebuild
cp -rf ${branch_specific_files}/prebuild.sh ${githubdir}/tools/prebuild/

#testbed
#cp -rf ${aosdir}/tools/testbed ${githubdir}/tools/
#rm -rf ${githubdir}/tools/testbed/testscripts
#rm -rf ${githubdir}/tools/testbed/utilities
#rm -rf ${githubdir}/tools/testbed/unittest
#rm -rf ${githubdir}/tools/testbed/board/emulator
#rm -rf ${githubdir}/tools/testbed/board/linuxhost
#rm -rf ${githubdir}/tools/testbed/server_*.pem
#cd ${aosdir}/tools/testbed/utilities/
#mkdir ${githubdir}/tools/testbed/utilities
#cp -f install.sh ${githubdir}/tools/testbed/utilities/
#cp -f testbed_setup/tb*_st* ${githubdir}/tools/testbed/utilities/
#cp -f testbed_setup/98-usb-serial.rules ${githubdir}/tools/testbed/utilities/
cd ${githubdir}
git checkout tools/testbed/
cd ~/githubsync

#ywss folder
rm -rf ${githubdir}/framework/ywss/*
mkdir ${githubdir}/framework/ywss/lib/
cp -f ${aosdir}/framework/ywss/enrollee.h ${githubdir}/framework/ywss/
cp -f ${aosdir}/framework/ywss/awss.h ${githubdir}/framework/ywss/

#ywss4linkkit folder
rm -rf ${githubdir}/framework/ywss4linkkit/*
cp -f ${aosdir}/framework/ywss4linkkit/libywss/os/os.h ${githubdir}/framework/ywss4linkkit/
cp -f ${aosdir}/framework/ywss4linkkit/libywss/os/product/product.h ${githubdir}/framework/ywss4linkkit/
cp -f ${branch_specific_files}/ywss4linkkit.mk ${githubdir}/framework/ywss4linkkit/

#mesh folder
rm -rf ${githubdir}/kernel/protocols/mesh/*
mkdir ${githubdir}/kernel/protocols/mesh/lib
mkdir ${githubdir}/kernel/protocols/mesh/include
mkdir ${githubdir}/kernel/protocols/mesh/include/ip
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_config.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_80211.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_hal.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_types.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/ip/lwip_adapter.h ${githubdir}/kernel/protocols/mesh/include/ip/

#moc108 folder
rm -rf ${githubdir}/platform/mcu/moc108/*
cd ${aosdir}/platform/mcu/moc108/
mkdir ${githubdir}/platform/mcu/moc108/include
mkdir ${githubdir}/platform/mcu/moc108/include/lwip-2.0.2
mkdir ${githubdir}/platform/mcu/moc108/include/app
mkdir ${githubdir}/platform/mcu/moc108/include/func
mkdir ${githubdir}/platform/mcu/moc108/include/os
mkdir ${githubdir}/platform/mcu/moc108/include/driver
mkdir ${githubdir}/platform/mcu/moc108/include/ip
cp -rf mx108/mx378/func/mxchip/lwip-2.0.2/port ${githubdir}/platform/mcu/moc108/include/lwip-2.0.2/
cp -rf mx108/mx378/common ${githubdir}/platform/mcu/moc108/include/
cp -rf mx108/mx378/app/config ${githubdir}/platform/mcu/moc108/include/app/
cp -rf mx108/mx378/func/include ${githubdir}/platform/mcu/moc108/include/func/
cp -rf mx108/mx378/os/include ${githubdir}/platform/mcu/moc108/include/os/
cp -rf mx108/mx378/driver/include ${githubdir}/platform/mcu/moc108/include/driver/
cp -rf mx108/mx378/driver/common ${githubdir}/platform/mcu/moc108/include/driver/
cp -rf mx108/mx378/ip/common ${githubdir}/platform/mcu/moc108/include/ip/
cp -rf mx108/mx378/driver/entry/*.h ${githubdir}/platform/mcu/moc108/include/
cp -rf mx108/mx378/build ${githubdir}/platform/mcu/moc108/linkinfo
find ${githubdir}/platform/mcu/moc108/ -type f -name '*.c' -exec rm {} +
cp -rf aos ${githubdir}/platform/mcu/moc108/
cp -rf hal ${githubdir}/platform/mcu/moc108/
cp -rf encrypt_linux ${githubdir}/platform/mcu/moc108/
cp -rf encrypt_osx ${githubdir}/platform/mcu/moc108/
cp -rf encrypt_win.exe ${githubdir}/platform/mcu/moc108/
cp -rf gen_crc_bin.mk ${githubdir}/platform/mcu/moc108/

#bluetooth ais
rm -rf ${githubdir}/example/bluetooth/aisapp
rm -rf ${githubdir}/framework/bluetooth/ais
rm -rf ${githubdir}/framework/bluetooth/ais_ilop/ali_lib/*
rm -rf ${githubdir}/framework/bluetooth/ais_ilop/ble_app_ali/*.c
mkdir ${githubdir}/framework/bluetooth/ais_ilop/ali_lib/include
cd ${githubdir}/framework/bluetooth/ais_ilop/
cp -f ${aosdir}/framework/bluetooth/ais_ilop/ali_lib/common/ali_common.h ali_lib/include/
cp -f ${branch_specific_files}/ali_lib.mk ali_lib/
cp -f ${branch_specific_files}/ble_app_ali.mk ble_app_ali/
sed -i "/^#include <ali_core.h>/d" ${githubdir}/example/bluetooth/aisilopapp/aisilopapp.c

#additional folders to delete
rm -rf ${githubdir}/kernel/rhino/posix
rm -rf ${githubdir}/example/legacy_linkkitapp
rm -rf ${githubdir}/framework/protocol/legacy_linkkit
rm -rf ${githubdir}/framework/protocol/alink
rm -rf ${githubdir}/example/alinkapp
rm -rf ${githubdir}/build/scons_enabled.*

#branch specofic files
cp ${branch_specific_files}/k_config_linuxhost.h ${githubdir}/board/linuxhost/k_config.h
cp ${branch_specific_files}/linux.mk ${githubdir}/platform/arch/linux/
cp ${branch_specific_files}/autobuild.sh ${githubdir}/build/
cp -f ${branch_specific_files}/linuxhost.mk ${githubdir}/board/linuxhost/linuxhost.mk
cp -f ${branch_specific_files}/yts_main.c ${githubdir}/example/yts/main.c
cp -f ${branch_specific_files}/yts.mk ${githubdir}/example/yts/yts.mk
cp -rf ${branch_specific_files}/ywss.mk ${githubdir}/framework/ywss/
cp -f ${branch_specific_files}/syscall_ftbl.c ${githubdir}/framework/fsyscall/syscall_ftbl.c
cp -f ${branch_specific_files}/mesh.mk ${githubdir}/kernel/protocols/mesh/
cp -f ${branch_specific_files}/syscall_ktbl.c ${githubdir}/kernel/ksyscall/syscall_ktbl.c
cp -f ${branch_specific_files}/moc108.mk ${githubdir}/platform/mcu/moc108/moc108.mk

cd ${aosdir}
target=meshapp@linuxhost
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
cp mesh.a libmesh.a
strip --strip-debug libmesh.a
mkdir ${githubdir}/kernel/protocols/mesh/lib/linux
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/linux/libmesh.a

cd ${aosdir}
target=alinkapp@linuxhost
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
cp ywss.a libywss.a
strip --strip-debug libywss.a
mkdir ${githubdir}/framework/ywss/lib/linux
cp libywss.a ${githubdir}/framework/ywss/lib/linux/libywss.a

cd ${aosdir}
target=alinkapp@mk3060
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
cp mesh.a libmesh.a
arm-none-eabi-strip --strip-debug libmesh.a
mkdir ${githubdir}/kernel/protocols/mesh/lib/arm968es
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/arm968es/libmesh.a
cp ywss.a libywss.a
arm-none-eabi-strip --strip-debug libywss.a
mkdir ${githubdir}/framework/ywss/lib/arm968es
cp libywss.a ${githubdir}/framework/ywss/lib/arm968es/libywss.a
echo "create libmoc108.a" > packscript
echo "addlib moc108.a" >> packscript
echo "addlib entry.a" >> packscript
echo "addlib hal_init.a" >> packscript
echo "addlib ${aosdir}/platform/mcu/moc108/librwnx/librwnx.a" >> packscript
echo "save" >> packscript
echo "end" >> packscript
arm-none-eabi-ar -M < packscript
arm-none-eabi-strip --strip-debug libmoc108.a
mv libmoc108.a ${githubdir}/platform/mcu/moc108/
cd ${githubdir}/platform/mcu/moc108/
arm-none-eabi-ar dv libmoc108.a aos_main.o
arm-none-eabi-ar dv libmoc108.a soc_impl.o
arm-none-eabi-ar dv libmoc108.a trace_impl.o
arm-none-eabi-ar dv libmoc108.a mesh_wifi_hal.o

cd ${aosdir}
target=alinkapp@esp32devkitc
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
cp mesh.a libmesh.a
xtensa-esp32-elf-strip --strip-debug libmesh.a
mkdir ${githubdir}/kernel/protocols/mesh/lib/xtensa
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/xtensa/libmesh.a
cp ywss.a libywss.a
xtensa-esp32-elf-strip --strip-debug libywss.a
mkdir ${githubdir}/framework/ywss/lib/xtensa
cp libywss.a ${githubdir}/framework/ywss/lib/xtensa/libywss.a

cd ${aosdir}
target=alinkapp@lpcxpresso54102
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
cp ywss.a libywss.a
arm-none-eabi-strip --strip-debug libywss.a
mkdir ${githubdir}/framework/ywss/lib/cortex-m4
cp libywss.a ${githubdir}/framework/ywss/lib/cortex-m4/libywss.a

cd ${aosdir}
target=alinkapp@amebaz_dev
aos make ${target} > /dev/null
#if [ $? -ne 0 ]; then
#    echo "error: build ${target} failed"
#    exit 1
#fi
cd ${aosdir}/out/${target}/libraries/
cp mesh.a libmesh.a
arm-none-eabi-strip --strip-debug libmesh.a
mkdir ${githubdir}/kernel/protocols/mesh/lib/cortex-m4
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/cortex-m4/libmesh.a

cd ${aosdir}
target=bluetooth.aisilopapp@pca10056
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
arm-none-eabi-strip --strip-debug ali_lib.a
arm-none-eabi-strip --strip-debug ble_app_ali.a
mkdir -p ${githubdir}/framework/bluetooth/ais_ilop/ali_lib/lib/cortex-m4
mv ali_lib.a ${githubdir}/framework/bluetooth/ais_ilop/ali_lib/lib/cortex-m4/
mkdir -p ${githubdir}/framework/bluetooth/ais_ilop/ble_app_ali/lib/cortex-m4
mv ble_app_ali.a ${githubdir}/framework/bluetooth/ais_ilop/ble_app_ali/lib/cortex-m4/

cd ${aosdir}
target=linkkitapp@mk3060
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
arm-none-eabi-strip --strip-debug libywss.a
mkdir -p ${githubdir}/framework/ywss4linkkit/lib/arm968es
cp libywss.a ${githubdir}/framework/ywss4linkkit/lib/arm968es/libywss.a

cd ${aosdir}
target=linkkitapp@esp8266
aos make -e ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
xtensa-lx106-elf-strip --strip-debug libywss.a
mkdir -p ${githubdir}/framework/ywss4linkkit/lib/xtensa
cp libywss.a ${githubdir}/framework/ywss4linkkit/lib/xtensa/libywss.a

cd ${aosdir}
target=linkkitapp@linuxhost
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
strip --strip-debug libywss.a
mkdir -p ${githubdir}/framework/ywss4linkkit/lib/linux
cp libywss.a ${githubdir}/framework/ywss4linkkit/lib/linux/libywss.a

cd ${aosdir}
target=linkkitapp@amebaz_dev
aos make ${target} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: build ${target} failed"
    exit 1
fi
cd ${aosdir}/out/${target}/libraries/
arm-none-eabi-strip --strip-debug libywss.a
mkdir -p ${githubdir}/framework/ywss4linkkit/lib/cortex-m4
cp libywss.a ${githubdir}/framework/ywss4linkkit/lib/cortex-m4/libywss.a

cd ${githubdir}
grep -rl "AOS-R-[0-9]\.[0-9]\.[0-9]" | xargs sed -i "s|AOS-R-[0-9].[0-9].[0-9]|AOS-R-${version}|g"
git add -A
datetime=`date +%F@%H:%M`
git commit -m "code synchronization at ${datetime}" > /dev/null
#git push -f origin master
