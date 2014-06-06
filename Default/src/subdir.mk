################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/contour_format.cpp \
../src/contour_output.cpp \
../src/midpoint_displacement.cpp \
../src/rivers.cpp \
../src/settlements.cpp \
../src/terrain_generator.cpp \
../src/vegetation.cpp 

OBJS += \
./src/contour_format.o \
./src/contour_output.o \
./src/midpoint_displacement.o \
./src/rivers.o \
./src/settlements.o \
./src/terrain_generator.o \
./src/vegetation.o 

CPP_DEPS += \
./src/contour_format.d \
./src/contour_output.d \
./src/midpoint_displacement.d \
./src/rivers.d \
./src/settlements.d \
./src/terrain_generator.d \
./src/vegetation.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I"C:\Users\sashman\terrain_generator\rapidjson" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


