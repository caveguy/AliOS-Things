import os

src = Split('''
        bsp/entry.c
        bsp/heap_iram.c
        bsp/syscall.c
        bsp/key.c
        bsp/driver/interrupt.c
        bsp/driver/uart.c
        hal/uart.c
        hal/flash.c
        hal/misc.c
        hal/wifi_port.c
        hal/ota_port.c
        hal/upgrade_lib.c
        bsp/driver/gpio.c
        bsp/driver/hw_timer.c
        bsp/driver/i2c_master.c
        bsp/driver/spi_interface.c
''')

global_includes = Split('''
        bsp/include
        bsp/include/xtensa
        bsp/include/espressif
        bsp/include/espressif/esp8266
        bsp/include/lwip
        bsp/include/lwip/ipv4
        bsp/include/lwip/ipv6
        bsp/include/driver
''')

local_includes = []

dependencis = Split('''
        framework/common 
        kernel/modules/fs/kv 
        utility/libc
        kernel/protocols/net
        security/alicrypto
        kernel/hal
        kernel/vcall
        kernel/init
''')

global_cflags = Split('''
        -u call_user_start
        -fno-inline-functions
        -ffunction-sections
        -fdata-sections
        -mlongcalls
        -DESPOS_FOR_ESP8266
        -Wl,-static
''')

local_cflags = ['-std=gnu99']

global_ldflags = Split('''
        -nostdlib 
        -Lplatform/mcu/esp8266/bsp/lib 
        -Wl,--no-check-sections 
        -Wl,--gc-sections 
        -mtext-section-literals 
        -fno-builtin-printf 
        -Wl,-static 
        -u call_user_start 
        -Wl,-EL 
        -mlongcalls
''')

global_macro = Split('''
        CONFIG_AOS_KV_BUFFER_SIZE=8192
        CONFIG_ESP_LWIP
        XT_USE_THREAD_SAFE_CLIB=0
''')

prebuild_libs = Split('''
        bsp/lib/libhal.a
        bsp/lib/libcrypto.a
        bsp/lib/libmain.a
        bsp/lib/libcirom.a
        bsp/lib/libnet80211.a
        bsp/lib/libpp.a
        bsp/lib/libwpa.a
        bsp/lib/libphy.a
        bsp/lib/libgcc.a
        bsp/lib/liblwip.a
''')

ld_files = ['bsp/ld/eagle.app.v6.new.1024.app1.ld']
if aos_global_config.get_aos_global_config('wifi', 0) == 1:
    local_cflags.append('ENABLE_WIFI')
else:
    local_includes.append('bsp')

if aos_global_config.get_aos_global_config('vcall', 'rhino') == 'freertos':
    global_includes.append(os.path.join(os.getenv('SDK8266_PATH'), 'include/espos'))
    global_includes.append(os.path.join(os.getenv('SDK8266_PATH'), 'include/freertos'))
    prebuild_libs.append('lib/libespos.a')
    prebuild_libs.append('lib/lib/libfreertos.a')
else:
    dependencis.append('kernel/rhino')
    dependencis.append('platform/arch/xtensa/lx106')
    src.append('aos/hook_impl.c')
    src.append('aos/soc_impl.c')
    src.append('aos/trace_impl.c')

component = aos_arch_component('esp8266', src)
component.add_component_dependencis(*dependencis)

component.add_includes(*local_includes)
component.add_global_includes(*global_includes)


for cflag in global_cflags:
    component.add_global_cflags(cflag)
for cflag in local_cflags:
    component.add_cflags(cflag)

for ldflag in global_ldflags:
    component.add_global_ldflags(ldflag)

for lib in prebuild_libs:
    component.add_prebuilt_lib(lib)

for macro in global_macro:
    component.add_global_macro(macro)

for ld in ld_files:
    component.add_global_ld_file(ld)

# component.set_global_arch('Xtensa')
component.set_global_mcu_family('esp8266')

aos_global_config.set_aos_global_config('use_private_lwip', 1)

tool_chain = aos_global_config.create_tool_chain()
tool_chain.set_prefix('xtensa-lx106-elf-')
tool_chain.set_cppflags('-DSYSINFO_PRODUCT_MODEL=\\"ALI_AOS_ESP8266\\" -DSYSINFO_DEVICE_NAME=\\"ESP8266\\"')
tool_chain.set_linkcom('$LINK -o $TARGET -Wl,-Map,$MAPFILE -Wl,--start-group $LIBS  -Wl,--end-group -Wl,--gc-sections -Wl,--cref $LDFLAGS $LINKFLAGS')
aos_global_config.tool_chain_config(tool_chain)
