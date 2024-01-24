
#---------------SCEUA-WBM水文模型--说明---------------------------
# 输入资料：流域面积，流域面均降水量（mm）；
# 流域潜在蒸散发量（mm）；出口断面实测流量（m3/s）；
# 流域面均气温（℃）
# 所有资料都为月尺度
# Author: Xiaoxiang Guan
# e-mail: guan@gfz-potsdam.de


library(rtop)
library(tidyverse)

fp_xHM <<- "D:/xHM/scr/xHM.exe "
fp_gp_tem <<- "D:/xHM/example_data/Global_Para_tem.txt"
fp_gp <<- "D:/xHM/example_data/Global_Para_SCEUA.txt"
df_gp <<- data.frame(
  field = c("SOIL_D", "SOIL_d1", "SOIL_d2", "ROUTE_CHANNEL_k"),
  value = c(1.5, 0.2, 0.2, 2)
)

Qobs <<- read.table(
  "D:/xHM/example_data/Qobs/Qobs_daily_Jianning.txt",
  header = T
) %>% mutate(
  date = as.Date(time)
) %>% filter(
  date >= as.Date("2004-01-01"),
  date <= as.Date("2008-12-31")
)


data_start <<- as.Date("2003-10-01")
data_end <<- as.Date("2013-12-31")
dt_ts <<- seq(data_start,
              data_end,
              1)

#-------------拟合度函数----------
fit_xHM = function(x) {
  file.copy(fp_gp_tem, fp_gp, overwrite = TRUE)
  df_gp$value = x
  write.table(
    df_gp,
    file = fp_gp,
    col.names = F, row.names = F, quote = F, append = T, sep = ','
  )
  system(paste0(fp_xHM, fp_gp))
  
  Qsim = read.table(
    "D:/xHM/output/Qout_outlet0.txt",
    header = F, skip = 10
  ) %>% mutate(
    date = dt_ts
  ) %>% filter(
    date >= as.Date("2004-01-01"),
    date <= as.Date("2008-12-31")
  )
  Qre = Qobs$Qobs
  Qsi = Qsim[,4]
  R2 = 1-sum((Qre-Qsi)^2,na.rm = T)/sum((Qre-mean(Qre))^2,na.rm = T) #NSE
  return(1-R2)
}
#----------------SCEUA-xHM-Main----------------

paralower = c(1  , 0.05,  0.05, 0.01)
paraupper = c(3  , 0.5, 0.5, 5)
parainitial = c(1.5, 0.2, 0.2, 0.4)

sceua_xHM = sceua(fit_xHM, pars = parainitial, lower = paralower, 
                  upper = paraupper, maxn = 2000, pcento = 0.0001,iprint = 0)
fit(sceua_xHM$par)
para = sceua_xHM$par

Qsim = read.table(
  "D:/xHM/output/Qout_outlet0.txt",
  header = F, skip = 10
) %>% mutate(
  date = dt_ts
) %>% filter(
  date >= as.Date("2004-01-01"),
  date <= as.Date("2008-12-31")
)
Qre = Qobs$Qobs
Qsi = Qsim[,4]
R2 = 1-sum((Qre-Qsi)^2,na.rm = T)/sum((Qre-mean(Qre))^2,na.rm = T) #NSE
plot(Qobs$date, Qobs$Qobs, type = "p", col = "black")
points(Qsim$date, Qsim[,1] + Qsim[,3], type = "l", col = "red")

