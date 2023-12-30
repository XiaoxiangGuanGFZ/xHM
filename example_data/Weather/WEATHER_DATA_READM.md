# Preparing the weather inputs
## Weather fields

The hydrological model requires 8 fundamental weather variables to drive the provess simulation. 
The scale_factor is used in NetCDF file configuration to convert all the data into integer data type in order to 
reduce storage. 

| abbreviation | variable name | unit | scale_factor |
| ------------ | ------------- | ---- | ------------ |
| PRE | precipitation | mm | 0.1 |
| PRS | atmospheric pressure | kPa | 1.0 |
| RHU | relative humidity | % | 1.0 |
| SSD | sunshine duration | hours | 1.0 |
| WIN | wind speed | m/s | 0.1 |
| TEM_AVG | average air temperature | C | 0.1 |
| TEM_MAX | maximum air temperature | C | 0.1 |
| TEM_MIN | minimum air temperature | C | 0.1 |

## Purpose of the WEATHER executable 
The data for the 8 variables is collected at either station scale or gridded scale. The WEATHER executable is designed to 
interpolate the station/gridded weather data into the spatial resolution on which the model is setup. 
The executable stores the weather data in a NetCDF file in the data type of `NC_INT` (rather than `NC_DOUBLE`) in order to decrease the storage consumption. 

## ASCII data preparation 
The raw weather data is stored in the form of 2D dataframe in a text file (.csv), together with the a separate geoinfo file where the fields `ID`, `lon`, `lat` and `DEM` are included. 
An independent file is required to instruct the program of the command arguments including file path of the data, number of stations, staring time of the weather variables, et al. 

the file list:
- parameter file:program global parameters/arguments
- raw weather data file: weather weather data is staored
- geoinfo data: store the geographic infor of the weather sites/grids
- GEO data: spatial extent and resolution on which the model is set up 

## Interpolation method
The **Inverse distance weighting** is applied to interpolate the weather data into the required spatial resolution for all the 8 variables. For the air temperature, the elapse rate of air temperature with altitude is considered as 0.65c per 100 meters. 

## How to use

```PowerShell
cd ./xHM/scr/
WEATHER.exe ../example_data/weather/Para_Weather_Chitan1km_PRE.txt
```




