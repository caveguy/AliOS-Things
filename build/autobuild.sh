#!/usr/bin/env bash

linux_posix_targets="alinkapp meshapp networkapp"
linux_targets="alinkapp networkapp helloworld linuxapp meshapp tls yts linkkitapp"
linux_platforms="linuxhost linuxhost@debug linuxhost@release"
mk3060_targets="alinkapp helloworld linuxapp meshapp tls uDataapp networkapp"
mk3060_platforms="mk3060 mk3060@release"
b_l475e_targets="mqttapp helloworld tls uDataapp networkapp"
b_l475e_platforms="b_l475e"
starterkit_targets="ldapp"
starterkit_platforms="starterkit"
lpcxpresso54102_targets="helloworld alinkapp mqttapp tls networkapp"
lpcxpresso54102_platforms="lpcxpresso54102"
esp32_targets="alinkapp helloworld bluetooth.bleadv bluetooth.bleperipheral networkapp bluetooth.aisapp bluetooth.aisilopapp mqttapp"
esp32_platforms="esp32devkitc"
esp8266_targets="helloworld linkkitapp"
esp8266_platforms="esp8266"
mk3239_targets="bluetooth.ble_advertisements bluetooth.ble_show_system_time"
mk3239_platforms="mk3239"
pca10056_targets="bluetooth.bleperipheral bluetooth.aisilopapp bluetooth.aisapp"
pca10056_platforms="pca10056"
eml3047_targets="lorawan.lorawanapp lorawan.linklora"
eml3047_platforms="eml3047"

scons_build_targets="helloworld@b_l475e helloworld@starterkit"
scons_ide_targets="helloworld@b_l475e helloworld@starterkit"
ide_types="keil iar"

keil_iar_targets="helloworld@b_l475e"
compiler_types="armcc iar"

if [ "$(uname)" = "Linux" ]; then
    OS="Linux"
    keil_iar_targets=""
elif [ "$(uname)" = "Darwin" ]; then
    OS="OSX"
    linux_platforms=""
    keil_iar_targets=""
elif [ "$(uname | grep NT)" != "" ]; then
    OS="Windows"
    linux_platforms=""
    esp8266_platforms=""
else
    echo "error: unkonw OS"
    exit 1
fi
echo "OS: ${OS}"

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

for target in ${keil_iar_targets}; do
    for compiler in ${compiler_types}; do
        aos make clean > /dev/null 2>&1
        aos make ${target} COMPILER=${compiler} > ${target}_${compiler}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build make ${target} COMPILER=${compiler} at ${branch} branch succeed"
            rm -f ${target}_${compiler}@${branch}.log
        else
            echo -e "build make ${target} COMPILER=${compiler} at ${branch} branch failed, log:\n"
            cat ${target}_${compiler}@${branch}.log
            echo -e "\nbuild make ${target} COMPILER=${compiler} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#scons tmp
aos make clean > /dev/null 2>&1
for target in ${scons_build_targets}; do
    aos scons ${target} > ${target}@${branch}.log 2>&1
    if [ $? -eq 0 ]; then
        echo "build scons ${target} at ${branch} branch succeed"
        rm -f ${target}@${branch}.log
    else
        echo -e "build scons ${target} at ${branch} branch failed, log:\n"
        cat ${target}@${branch}.log
        echo -e "\nbuild ${target} at ${branch} branch failed"
        aos make clean > /dev/null 2>&1
        exit 1
    fi
done

#tarsfer test
aos make clean > /dev/null 2>&1
for target in ${scons_ide_targets}; do
    for ide in ${ide_types}; do
        aos scons ${target} IDE=${ide} > ${target}2IDE_${ide}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build scons ${target} IDE=${ide} at ${branch} branch succeed"
            rm -f ${target}2IDE_${ide}@${branch}.log
        else
            echo -e "build scons ${target} IDE=${ide} at ${branch} branch failed, log:\n"
            cat ${target}2IDE_${ide}@${branch}.log
            echo -e "\nbuild scons ${target} IDE=${ide} at ${branch} branch failed"
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

#single-bin, mk3060
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
:`
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
`

#single-bin, lpc54102
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

#single-bin, b_l475e
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

#single-bin, starterkit
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

#single-bin, esp32
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

#single-bin, esp8266
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

#single-bin, mk3239
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

#single-bin, pca10056
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

#single-bin, eml3047
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


aos make clean > /dev/null 2>&1
echo "build ${branch} branch succeed"

echo "----------check CODE-STYLE now----------"
#./build/astyle.sh
echo "----------------------------------------"
echo "----------check COPYRIGHT now-----------"
python ./build/copyright.py
echo "----------------------------------------"
