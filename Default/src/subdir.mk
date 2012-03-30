################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/3d_view.cpp \
../src/contour_format.cpp \
../src/midpoint_displacement.cpp \
../src/rivers.cpp \
../src/settlements.cpp \
../src/terrain_generator.cpp \
../src/vegetation.cpp 

OBJS += \
./src/3d_view.o \
./src/contour_format.o \
./src/midpoint_displacement.o \
./src/rivers.o \
./src/settlements.o \
./src/terrain_generator.o \
./src/vegetation.o 

CPP_DEPS += \
./src/3d_view.d \
./src/contour_format.d \
./src/midpoint_displacement.d \
./src/rivers.d \
./src/settlements.d \
./src/terrain_generator.d \
./src/vegetation.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


