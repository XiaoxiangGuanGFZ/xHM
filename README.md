# xHM

Develop a raster-based distributed hydrological model to simulate catchment-scale hydrological processes with high spatial resolution. The model should support both daily and sub-daily discharge forecasting while improving process-based understanding of hydrological dynamics at the watershed scale.

The model will be implemented on a grid-based structure, where each cell represents a hydrological response unit (HRU) that interacts with neighboring cells. Fundamental input datasets include raster-based Digital Elevation Models (DEM) for terrain-driven flow routing, land cover maps for vegetation and infiltration characteristics, and soil maps to parameterize water retention and percolation properties. 



## Main processes

- canopy interception :heavy_check_mark:
- snow accumulation and melting :heavy_check_mark:
- evapotranspiration from canopy and soil :heavy_check_mark:
- soil infiltration, percolation, desorption :heavy_check_mark:
- subsurface (saturated) water movement :heavy_check_mark:
- overland routing :heavy_check_mark:
- river (channel) routing :heavy_check_mark:

## To-Do-List

1. water conservation test for runoff generation module
2. test hourly scale snow modeling and discharge simulation

## Outlook

- Employ parallel processing techniques to improve computational efficiency
- Reservoirs, lakes, and human-induced modifications to natural flow regimes
- Couple with groundwater models could enhance representation of baseflow contributions
- Integrate remote sensing and machine learning-based corrections
- Enhance applicability to urbanized catchments

## Contact
[Xiaoxiang Guan](https://www.gfz-potsdam.de/staff/guan.xiaoxiang/sec44)

