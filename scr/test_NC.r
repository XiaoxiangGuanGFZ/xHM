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

## ------- test the output -------------

library(ncdf4)
library(tidyverse)
library(data.table)
ws_radia <- "D:/Chitan/data/radiation/"
ws_out <- "D:/xHM/output/"

df_rad = fread(
  paste0(ws_radia, "Fuzhou.txt"),
  header = T
) %>% filter(
  y <= 2005
)

var = "Rnu"
fname = paste0(var, ".nc")
nc <- nc_open(paste0(ws_out, fname))
nc
df_nc <- ncvar_get(nc, var)
df_sim = df_nc[25,20,]
df_sim

plot(
  df_rad$Rn, df_sim, type = 'p'
)

# Rs or Rn: more or less match; reasonable

# ------------ Ep --------------
# aerodynamic resistance 
# canopy resistance

var = "Ep"
fname = paste0(var, ".nc")
nc <- nc_open(paste0(ws_out, fname))
nc
df_nc <- ncvar_get(nc, var)
df_sim = df_nc[25,20,]
df_sim

plot(df_sim)

df_pre <- read.table(
  "D:/xHM/example_data/Weather/data_PRE.csv",
  header = F, sep = ','
)
df_pre * 10
write.table(
  df_pre * 10,
  "D:/xHM/example_data/Weather/data_PRE.csv",
  col.names = F, row.names = F, quote = F, append = F, sep = ','
)

ts_pre = round(rowMeans(df_pre), 2)
plot(ts_pre)

# --------- test infiltration --------

SM = 0.2
porosity = 0.482
SCS = 0.00078
air_Pres = 0.405
b = 11.4
step_time = 24
effective = (2 * b + 3) / (2 * b + 6) * air_Pres

w = 0.03 / step_time
w = seq(0.0188, 0.06, 0.0001) / step_time

Tp = SCS * effective * (porosity - SM) / (w * (w - SCS))
Tp


Tc = w * Tp / SCS - effective * (porosity - SM) / SCS * log(
  1 + w * Tp / (effective * (porosity - SM))
)
Tc

te = 24 - Tp + Tc
plot(te)
te

w[196] - SCS

plot(
  w - SCS, te
)
t
te

# ---------- percolation -----------


Soil_Hydro_con <- function(SM,
                           Soil_hydro_sat,
                           b,
                           Porosity,
                           Residual) {
  if (SM <= Residual)
  {
    out = 0
  } else if (SM <= Porosity)
  {
    out = Soil_hydro_sat * ((SM - Residual) / (Porosity - Residual))^(2 * b + 3)
  } 
  else
  {
    out = Soil_hydro_sat
  }
  return(out)
}


x = seq(0.001, 0.55, 0.001)
x^(1/ (2 * 11.2 + 3))

SM = 0.3
input = 0.005
SCS = 0.005
Porosity = 0.5
Residual = 0.05
b = 8.5
d = 0.2

# SM = seq(0.01, 0.6, 0.02)
# soil_hydro_con = 0
# for (i in 1:length(SM)) {
#   soil_hydro_con = Soil_Hydro_con(
#     SM[i],
#     SCS,
#     b,
#     Porosity,
#     Residual
#   )
#   print(paste0(SM[i], " ", soil_hydro_con))
# }

input = seq(0.001, 0.090, 0.001)
for (i in 1:length(input)) {
  soil_hydro_con = Soil_Hydro_con(
    SM,
    SCS,
    b,
    Porosity,
    Residual
  )
  
  soil_hydro_con_end = Soil_Hydro_con(
    (SM + input[i] / d),
    SCS,
    b,
    Porosity,
    Residual
  )
  
  soil_hydro_con_avg = 0.5 * (soil_hydro_con_end + soil_hydro_con)
  SM_avg = (soil_hydro_con_avg / SCS) ^( 1/ (2 * b + 3)) * (Porosity - Residual) + Residual
  SM_avail = input[i] + (SM - SM_avg) * d
  print(paste0(input[i], " ", SM_avail))
}




soil_hydro_con = Soil_Hydro_con(
  SM,
  SCS,
  b,
  Porosity,
  Residual
)

soil_hydro_con_end = Soil_Hydro_con(
  (SM + input / d),
  SCS,
  b,
  Porosity,
  Residual
)

soil_hydro_con_avg = 0.5 * (soil_hydro_con_end + soil_hydro_con)
SM_avg = (soil_hydro_con_avg / SCS) ^( 1/ (2 * b + 3)) * (Porosity - Residual) + Residual
SM_avail = input + (SM - SM_avg) * d

#/ -------- soil id -------

fp = "D:/xHM/example_data/SOIL_HWSD_ID.txt"
df <- read.table(
  fp, header = T
)

for (i in 1:(dim(df)[1])) {
  if (df$T_SAND[i] == -99)
  {
    print(df$ID[i])
    df$T_SAND[i] = df$T_SAND[i-1]
    df$T_SILT[i] = df$T_SILT[i-1]
    df$T_CLAY[i] = df$T_CLAY[i-1]
    df$T_USDA_TEX_CLASS[i] = df$T_USDA_TEX_CLASS[i-1]
    df$T_OC[i] = df$T_OC[i-1]
  }
}

for (i in 1:(dim(df)[1])) {
  if (df$S_SAND[i] == -99)
  {
    
    df$S_SAND[i] = df$T_SAND[i]
    df$S_SILT[i] = df$T_SILT[i]
    df$S_CLAY[i] = df$T_CLAY[i]
    df$S_USDA_TEX_CLASS[i] = df$T_USDA_TEX_CLASS[i]
    df$S_OC[i] = df$T_OC[i]
  }
}

df
write.table(
  df,
  file = fp,
  col.names = T, row.names = F, quote = F, append = F, sep = '\t'
)
sum(df == -99)

# ------- subsurface water move ---------
step_time = 24
D = 1.5
z = 0.5
z = seq(0, D, 0.01)
n = 3
h = (1 - z/D) ^n
h
plot(z, h)


Soil_Hydro_Sat = 2.1 # m/h

step_space = 1000
slope = 90 / 1000
Qout = step_space * Soil_Hydro_Sat * D / n * slope * h * 8  # m3/h
Qout / 1000 / 1000 * 24 * 1000

Qout = seq(0, 1000, 1)  # m3/h
Qout_mm = Qout / 1000 / 1000 * 24 * 1000

plot(Qout_mm / 1000 / D)

# ------
1.5 / 1000
50 / 1000
0.05 / 0.0015

# --- channel routing ----
k = 0.1
timestep = 24
Q = 0
V = 0
n = 100
Qc = runif(n, 0, 20)
Qc = c(100, rep(0, 100))
Qout = NULL
for (i in 1:n) {
  Qin = Q + Qc[i]
  V_post = Qin / k + (V - Qin / k) * exp(- k * timestep)
  Q = Qin - (V_post - V) / timestep
  Qout[i] = Q
  print(Q)
}

plot(Qout)
# ------------ test xHM ----------

library(tidyverse)
ws <- "D:/xHM/"
dt_ts <- seq(
  as.Date("2003-10-01"),
  as.Date("2013-12-31"),
  1
)
df_date = data.frame(
  date = dt_ts
)
Qobs = read.table(
  paste0(ws, "example_data/Qobs/Qobs_daily_Jianning.txt"),
  header = T
) %>% mutate(
  date = as.Date(time)
) %>% filter(
  date >= as.Date("2004-01-01"),
  date <= as.Date("2008-12-31")
)

dim(Qobs)

#### Qsim

Qsim = read.table(
  paste0(ws, "output/Qout_outlet0.txt"),
  header = F, skip = 10
) %>% mutate(
  date = dt_ts
) %>% filter(
  date >= as.Date("2004-01-01"),
  date <= as.Date("2008-12-31")
)
dim(Qsim)

plot(Qobs$date, Qobs$Qobs, type = "p", col = "black")
points(Qsim$date, Qsim[,1] + Qsim[,3], type = "l", col = "red")

Qre = Qobs$Qobs
Qsi = Qsim[,1] + Qsim[,3]
R2 = 1-sum((Qre-Qsi)^2,na.rm = T)/sum((Qre-mean(Qre))^2,na.rm = T) #NSE

1 - R2

plot(
  Qobs$Qobs, Qsim[,4]
)
