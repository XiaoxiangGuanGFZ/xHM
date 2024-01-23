
#---------------SCEUA-WBM水文模型--说明---------------------------
# 输入资料：流域面积，流域面均降水量（mm）；
# 流域潜在蒸散发量（mm）；出口断面实测流量（m3/s）；
# 流域面均气温（℃）
# 所有资料都为月尺度
# Author: Xiaoxiang Guan
# e-mail: guan@gfz-potsdam.de


library(rtop)
library(tidyverse)

WBM = function(A,P,Er,Discharge,Tem,Ks,Kg,Ksn,Smax){
  # A:流域面积（km2）
  # P：流域面均月降水量（mm）
  # Er：流域面均月潜在蒸散发量(mm)
  # Discharge：出口断面月均流量（m3/s）
  # Tem：流域面均月气温（℃）
  # Ks：地表径流出流系数，
  # Kg：地下径流出流系数，
  # Ksn：融雪径流系数，
  # Smax：土壤蓄水容量（mm）
  S0 = 1
  SN0 = 0.2*Smax
  n = length(P)   #length of data sequence
  Pr = NULL   #rainfall /mm
  Psn = NULL  #snow-melt /mm
  Qs = NULL   #surface flow
  Qg = NULL   #groundwater discharge
  Qsn = NULL  #snow-melt driven runoff
  Q = NULL    #total runoff
  S = NULL    #soil moisture
  SN = NULL   #snow accumulation
  E = NULL    #actual evaporation
  
  for (i in 1:n) {
    if (i == 1) {
      S_p = S0   #S_p antecedent soil moisture
      SN_p = SN0  #SN_p antecedent snow accumulation
      #Qsn_p = 0.5
    }
    else {
      S_p = S[i-1]
      SN_p = SN[i-1]
      #Qsn_p = Qsn[i-1]
    }
    
    if (Tem[i] <= -3) {
      Pr[i] = 0
      Psn[i] = P[i]
      Qs[i] = 0
      Qsn[i] = 0
      Qg[i] = Kg*S_p
      
      E[i] = 0.2*Psn[i] #20% precipitated snow for Snow evaporation
      SN[i] = SN_p + 0.8*Psn[i] #80% snow accumulated
      S[i] = S_p - Qg[i]
      #*****soil moisture correction*****
      if (S[i] <= 0.08*Smax) {
        Qg[i] = Qg[i] + (S_p - 0.08*Smax)*Qg[i]/(Qg[i]+Er[i]+0.001)
        E[i] = E[i] + (S_p - 0.08*Smax)*(Er[i]+0.001)/(Qg[i]+Er[i]+0.001)
        S[i] = 0.08*Smax
      }
      if (S[i] >= Smax) {
        Qg[i] = Qg[i] + S_p - Smax
        S[i] = Smax
      }
      
    } else if (Tem[i] > -3 & Tem[i] <= 3) {
      Pr[i] = P[i]*(Tem[i]+3)/6
      Psn[i] = P[i]*(3-Tem[i])/6
      SN[i] = SN_p+Psn[i]
      Qsn[i] = Ksn*SN[i]*exp((Tem[i]-3)/6)
      SN[i] = SN[i] - Qsn[i]
      
      #*******snow storage correction****
      if (SN[i] < 0 ) {
        Qsn[i] = SN_p+Psn[i]
        SN[i] = 0
      }
      Qg[i] = Kg*S_p
      Qs[i] = Ks*Pr[i]*S_p/Smax
      
      S[i] = S_p-Qg[i]+Pr[i]-Qs[i]
      E[i] = Er[i]*S[i]/Smax
      S[i] = S[i]-E[i]
      total = Qs[i] + Qg[i] + Er[i]+0.01
      #*****soil moisture correction*****
      if (S[i] > Smax) {
        Qs[i] = Qs[i] + (S[i] - Smax)*Qs[i]/total
        Qg[i] = Qg[i] + (S[i] - Smax)*Qg[i]/total
        E[i] = E[i] + (S[i] - Smax)*(E[i]+0.01)/total
        S[i] = Smax
      }
      if (S[i] <= 0.08*Smax) {
        Qs[i] = Qs[i] + (S[i] - 0.08*Smax)*Qs[i]/total
        Qg[i] = Qg[i] + (S[i] - 0.08*Smax)*Qg[i]/total
        E[i] = E[i] + (S[i] - 0.08*Smax)*(E[i]+0.01)/total
        S[i] = 0.08*Smax
      }
      
    } else { #Tem[i] > 3
      Pr[i] = P[i]
      Psn[i] = 0
      if (SN_p > 0) {
        Qsn[i] = 0.5*Ksn*SN_p
        Pr[i] = Pr[i] + SN_p - Qsn[i]
        SN[i] = 0
        
      } else {
        Qsn[i] = 0
        SN[i] = 0
        Pr[i] = Pr[i] + SN_p
      }
      Qs[i] = Ks*Pr[i]*S_p/Smax
      Qg[i] = Kg*S_p
      E[i] = Er[i]*S_p/Smax
      S[i] = S_p+Pr[i]-Qs[i]-Qg[i]-E[i]
      total = Qs[i] + Qg[i] +Er[i]+0.01
      #*****soil moisture correction*****
      if (S[i] > Smax) {
        Qs[i] = Qs[i] + (S[i] - Smax)*Qs[i]/total
        Qg[i] = Qg[i] + (S[i] - Smax)*Qg[i]/total
        E[i] = E[i] + (S[i] - Smax)*(E[i]+0.01)/total
        S[i] = Smax
      }
      if (S[i] <= 0.08*Smax) {
        Qs[i] = Qs[i] + (S[i] - 0.08*Smax)*Qs[i]/total
        Qg[i] = Qg[i] + (S[i] - 0.08*Smax)*Qg[i]/total
        E[i] = E[i] + (S[i] - 0.08*Smax)*(E[i]+0.01)/total
        S[i] = 0.08*Smax
      }
    }
    
    Q[i] = Qs[i] + Qg[i] + Qsn[i]  #adjusted total runoff
  }
  R_si = Q   #unit:mm
  R_re = Discharge*30*24*3.6/A #unit:mm
  out = data.frame(R_si,R_re)
  colnames(out) = c('R_si','R_re')
  return(out)
}
#-------------拟合度函数----------
fit_WBM = function(x) {
  Ks = x[1];Kg = x[2];Ksn = x[3];Smax = x[4]
  out = WBM(Validinput_DAREA,Validinput_P,Validinput_E,Validinput_Q,Validinput_T,
            Ks,Kg,Ksn,Smax)
  Qsi = out$R_si
  Qre = out$R_re
  R2 = 1-sum((Qre-Qsi)^2,na.rm = T)/sum((Qre-mean(Qre))^2,na.rm = T) #NSE
  return(1-R2)
}
#----------------SCEUA-WBM-Main----------------


dat = read.table("J:/王国庆老师/小论文3-SWBM模型区域适用性研究/6个典型站点/石角－38363/data.csv",header = F,sep = ",")
dat = dat[dat[,1]<=1970,]
Validinput_P <<- dat[,3]
Validinput_E <<- dat[,6]
Validinput_Q <<- dat[,5]
Validinput_T <<- dat[,4]
Validinput_DAREA <<- 38363

paralower = c(0  ,0 ,  50, 0)
paraupper = c(1  ,1 , 450, 0.1)
parainitial=c(0.2,0.2 ,200,0.002)

sceua_WBM = sceua(fit_WBM, pars = parainitial, lower = paralower, 
                  upper = paraupper, maxn = 10000, pcento = 0.00001,iprint = 0)
fit(sceua_WBM$par)
para = sceua_WBM$par
Ks = para[1];Kg = para[2];Ksn = para[3];Smax = para[4]
out = WBM(Validinput_DAREA,Validinput_P,Validinput_E,Validinput_Q,Validinput_T,
          Ks,Kg,Ksn,Smax)
Qsi = out$R_si
Qre = out$R_re
ma = max(c(Qsi,Qre))
plot(Qre,Qsi,type = "p",xlim=c(0,ma),ylim = c(0,ma))



