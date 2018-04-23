#!/usr/bin/env bash

linux_posix_targets="alinkapp meshapp networkapp"
linux_targets="alinkapp networkapp helloworld linuxapp meshapp tls yts linkkitapp"
linux_platforms="linuxhost linuxhost@debug linuxhost@release"
mk3060_targets="alinkapp helloworld linuxapp meshapp tls uDataapp networkapp linkkitapp"
mk3060_platforms="mk3060 mk3060@release"
b_l475e_targets="nano helloworld mqttapp alinkapp tls uDataapp networkapp"
b_l475e_platforms="b_l475e"
starterkit_targets="ldapp helloworld mqttapp"
starterkit_platforms="starterkit"
lpcxpresso54102_targets="helloworld alinkapp mqttapp tls networkapp"
lpcxpresso54102_platforms="lpcxpresso54102"
esp32_targets="alinkapp helloworld bluetooth.bleadv bluetooth.bleperipheral networkapp bluetooth.aisilopapp mqttapp"
esp32_platforms="esp32devkitc"
esp8266_targets="helloworld linkkitapp"
esp8266_platforms="esp8266"
mk3239_targets="bluetooth.ble_advertisements bluetooth.ble_show_system_time"
mk3239_platforms="mk3239"
pca10056_targets="bluetooth.bleperipheral bluetooth.aisilopapp"
pca10056_platforms="pca10056"
eml3047_targets="lorawan.lorawanapp lorawan.linklora"
eml3047_platforms="eml3047"
csky_targets="helloworld coapapp"
csky_platforms=""

keil_iar_targets="nano@b_l475e helloworld@b_l475e mqttapp@b_l475e alinkapp@b_l475e helloworld@starterkit mqttapp@starterkit"
compiler_types="armcc iar"
build_system="make"
build_tools="iar armcc"
build_ide="iar keil"

if [ "$(uname)" = "Linux" ]; then
    CUR_OS="Linux"
    keil_iar_targets=""
    csky_platforms="hobbit1_evb"
elif [ "$(uname)" = "Darwin" ]; then
    CUR_OS="OSX"
    linux_platforms=""
    keil_iar_targets=""
elif [ "$(uname | grep NT)" != "" ]; then
    CUR_OS="Windows"
    linux_platforms=""
    esp8266_platforms=""
else
    echo "error: unkonw OS"
    exit 1
fi
echo "CUR_OS: ${CUR_OS}"

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: not in any git repository"
    exit 1
fi

JNUM=`cat /proc/cpuinfo | grep processor | wc -l`

if [ -f ~/.bashrc ]; then
    . ~/.bashrc
fi

branch=`git status | grep "On branch" | sed -r 's/.*On branch //g'`
cd $(git rev-parse --show-toplevel)


for s in ${build_system}; do
    for i in ${keil_iar_targets}; do
        for t in ${build_tools}; do
            aos make clean > /dev/null 2>&1
            aos ${s} ${i} COMPILER=${t} > ${s}_${i}_${t}@${branch}.log 2>&1

            if [ $? -eq 0 ]; then
                echo -e "build aos ${s} ${i} COMPILER=${t} at ${branch} branch succeed"
                rm -f ${s}_${i}_${t}@${branch}.log
            else
                echo -e "build aos ${s} ${i} COMPILER=${t} at ${branch} branch failed, log:\n"
                cat ${s}_${i}_${t}@${branch}.log
                echo -e "\nbuild aos ${s} ${i} COMPILER=${t} at ${branch} branch failed"
                aos make clean > /dev/null 2>&1
                exit 1
            fi
        done
    done
done

#ide tarsfer test
for target in ${keil_iar_targets}; do
    for ide in ${build_ide}; do
        aos make clean > /dev/null 2>&1
        aos make ${target} IDE=${ide} > ${target}2IDE_${ide}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build aos make ${target} IDE=${ide} at ${branch} branch succeed"
            rm -f ${target}2IDE_${ide}@${branch}.log
        else
            echo -e "build aos make ${target} IDE=${ide} at ${branch} branch failed, log:\n"
            cat ${target}2IDE_${ide}@${branch}.log
            echo -e "\nbuild make ${target} IDE=${ide} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#linuxhost posix
aos make clean > /dev/null 2>&1
for target in ${linux_posix_targets}; do
    for platform in ${linux_platforms}; do
        vcall=posix aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build vcall=posix ${target}@${platform} at ${branch} branch succeed"
            rm -f ${target}@${platform}@${branch}.log
        else
            echo -e "build vcall=posix ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#linuxhost
aos make clean > /dev/null 2>&1
for target in ${linux_targets}; do
    for platform in ${linux_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build ${target}@${platform} at ${branch} branch succeed"
            rm -f ${target}@${platform}@${branch}.log
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#mk3060
aos make clean > /dev/null 2>&1
for target in ${mk3060_targets}; do
    for platform in ${mk3060_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#multi-bins, mk3060
bins_type=""
aos make clean > /dev/null 2>&1
for target in ${mk3060_targets}; do
    for platform in ${mk3060_platforms}; do
        for bins in ${bins_type}; do
            if [ "${target}" = "tls" ] || [ "${target}" = "meshapp" ]; then
                continue
            fi
            aos make ${target}@${platform} BINS=${bins} JOBS=${JNUM} > ${target}@${platform}@${bins}@${branch}.log 2>&1
            if [ $? -eq 0 ]; then
                rm -f ${target}@${platform}@${bins}@${branch}.log
                echo "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch succeed"
            else
                echo -e "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed, log:\n"
                cat ${target}@${platform}@${bins}@${branch}.log
                rm -f ${target}@${platform}@${bins}@${branch}.log
                echo -e "\nbuild ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed"
                aos make clean > /dev/null 2>&1
                exit 1
            fi
        done
    done
done

#lpc54102
aos make clean > /dev/null 2>&1
for target in ${lpcxpresso54102_targets}; do
    for platform in ${lpcxpresso54102_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#b_l475e
aos make clean > /dev/null 2>&1
for target in ${b_l475e_targets}; do
    for platform in ${b_l475e_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#starterkit
aos make clean > /dev/null 2>&1
for target in ${starterkit_targets}; do
    for platform in ${starterkit_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#esp32
aos make clean > /dev/null 2>&1
for target in ${esp32_targets}; do
    for platform in ${esp32_platforms}; do
        aos make ${target}@${platform} wifi=1 JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#esp8266
aos make clean > /dev/null 2>&1
for target in ${esp8266_targets}; do
    for platform in ${esp8266_platforms}; do
        aos make ${target}@${platform} wifi=1 JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#mk3239
aos make clean > /dev/null 2>&1
for target in ${mk3239_targets}; do
    for platform in ${mk3239_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#pca10056
aos make clean > /dev/null 2>&1
for target in ${pca10056_targets}; do
    for platform in ${pca10056_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#eml3047
aos make clean > /dev/null 2>&1
for target in ${eml3047_targets}; do
    for platform in ${eml3047_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#csky
aos make clean > /dev/null 2>&1
for target in ${csky_targets}; do
    for platform in ${csky_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -f ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -f ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

aos make clean > /dev/null 2>&1
echo "build ${branch} branch succeed"

echo "----------check CODE-STYLE now----------"
#./build/astyle.sh
echo "----------------------------------------"
echo "----------check COPYRIGHT now-----------"
python ./build/copyright.py
echo "----------------------------------------"
