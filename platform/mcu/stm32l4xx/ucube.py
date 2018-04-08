src =Split(''' 
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c
    aos/soc_impl.c
    aos/trace_impl.c
''')
component =aos_arch_component('stm32l4xx', src)

HOST_MCU_NAME = aos_global_config.get_aos_global_config('HOST_MCU_NAME')
dependencis =Split(''' 
    platform/arch/arm/armv7m
    utility/libc
    kernel/rhino
    kernel/hal
    kernel/vfs
    utility/digest_algorithm
''')
for i in dependencis:
    component.add_component_dependencis(i)

global_includes =Split(''' 
    Drivers/STM32L4xx_HAL_Driver/Inc
    Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32L4xx/Include
''')
for i in global_includes:
    component.add_global_includes(i)
    
component.add_global_includes('src/'+HOST_MCU_NAME+'/runapp')
component.add_global_includes('src/'+HOST_MCU_NAME+'/hal')
component.add_global_includes('src/'+HOST_MCU_NAME)
component.add_sources('src/'+HOST_MCU_NAME+'/runapp/soc_init.c')
component.add_sources('src/'+HOST_MCU_NAME+'/runapp/stm32l4xx_hal_msp.c')
component.add_sources('src/'+HOST_MCU_NAME+'/runapp/stm32l4xx_it.c')
component.add_sources('src/'+HOST_MCU_NAME+'/runapp/system_stm32l4xx.c')
component.add_sources('src/'+HOST_MCU_NAME+'/runapp/aos.c')
component.add_sources('src/'+HOST_MCU_NAME+'/hal/hal_uart_stm32l4.c')
component.add_sources('src/'+HOST_MCU_NAME+'/hal/hw.c')
component.add_sources('src/'+HOST_MCU_NAME+'/hal/flash_l4.c')
component.add_sources('src/'+HOST_MCU_NAME+'/hal/flash_port.c')

global_macros =Split(''' 
    CONFIG_AOS_KV_MULTIPTN_MODE
    CONFIG_AOS_KV_PTN=6
    CONFIG_AOS_KV_SECOND_PTN=7
    CONFIG_AOS_KV_PTN_SIZE=4096
    CONFIG_AOS_KV_BUFFER_SIZE=8192
''')
for i in global_macros:
    component.add_global_macro(i)

if HOST_MCU_NAME == 'STM32L433RC-Nucleo':
    component.add_global_macro('STM32L433xx')
    component.add_sources('src/'+HOST_MCU_NAME+'/hal/hal_gpio_stm32l4.c')
    component.add_sources('src/'+HOST_MCU_NAME+'/hal/hal_i2c_stm32l4.c')
elif HOST_MCU_NAME == 'STM32L432KC-Nucleo':
    component.add_global_macro('STM32L432xx')
    
    
if aos_global_config.compiler == 'armcc':    
    component.add_sources('src/STM32L433RC-Nucleo/startup_stm32l433xx_keil.s')
elif  aos_global_config.compiler == 'iar':
    component.add_sources('src/STM32L433RC-Nucleo/startup_stm32l433xx_iar.s')
else:
    component.add_sources('src/STM32L433RC-Nucleo/startup_stm32l433xx.s')

global_cflags = []

if aos_global_config.compiler == 'armcc':    
    global_cflags = Split(''' 
        --c99 
        --cpu=7E-M 
        -D__MICROLIB 
        -g 
        --apcs=interwork 
        --split_sections
    ''')
elif  aos_global_config.compiler == 'iar':
    global_cflags = Split(''' 
        --cpu=Cortex-M4
        --cpu_mode=thumb
        --endian=little
    ''')
else:    
    global_cflags = Split(''' 
        -mcpu=cortex-m4
        -march=armv7-m
        -mlittle-endian
        -mthumb -mthumb-interwork
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -w
        -D__VFP_FP__
    ''')    

for i in global_cflags:  
    component.add_global_cflags(i)
    
global_asflags = []
if aos_global_config.compiler == 'armcc':    
    global_asflags = Split(''' 
        --cpu=7E-M 
        -g 
        --apcs=interwork 
        --library_type=microlib 
        --pd 
        "__MICROLIB SETA 1"
    ''')
elif  aos_global_config.compiler == 'iar':
    global_asflags = Split(''' 
        --cpu
        Cortex-M4
        --cpu_mode 
        thumb
        --endian 
        little
    ''')
else:    
    global_asflags = Split(''' 
        -mcpu=cortex-m4
        -mlittle-endian
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -w
    ''')    

for i in global_asflags:  
    component.add_global_asflags(i)    
    
global_ldflags = []
if aos_global_config.compiler == 'armcc':    
    global_ldflags = Split(''' 
        -L 
        --cpu=7E-M
        -L 
        --strict
        -L
        --xref 
        -L 
        --callgraph 
        -L 
        --symbols
        -L 
        --info=sizes 
        -L 
        --info=totals 
        -L 
        --info=unused 
        -L 
        --info=veneers 
        -L 
        --info=summarysizes
        -L       
        --scatter=platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.sct
    ''')
elif  aos_global_config.compiler == 'iar':
    global_ldflags = Split(''' 
        --silent 
        --cpu=Cortex-M4.vfp
         --config
         platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
    ''')
else:    
    global_ldflags = Split(''' 
        -mcpu=cortex-m4
        -mlittle-endian
        -mthumb 
        -mthumb-interwork
        --specs=nosys.specs
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -T 
        platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.ld
    ''')    

for i in global_ldflags:  
    component.add_global_ldflags(i)     

if aos_global_config.compiler == 'armcc':     
    component.add_external_obj('src/STM32L433RC-Nucleo/startup_stm32l433xx_keil.o')
    component.add_external_obj('src/'+HOST_MCU_NAME+'/runapp/stm32l4xx_it.o')
    component.add_external_obj('src/'+HOST_MCU_NAME+'/runapp/stm32l4xx_hal_msp.o')

component.add_component_dependencis('kernel/vcall')
component.add_component_dependencis('kernel/init')
    
    
    

