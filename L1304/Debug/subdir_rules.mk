################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs2011/ccs/tools/compiler/ti-cgt-armllvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug" -I"D:/ti/mspm0_sdk_2_04_00_06/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_04_00_06/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1938932431: ../empty_mspm0l1304.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/ccs2011/ccs/utils/sysconfig_1.23.0/sysconfig_cli.bat" --script "C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/empty_mspm0l1304.syscfg" -o "syscfg" -s "D:/ti/mspm0_sdk_2_04_00_06/.metadata/product.json" -d "MSPM0L130X" -p "SOT-16(DYY)" -r "Default" --context "system" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/device_linker.cmd: build-1938932431 ../empty_mspm0l1304.syscfg
syscfg/device.opt: build-1938932431
syscfg/device.cmd.genlibs: build-1938932431
syscfg/ti_msp_dl_config.c: build-1938932431
syscfg/ti_msp_dl_config.h: build-1938932431
syscfg/Event.dot: build-1938932431
syscfg: build-1938932431

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs2011/ccs/tools/compiler/ti-cgt-armllvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug" -I"D:/ti/mspm0_sdk_2_04_00_06/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_04_00_06/source" -gdwarf-3 -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0l130x_ticlang.o: D:/ti/mspm0_sdk_2_04_00_06/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0l130x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs2011/ccs/tools/compiler/ti-cgt-armllvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug" -I"D:/ti/mspm0_sdk_2_04_00_06/source/third_party/CMSIS/Core/Include" -I"D:/ti/mspm0_sdk_2_04_00_06/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/1/workspace_ccstheia/empty_mspm0l1304_nortos_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


