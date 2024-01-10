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

set(GEO
    GEO_main.c
    GEO2NC.c
    GEO2ASCII.c
    GEO_para.c
    NC_copy_global_att.c
)

set(WEATHER
    Weather2NC.c
    Weather2NC_main.c
    NC_copy_global_att.c
    NetCDF_IO_geo.c
)

set(UH
    UH_Generation.c
    UH_GENE_main.c
    NC_copy_global_att.c
)

set(ET
    Evaporation_soil.c
    Evapotranspiration.c
	Evapotranspiration_Energy.c
	Evapotranspiration_main.c
    Radiation_Calc.c
    Resistance.c
    Calendar.c
    Lookup_VegLib.c
    NC_copy_global_att.c
)

set(xHM
    Evaporation_soil.c
    Evapotranspiration.c
	Evapotranspiration_Energy.c
	xHM_main.c
    Radiation_Calc.c
    Resistance.c
    Calendar.c
    Soil_Desorption.c
    NC_copy_global_att.c
    HM_GlobalPara.c
    OutNamelist.c
    Check_Data.c
    Initial_VAR.c
    Lookup_VegLib.c
    Lookup_SoilLib.c
    NetCDF_IO_geo.c
    UH_Generation.c
)


project(xHM)  # Set your project name here

# Specify the path to NetCDF include directory
include_directories("C:/netCDF4.9.2/include")

# Specify the path to NetCDF libraries directory
link_directories("C:/netCDF4.9.2/lib")

# Add the executable target
add_executable(SNOW ${SNOW_module})
add_executable(GEO ${GEO})
add_executable(WEATHER ${WEATHER})
add_executable(UH ${UH})
# add_executable(ET ${ET})
add_executable(xHM ${xHM})


# Link NetCDF libraries
target_link_libraries(GEO PRIVATE netcdf)
target_link_libraries(WEATHER PRIVATE netcdf)
target_link_libraries(UH PRIVATE netcdf)
# target_link_libraries(ET PRIVATE netcdf)
target_link_libraries(xHM PRIVATE netcdf)

## cmake -G "MinGW Makefiles" .
## mingw32-make
