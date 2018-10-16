################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cgic.c \
../cgictest.c \
../config.c \
../ipconfig.c \
../waterconfig.c 

OBJS += \
./cgic.o \
./cgictest.o \
./config.o \
./ipconfig.o \
./waterconfig.o 

C_DEPS += \
./cgic.d \
./cgictest.d \
./config.d \
./ipconfig.d \
./waterconfig.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


