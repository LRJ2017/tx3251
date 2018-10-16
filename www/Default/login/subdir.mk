################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../login/cgic.c \
../login/login.c 

OBJS += \
./login/cgic.o \
./login/login.o 

C_DEPS += \
./login/cgic.d \
./login/login.d 


# Each subdirectory must supply rules for building sources it contributes
login/%.o: ../login/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


