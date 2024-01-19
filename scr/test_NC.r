library(ncdf4)
fname = "D:/xHM/example_data/JN_GEO_1km/GEO_data.nc"
nc <- nc_open(fname)
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

# -------- weather nc -------

fp <- "D:/xHM/example_data/weather/weather_Chitan1km.nc"
nc <- nc_open(fp)
nc
df <- ncvar_get(nc, "PRE")
dim(df)

df_clip<- as.data.frame(t(df[,,16]))
dim(df_clip)
View(df_clip)

nc_close(nc)

# -------  UH.nc -----
fp = "D:/xHM/example_data/JN_GEO_1km/UH.nc"
nc <- nc_open(fp)
nc
df <- ncvar_get(nc, "UH0")
dim(df)

df_clip<- as.data.frame(t(df[,,60]))
dim(df_clip)

View(
  round(df_clip, 4)
)

nc_close(nc)

sum(df[367, 38, ]) * 24

UH <- function(t, step)
{
  uh = NULL
  Ts = 235.938  # hours
  Tr = 235.938
  for (i in 1:length(t)) {
    if (t[i] < Ts){
      uh[i] = 0
    } else if (t[i] < Ts + step) 
    {
      # uh[i] = 1 / step * (1 - exp((t[i] - Ts) / Tr))
      uh[i] = 1 / step * (exp(1) - exp(1 - (t[i] - Ts) / Tr))
    } else {
      uh[i] = 1 / step * exp( - (t[i] - Ts) / Tr) * (exp(step/Tr) - 1)
    }
  }
  return(uh)
}

step = 1
t = seq(step, step * 80 * 24, step)
uh = UH(t, step)
sum(abs(uh)) * step

uh = UH(t, step)

x = seq(0.1, 1.0,0.1)
exp(x)

# ----------------- 
#  weather

ws <- "D:/xHM/example_data/Weather/"
fname = "rr_daily_18sites_2001-2016.csv"

df <- read.table(
  paste0(ws, fname),
  header = F, sep = ','
)

df <- df / 10

write.table(
  df,
  paste0(ws, "data_PRE.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

fname = "weather_daily_2001-2016.csv"
df <- read.table(
  paste0(ws, fname),
  header = F, sep = ','
)

write.table(
  as.integer(df[,1]),
  paste0(ws, "data_PRS.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,2]),
  paste0(ws, "data_RHU.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,3]),
  paste0(ws, "data_SSD.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,4] * 10),
  paste0(ws, "data_WIN.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,5] * 10),
  paste0(ws, "data_TEM_AVG.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,6] * 10),
  paste0(ws, "data_TEM_MAX.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

write.table(
  as.integer(df[,7] * 10),
  paste0(ws, "data_TEM_MIN.csv"),
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

# --- 
var = "TEM_MAX"
fname = paste0(var, ".nc")
nc <- nc_open(paste0(ws, fname))
nc
df_time <- ncvar_get(nc, "time")
df_data <- ncvar_get(nc, var)
View(df_data[,,3])
as.POSIXct(df_time[1], origin = "1970-01-01", tz = "UTC")


nc_close(nc)

# --------- output ---------
ws <- "D:/xHM/output/"
var = "Ep"
fname = paste0(var, ".nc")
nc <- nc_open(paste0(ws, fname))
nc
df_time <- ncvar_get(nc, "time")
as.POSIXct(df_time[30], origin = "1970-01-01", tz = "UTC")

df_data <- ncvar_get(nc, var)
View(df_data[,,40])
nc_close(nc)
