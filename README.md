# xHM
(cross-scale难说) hydrological model

## Main processes
- canopy interception
- snow accumulation and melting
- evapotranspiration
- soil moisture dynamics
- infiltration and surface (excess infiltration) runoff
- deep percolation, desorption
- subsurface (saturated) storage and saturated zone runoff 
- overland routing
- river (channel) routing

## To-Do-List
- test snow model for other stations (areas), more test studies
- test the parameter sensitivity
- test hourly scale snow modeling 

- split the functions into more independent source code files
- give more, detailed explanation for each function,as much as possible 
- give each source code file a signature
- how to describe the relationship among different source code files, put it at the beginning of each source code file ?!
- header (& source code) files for initializing the state (hydrological processes)
- header (& source code) files for data input 
- header (& source code) files for look-up table 
- test individual modules
- Netcdf read and write
- watershed delineation (DEM fill, flow direction, flow accumulation, riverlinks, subbasins)


## Contact
[Xiaoxiang Guan](https://www.gfz-potsdam.de/staff/guan.xiaoxiang/sec44)
