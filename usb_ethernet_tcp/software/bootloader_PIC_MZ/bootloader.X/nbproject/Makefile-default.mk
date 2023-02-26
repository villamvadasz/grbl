#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/main.c ../src/bootloader.c ../src/datastream_usb_device_hid.c ../src/drv_usbhs.c ../src/plib_evic.c ../src/plib_nvm.c ../src/sys_int.c ../src/usb_device.c ../src/usb_device_hid.c ../src/drv_usbhs_device.c ../src/usb_device_init_data.c ../src/bootloader_nvm_interface.c ../src/exceptions.c ../src/interrupts.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/bootloader.o ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o ${OBJECTDIR}/_ext/1360937237/plib_evic.o ${OBJECTDIR}/_ext/1360937237/plib_nvm.o ${OBJECTDIR}/_ext/1360937237/sys_int.o ${OBJECTDIR}/_ext/1360937237/usb_device.o ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o ${OBJECTDIR}/_ext/1360937237/exceptions.o ${OBJECTDIR}/_ext/1360937237/interrupts.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1360937237/bootloader.o.d ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d ${OBJECTDIR}/_ext/1360937237/plib_evic.o.d ${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d ${OBJECTDIR}/_ext/1360937237/sys_int.o.d ${OBJECTDIR}/_ext/1360937237/usb_device.o.d ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d ${OBJECTDIR}/_ext/1360937237/exceptions.o.d ${OBJECTDIR}/_ext/1360937237/interrupts.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/bootloader.o ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o ${OBJECTDIR}/_ext/1360937237/plib_evic.o ${OBJECTDIR}/_ext/1360937237/plib_nvm.o ${OBJECTDIR}/_ext/1360937237/sys_int.o ${OBJECTDIR}/_ext/1360937237/usb_device.o ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o ${OBJECTDIR}/_ext/1360937237/exceptions.o ${OBJECTDIR}/_ext/1360937237/interrupts.o

# Source Files
SOURCEFILES=../src/main.c ../src/bootloader.c ../src/datastream_usb_device_hid.c ../src/drv_usbhs.c ../src/plib_evic.c ../src/plib_nvm.c ../src/sys_int.c ../src/usb_device.c ../src/usb_device_hid.c ../src/drv_usbhs_device.c ../src/usb_device_init_data.c ../src/bootloader_nvm_interface.c ../src/exceptions.c ../src/interrupts.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MZ2048ECG144
MP_LINKER_FILE_OPTION=,--script="..\lkr\btl.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/bootloader.o: ../src/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bootloader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/bootloader.o.d" -o ${OBJECTDIR}/_ext/1360937237/bootloader.o ../src/bootloader.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o: ../src/datastream_usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d" -o ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o ../src/datastream_usb_device_hid.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/drv_usbhs.o: ../src/drv_usbhs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d" -o ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o ../src/drv_usbhs.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/plib_evic.o: ../src/plib_evic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_evic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_evic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/plib_evic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/plib_evic.o.d" -o ${OBJECTDIR}/_ext/1360937237/plib_evic.o ../src/plib_evic.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/plib_nvm.o: ../src/plib_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/1360937237/plib_nvm.o ../src/plib_nvm.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/sys_int.o: ../src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/sys_int.o.d" -o ${OBJECTDIR}/_ext/1360937237/sys_int.o ../src/sys_int.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device.o: ../src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device.o ../src/usb_device.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device_hid.o: ../src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o ../src/usb_device_hid.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o: ../src/drv_usbhs_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d" -o ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o ../src/drv_usbhs_device.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o: ../src/usb_device_init_data.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o ../src/usb_device_init_data.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o: ../src/bootloader_nvm_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d" -o ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o ../src/bootloader_nvm_interface.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/exceptions.o: ../src/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/exceptions.o.d" -o ${OBJECTDIR}/_ext/1360937237/exceptions.o ../src/exceptions.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/interrupts.o: ../src/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/interrupts.o.d" -o ${OBJECTDIR}/_ext/1360937237/interrupts.o ../src/interrupts.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
else
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/bootloader.o: ../src/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bootloader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/bootloader.o.d" -o ${OBJECTDIR}/_ext/1360937237/bootloader.o ../src/bootloader.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o: ../src/datastream_usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o.d" -o ${OBJECTDIR}/_ext/1360937237/datastream_usb_device_hid.o ../src/datastream_usb_device_hid.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/drv_usbhs.o: ../src/drv_usbhs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/drv_usbhs.o.d" -o ${OBJECTDIR}/_ext/1360937237/drv_usbhs.o ../src/drv_usbhs.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/plib_evic.o: ../src/plib_evic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_evic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_evic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/plib_evic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/plib_evic.o.d" -o ${OBJECTDIR}/_ext/1360937237/plib_evic.o ../src/plib_evic.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/plib_nvm.o: ../src/plib_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/plib_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/1360937237/plib_nvm.o ../src/plib_nvm.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/sys_int.o: ../src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/sys_int.o.d" -o ${OBJECTDIR}/_ext/1360937237/sys_int.o ../src/sys_int.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device.o: ../src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device.o ../src/usb_device.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device_hid.o: ../src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device_hid.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device_hid.o ../src/usb_device_hid.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o: ../src/drv_usbhs_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o.d" -o ${OBJECTDIR}/_ext/1360937237/drv_usbhs_device.o ../src/drv_usbhs_device.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o: ../src/usb_device_init_data.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o.d" -o ${OBJECTDIR}/_ext/1360937237/usb_device_init_data.o ../src/usb_device_init_data.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o: ../src/bootloader_nvm_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o.d" -o ${OBJECTDIR}/_ext/1360937237/bootloader_nvm_interface.o ../src/bootloader_nvm_interface.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/exceptions.o: ../src/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/exceptions.o.d" -o ${OBJECTDIR}/_ext/1360937237/exceptions.o ../src/exceptions.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
${OBJECTDIR}/_ext/1360937237/interrupts.o: ../src/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I"../inc" -MMD -MF "${OBJECTDIR}/_ext/1360937237/interrupts.o.d" -o ${OBJECTDIR}/_ext/1360937237/interrupts.o ../src/interrupts.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mdfp=${DFP_DIR}  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../lkr/btl.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_ICD4=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x27F   -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_ICD4=1,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -mdfp=${DFP_DIR}
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../lkr/btl.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -mdfp=${DFP_DIR}
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
