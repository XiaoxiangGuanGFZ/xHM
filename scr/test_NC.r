library(ncdf4)

nc <- nc_open("D:/xHM/example_data/GEO_data.nc")
nc
df_dem <- ncvar_get(nc, "DEM")
df_fdr <- ncvar_get(nc, "FDR")
View(df_dem)
View(df_fdr)

ncvar_get(nc, "lon")
ncvar_get(nc, "lat")
des = "D8 direction type: 1(East/Right); 2(SouthEast/LowerRight);4(South/Down); 8(SouthWest/Lowerleft); 16(West/Left); 32(NorthWest/UpperLeft); 64(North/Up); 128(NorthEst/UpperRight)"
nchar(des)

nc_close(nc)
