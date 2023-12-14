library(ncdf4)

nc <- nc_open("D:/xHM/example_data/GEO_data.nc")
nc
df_dem <- ncvar_get(nc, "DEM")
df_dem <- as.data.frame(t(df_dem))
dim(df_dem)
View(df_dem)

df_fdr <- ncvar_get(nc, "FDR")
df_fdr <- t(df_fdr)
View(t(df_fdr))

ncvar_get(nc, "lon")
ncvar_get(nc, "lat")
des = "D8 direction type: 1(East/Right); 2(SouthEast/LowerRight);4(South/Down); 8(SouthWest/Lowerleft); 16(West/Left); 32(NorthWest/UpperLeft); 64(North/Up); 128(NorthEst/UpperRight)"
nchar(des)

nc_close(nc)

# ---- check the function of 2ASCII ----
ncols = 457

fname = "outlets.txt"
df_raw <- read.table(
  paste0("D:/example_data/", fname),
  header = F, sep = ' ', skip = 6
)
df_out <- read.table(
  paste0("D:/xHM/example_data/", fname),
  header = F, sep = ' ', skip = 6
)
dim(df_out)

for (i in 1:ncols) {
  error = abs(sum(df_raw[, i] - df_out[, i]))
  print(
    paste0("row: ", i, "-ABSE:", error)
  )
}

#
