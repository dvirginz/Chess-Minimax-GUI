################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Advisor.c \
../GameBoard.c \
../Test_XML.c \
../Tools.c \
../XML.c \
../array.c \
../chess_parser.c \
../main.c \
../parser_unit_test.c 

O_SRCS += \
../Advisor.o \
../GameBoard.o \
../Tools.o \
../array.o \
../main.o 

OBJS += \
./Advisor.o \
./GameBoard.o \
./Test_XML.o \
./Tools.o \
./XML.o \
./array.o \
./chess_parser.o \
./main.o \
./parser_unit_test.o 

C_DEPS += \
./Advisor.d \
./GameBoard.d \
./Test_XML.d \
./Tools.d \
./XML.d \
./array.d \
./chess_parser.d \
./main.d \
./parser_unit_test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


