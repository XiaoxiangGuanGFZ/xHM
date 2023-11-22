# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

enable_language(C)

# Set the C compiler
# Replace "path/to/your/gcc.exe" with the actual path to your GCC compiler executable

set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")  
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")  # Uncomment and replace "path/to/your/g++.exe" if you are using C++

# Add your source files here
set(SNOW_module
    Snow_main.c
    SnowAccuMelt.c
    SnowAtmosphericStability.c
    SnowEnergy.c
    Radiation_Calc.c
)

project(kNN_MOF_cp)  # Set your project name here
# Add the executable target
add_executable(snow ${SNOW_module})


## cmake -G "MinGW Makefiles" .
## mingw32-make
