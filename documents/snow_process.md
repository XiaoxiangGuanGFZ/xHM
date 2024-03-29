# Snow accumulation and melt
## Energy balance of snowpack
The model represents the snowpack as a two-layer medium (a thin surface, and a thick deeper layer), and solves an energy and mass balance for the ground surface snowpack similarly to other cold land processes models [Anderson, 1976; Wigmosta et al., 1994; Tarboton et al., 1995]. Energy exchange between the atmosphere, forest canopy and snowpack occurs only with the surface layer. The energy balance of the surface layer is:

$$
  	{\rho_w}{c_s} \frac{d(WT_s)}{dt}=Q_r+Q_s+Q_e+Q_p+Q_m
$$

where:
- $c_s$ is the specific heat of ice, 2.1 kJ/(kg°C).
- $\rho_w$ is the density of water, 1000 $kg/m^3$
- $W$ is the water equivalent, If the flux terms are expressed in watts per square meter then $W$ is given in meters.
- $T_s$ is the temperature of the surface layer
- $Q_r$ is the net radiation flux 
- $Q_s$ is the sensible heat flux
- $Q_e$ is the latent heat flux
- $Q_p$ is the energy flux adverted to the snowpack by rain or snow
- $Q_m$ is the energy flux given to the pack because of liquid water refreezing or removed from the pack during melt.
- the unit for the energy flux is $kJ/(m^2h)$.

This equation can be solved via a forward finite difference scheme over the model time step (${\Delta}t$):

$$
W^{t+\Delta t} {T_s}^{t+\Delta t} - W^{t} {T_s}^{t} = \frac{\Delta t}{\rho_w c_s}(Q_r+Q_s+Q_e+Q_p+Q_m)
$$

## Energy compoments
Net radiation $Q_r$ at the snow surface is either measured or calculated given incoming shortwave and longwave radiation as:

$$
    Q_r=L_i + S_i (1-\alpha) - \sigma {T_s}^4
$$

where:
- $L_i$ and $S_i$ are incoming long and shortwave radiation
- $\alpha$ is the snow surface albedo
- $\sigma$ is the Stefan-Boltzmann constant $4.903 \times 10^{-9}MJ/(K^4m^2d)$

The flux of sensible heat to the snowpack is given by:

$$
    Q_s = \frac{\rho c_p (T_a - T_s)}{r_{a,s}}
$$

where:
- $\rho$ is the air density, assumed as 1.2922 $kg/m^3$
- $c_p$ is the specific heat of air, 1.005 kJ/(kg°C)
- $T_a$ is the air temperature [°C]
- $r_{a,s}$ is the aerodynamic resistance (unit: h/m) between the snow surface and the near-surface reference height, given by:

$$
r_{a,s}=\frac{[\ln{(\frac{z-d_s}{z_0})}]^2}{k^2 U_z}
$$

where:
- $k$ is von Karman's constant, which is dimensionless and assumed as 0.4
- $z_0$ is the snow surface roughness, assumed as 0.0003 m
- $d_s$ is the snow depth [m]
- $U_z$ is the wind speed (unit: m/h) at the near-surface reference height $z$. 
- $z=2+z_d+z_0$, $z_d$ and $z_0$ are zero-plane displacement (here, snow depth $d_s$) and roughness height respectively (Stock, 2000).

$U_z$ can be derived by assuming logarithmic wind speed profile, where $U_{z_1}$ and $U_{z_2}$ are wind speeds at the height of $z_1$ and $z_2$ respectively: 

$$U_{z_2}=U_{z_1} \frac{\ln((z_2-z_d)/z_0)}{\ln((z_1-z_d)/z_0)}$$


the flux of latent heat to the snow surface is given by:

$$
Q_e = \lambda_i \rho \frac{0.622}{P_a} \frac{e(T_a) - e_s(T_s)}{r_{a,s}}
$$

where:
- ${\lambda}_i$ is the latent heat of vaporization when liquid water is present in the surface layer and the latent heat of sublimation in the absence of it. 
- The latent heat of vaporization is set as 2500 kJ/kg (although it changes with water temperature), as we assume that snow melting is isothermal, meaning that the snow temperature is unchanged at 0 °C during melting. 
- The latent heat of sublimation is taken as 2838 kJ/kg for ice at 0°C.
- $P_a$ is the atmospheric pressure, [kPa]
- $e$ and $e_s$ are the vapor and saturation vapor pressure [kPa], respectively

Advected energy to the snowpack via precipitation (rain or snow) is given by:

$$
Q_P = \frac{\rho_w c_w T_a P_L + \rho_w c_s T_a P_I}{\Delta t}
$$

where:
- $c_w$ is the specific heat of water, assumed as 4.22 kJ/(kg°C)
- $P_L$ is depth of water in the liquid phrase, [m]
- $P_I$ is the (liquid) water equivalent of the solid phase, [m]

## Rainfall-snowfall partition

Precipitation $P$ is partitioned into snowfall and rainfall on the basis of a temperature threshold:

$$
P_s = P, T_a \le T_{\min};
$$

$$
P_s = \frac{T_{\max} - T_a}{T_{\max} - T_{\min}} P, T_{\min} \le T_a \le T_{\max};
$$

$$
P_s = 0, T_a \ge T_{\max};
$$ 

$$
P_r = P- P_s
$$

The snowpack receives water in both liquid ($P_L$) and solid ($P_I$) phases. In cells without an overstory canopy, this is simply the depths of rainfall ($P_r$) and snowfall ($P_s$), respectively; i.e., $P_L = P_r$, and $P_I = P_s$. Typically, the air temperature thresholds ( $T_{\min}$ and $T_{\max}$ ) are taken as -1.1 and 3.3 $\degree$ C.

## Mass balance for snowpack
The total energy available for refreezing liquid water or melting the snowpack over a given time step depends on the net energy exchange at the snow surface:

$$
Q_{net} = (Q_r + Q_s + Q_e + Q_p) \Delta t
$$

If $Q_{net}$ is negative, then energy is being lost by the pack, and liquid water (if present) is refrozen. If $Q_{net}$ is sufficiently negative to refreeze all liquid water, then the pack may cool. If $Q_{net}$ is positive, then the excess energy available after the cold content has been satisfied, produces snowmelt

$$
Q_m \Delta t = \min(-Q_{net}, \rho_w \lambda_f W_{liq}), Q_{net} < 0
$$ 

$$
Q_m \Delta t = - (Q_{net} + c_s W_{ice} T_s^t), Q_{net} \ge 0
$$

where:
- $W_{liq}$ is the volume per unit area (depth) of liquid water
- $W_{ice}$ is the water equivalent of ice
- and $W = W_{liq} + W_{ice}$
- unit: m

The mass balance of the surface layer is given by:

$$
\Delta W_{liq} = P_L + (\frac{Q_e}{\rho_w \lambda_v} - \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

$$
\Delta W_{ice} = P_I + (\frac{Q_e}{\rho_w \lambda_s} + \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

where:
- $\lambda_v$ is the latent heat of vaporization [2500 kJ/kg]
- $\lambda_s$ is the latent heat of sublimation [2838 kJ/kg]
- $\lambda_f$ is the latent heat of fusion [334 kJ/kg]

where $Q_e$ exchanges water with the liquid phase if liquid
water is present and $Q_e$ exchanges water with the ice phase
in the absence of liquid water. That means, when liquid water is present in the snowpack, the vaporization or condensation is determined by (vapor-water latent heat) the deficit between wapor pressure and saturated vapor pressure near the snowpack surface ( $e(T_a) - e_s(T_s)$ ). In the absence of liquid water, the snowpack exchanges with atmosphere through sublimation or deposition (vapor to solid).

|scenario ($W$ > 0) | description | snow mass process |
| ------------- | ------------- | -------------- |
| $W_{liq}$ > 0.0, $Q_{net}$ > 0| liquid water exists in the snowpack and receives net energy from environment | water fusion (ice melts from solid to liquid) |
| $W_{liq}$ > 0.0, $Q_{net}$ < 0| liquid water exists in the snowpack and transmits net energy to the environment| liquid water refreezes (from liquid to solid phase)|
| $W_{liq}$ = 0.0, $Q_{net}$ > 0| no liquid water in the snowpack and receives net energy| melting from solid to liquid occurs in the snowpack |
| $W_{liq}$ = 0.0, $Q_{net}$ < 0| no liquid water in the snowpack and transmits net energy to environment | snowpack (solid phase) becomes colder (temperature decreases)|


If $W_{ice}$ exceeds the maximum
thickness of the surface layer (typically taken as 0.10 m of
SWE), then the excess, along with its cold content, is
distributed to the deeper (pack) layer. Similarly, if $W_{liq}$
exceeds the liquid water holding capacity of the surface
layer, the excess is drained to the pack layer. 
If the temperature of the pack layer is below freezing then liquid
water transferred from the surface layer can refreeze. Liquid
water remaining in the pack above its holding capacity is
immediately routed to the soil as snowpack outflow. The
dynamics of liquid water routing through the snowpack are
not considered in this model because of the relatively coarse
temporal and spatial resolutions of the model (typically 1 to
3 h and 50–100 km2, respectively) [Lundquist and
Dettinger, 2005].

During melting, $Q_m$ is negative, removing mass from the ice phase and increasing the liquid phase. Water is removed from snowpack when the liquid phase exceeds the current liquid water storage of the snowpack, taken as $0.06W$ (Wigmosta et al., 1994) 

## Snowpack densification

As fresh snow density is not normally available in the meteorological record, an empirical relationship is used to relate snow densitys to air temperature. The relationship is:

$$
\rho_s = 67.92 + 51.25 e^{T_a/2.59}
$$

where:
- $T_a$ is ambient air temperature [C] (Hedstrom and Pomeroy, 1998)


Following a similar approach to Anderson [1976] and Jordan [1991],
compaction (due to snow densification) is calculated as the
sum of two fractional compaction rates representing compaction
due to metamorphism and overburden, respectively:

$$
\frac{\Delta \rho_s}{\Delta t} = (CR_m + CR_o) \rho_s
$$

where:
- $\rho_s$ is the snow density, in unit $kg/m^3$
- $CR_m$ and $CR_o$ are the compaction rates (with unit of $1/s$ from dimensional analysis) due to metamorphism and overburden, respectively
- $\Delta t$ is time interval, converted to second [s], for instance 86400 s for 24 hours (daily step)

Snowpack depth $d_s$ (unit: m) can be updated by:

$$
d_s \rho_s = W \rho_w
$$

Destructive metamorphism is important for newer snow, and the following empirical function is used:

$$
CR_m = 2.778 \times 10^{-6} c_3 c_4 e^{-0.04(273.15-T_s)}
$$

where, if $\rho_s$ <= 150 $kg/m^3$, then $c_3$ = 1, otherwise $c_3=e^{-0.046(\rho_s-150)}$. $c_4$ = 1 when $\rho_l$ = 0, otherwise ($\rho_l$ > 0) $c_4$ = 2. And:
- $T_s$ is the snowpack temperature
- $\rho_l$ is the bulk density of the liquid water in the snowpack. Therefore, $c_4$ is largely determined by whether there is liquid water in the snowpack.

After the initial settling stage, the densification rate is controlled by the overburden snow, and the corresponding compaction rate can be estimated by:

$$
CR_o=\frac{P_{load}}{\eta_0} e^{-c_5 (273.15-T_s)} e^{-c_6 \rho_s}
$$

where:
- $\eta_0 = 3.6 \times 10^6 N s/m^2$ is the viscosity coefficient at $0 \degree C$
- $c_5=0.08 k^{-1}$, and $c_6=0.021m^3/kg$
- $P_{load}$ is the load pressure
- $T_s$ is the temperature of snowpack surface in $\degree C$

Snowpacks are naturally layered media, therefore
the load pressure is different for each layer of the pack
corresponding to different compaction rates. The model
represents that "internal" compaction as an effective load
pressure, i.e.,

$$
P_{load} = \frac{1}{2} g \rho_w (W_{ns} + f_{com} W_s)
$$

where:
- $g$ is the the acceleration of gravity, 9.8 $m/s^2$
- $W_{ns}$ and $W_s$ are the amounts of newly fallen snow and snow on the ground (in water equivalent units, m), respectively
- $f_{com}$ is the internal compaction rate coefficient taken as 0.6 after calibration to measurements from the Cold Land Processes Experiment in Fraser Park, Colorado [Andreadis et al., 2008]. 

## Snow albedo
Snow albedo is assumed to decay with age on the basis of observations from the DEMO experiment:

$$
\alpha_a = 0.85 {\lambda_a}^{t^{0.58}}
$$

$$
\alpha_m = 0.85 {\lambda_m}^{t^{0.46}}
$$

where:
- $\alpha_a$ and $\alpha_m$ are the albedo during the accumulation and melting seasons. Accumulation and melt seasons are defined on the basis of the absence and presence of liquid water in the snow surface layer, respectively.
- $t$ here is the time since the last snowfall (in days)
- $\lambda_a$=0.92 and $\lambda_m$ = 0.70

## Atmospheric Stability

The calculation of turbulent energy exchange is complicated by the stability of the atmospheric boundary layer. During snowmelt, the atmosphere immediately above the snow surface is typically
warmer. As parcels of cooler air near the snow surface are
transported upward by turbulent eddies they tend to sink back
toward the surface and turbulent exchange is suppressed. In
the presence of a snow cover, aerodynamic resistance is
typically corrected for atmospheric stability according to
the bulk Richardson’s number (Rib). The latter is a dimensionless
ratio relating the buoyant and mechanical forces (i.e.,
turbulent eddies) acting on a parcel of air [Anderson, 1976]:

$$
Ri_b = \frac{2gz_m(T_a(z_m)-T_s)}{(T_a(z_m)+ T_s) U_{z_m}^2}
$$

where: 
- $g$: gravity acceleration, $9.8 m/s^2 $
- $z_m$ is the measurement height [m]
- $T_a(z_m)$ is the air temperature [Kelvin degree, K] at the height of $z_m$
- $T_s$ is snow temperature [K]
- $U_{z_m}$ is wind speed at height of $z_m$, [m/s]


Unstable conditions ($Ri_b<0$):

$$
r_{a,s} = \frac{r_{a,s}}{(1 - 16Ri_b )^{0.5}},
$$

Stable conditions ($Ri_b>0$)

first compute

$$
Ri_u = \frac{1}{\ln(z_m/z_0) + 5}
$$

when $Ri_b > Ri_u$, then $Ri_b = Ri_u$ and:

$$
r_{a,s} = \frac{r_{a,s}}{(1 - Ri_b / Ri_{cr})^2}
$$

where:
- $Ri_{cr}$ is the critical value of the Richardson’s number
(commonly taken as 0.2). 
- $Ri_u$ is the upper limit for Richardson's Number

While the bulk Richardson’s number correction has the advantage of being straightforward to calculate on the basis of observations at only one level above the snow surface, previous investigators have
noted that its use results in no turbulent exchange under common melt conditions and leads to an underestimation of the latent and sensible heat fluxes to the snowpack [e.g., Jordan, 1991; Tarboton et al., 1995].

## Snow Interception
While many models characterize the effect of the
forest canopy on the micrometeorology above the forest
snowpack, almost none attempt to model explicitly the
combined canopy processes that govern snow interception,
sublimation, mass release, and melt from the forest canopy.
A simple snow interception algorithm is presented here that
represents canopy interception, snowmelt, and mass release
at the spatial scales of distributed hydrology models.

During each time step, snowfall is intercepted by the
overstory up to the maximum interception storage capacity
according to:

$$
I=f_e P_sF
$$

where:
- $I$ is the water equivalent of snow intercepted during a time step
- $P_s$ is the snowfall over the time step
- $f_e$ is the efficiency of snow interception (taken as 0.6)[Storck et al., 2002].
- $F$ is the fractional ground cover of the overstory

The maximum interception capacity is given by:

$$
B=L_R \times m \times LAI
$$

where:
- $LAI$ is the single-sided leaf area index of the canopy
- $m$ (same units as $B$, in meters) is determined on the basis of observations of maximum snow interception capacity (40mm or 0.04m in setup experiment region). 

The leaf area ratio $L_R$ is a step function of temperature:

$$
L_R=4.0, T_a > -1
$$

$$
L_R=1.5T_a + 5.5, -3 < T_a \le -1
$$

$$
L_R=1.0, T_a \le -3
$$

which is based on observations from previous studies of
intercepted snow as well as data collected during the field
campaign described [Storck et al., 2002]. Kobayashi [1987]
observed that maximum snow interception loads on narrow
surfaces decreased rapidly as air temperature decreases
below 3 $\degree C$. Results from Kobayashi [1987] and Pfister and Schneebeli [1999] suggest that interception efficiency
decreases abruptly for temperatures less than -1 $\degree C$, and is approximately constant for temperatures less than -3 $\degree C$, leading to a discontinuous relationship. [note: $T_a$ is the air temperature in the unit of $\degree C$]

Newly intercepted rain is calculated with respect to
the water holding capacity of the intercepted snow ($W_c$),
which is given by the sum of capacity of the snow and the
bare branches:

$$
W_c = hW_{ice} + e^{-4}(LAI_2)
$$

where $h$ is the water holding capacity of snow (taken
approximately as 3.5%) and $LAI_2$ is the all sided leaf area
index of the canopy. Excess rain becomes throughfall ($T_{co}$) (Wigmosta et al., 1994 attachment).
- $T_{co}$ is the throughfall excess rain. When $P_rF > W_c$, then $T_{co} = P_rF - W_c$, otherwise $T_{co} = 0$

The intercepted snowpack can contain both ice and
liquid water. The mass balance for each phase is:

$$
\Delta W_{ice} = I-M + (\frac{Q_e}{\rho_w \lambda_s} + \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

$$
\Delta W_{liq} = P_rF - T_{co} + (\frac{Q_e}{\rho_w \lambda_v} - \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

where:
- $M$ is the snow mass release from the canopy, in m.
- $P_RF$ is the rainfall on the overstory canopy.
- $\lambda_s$, $\lambda_v$ and $\lambda_f$ are the latent heat of sublimation, vaporization, and fusion, respectively

Snowmelt is calculated directly from a modified energy balance,
similar to that applied for the ground snowpack, with
canopy temperature being computed by iteratively solving
the intercepted snow energy balance. However,  testing of a fully iterative (on $T_s$) intercepted snow energy balance revealed that the intercepted snow temperature closely followed that predicted by $T_s = \min(T_a, 0)$

Given the intercepted snow temperature and air temperature,
snowmelt is calculated directly from above equations.
The individual terms of the energy balance are as described
for the ground snowpack model. 

However, the aerodynamic resistance ($r_{r,s}$) is calculated with respect to the sum of the displacement ($z_d$) and roughness ($z_0$) lengths of the canopy. Incoming shortwave and longwave radiation are taken as the values at the canopy reference height. 
The same formulation is used for the albedo of the ground snowpack and the snow 
on the canopy, the difference being in
the transmitted radiative fluxes.

Snowmelt in excess of the liquid water holding
capacity of the snow results in meltwater drip ($D_r$). Mass
release of snow ($M$) from the canopy occurs if sufficient snow is
available and is related linearly to the production of meltwater
drip:

$$
M=0,W_{ice} \le n
$$

$$
M=0.4D_r, W_{ice} > n
$$

where $n$ is the residual intercepted snow that can only be
melted (or sublimated) off the canopy (taken as 5mm or 0.005m on the
basis of observations of residual intercepted load). The ratio
of 0.4 in equation (33) is derived from observations of the
ratio of mass release to meltwater drip [Storck et al., 2002].

Rainfall and snowfall not intercepted by the canopy combine with mass release and drip to contribute energy and mass to the ground snowpack. The depth of liquid delivered to the ground snowpack ($P_L$) is equal to: 

$$
P_L = P_r(1-F) + T_{co} + D_r
$$

The (liquid) water equivalent depth of solid phase ($P_I$) is given by:

$$
P_I = P_s(1-f_e)F + P_s(1-F) + M
$$

where, the inputs for ground snow are listed and described as:
| component  | description |
| ---------- | ------------|
| $P_r(1-F)$ | the rainfall from open area|
| $T_{co}$   | the throughfall of rain from overstory (canopy)   |
| $D_r$      | meltwater drip from canopy snow |
| $P_s(1-f_e)F$ | snow throughfall from overstory canopy |
| $P_s(1-F)$    | snowfall from open area  |
| $M$           | mass release from canopy snow |

## Specific consideration

When both interception snow and ground snow exist, the incoming short and longwave radiation should be considered separately, as the canopy plays a role in insolation received by ground snow. 
The effect of forest canopy on insolation is formulated as (see `snow_radiation_calc.md` for detail):  

$$
\tau_F = (1-a_F) \exp(-LAI)
$$

where, $a_F$ is the albedo of the vegetation. Then the incoming shortwave radiation for ground snow is $S_i(1-F) + S_iF\tau_F$, which is definitely less than $S_i$ and for canopy snow is still $S_i$. In terms of longwave radiation, we assume that the temperature of canopy is close to the ambient air temperature, the effect of canopy on longwave radiation could be negligible, namely canopy snow and ground snow receive the same radiation intensity ($L_i$). 


## Data required and parameters
### Required forcing fields

| notation |   meaning     | unit  |
| -------- | ------------- | ----- |
| $L_i$  | incoming longwave radiation | kJ/(m^2h) |
| $S_i$  | incoming shortwave radiation | kJ/(m^2h) |
| $T_a$  | air temperature | degree Celsius °C |
| $U_m$  | wind speed at the measurement height $m$ | m/s |
| $P_a$  | atmospheric pressure | kPa |
| $P$    | precipitation (rainfall + snowfall) | mm |


### Local property (parameters)

| notation |   meaning     | unit  |
| -------- | ------------- | ----- |
| $F$      | the fractional ground cover of the overstory | ~ |
| $LAI$    | the single-sided leaf area index of the canopy | ~ |
| $LAI_2$  | the all-sided leaf area index of the canopy | ~ |
| $a_F$    | canopy albedo  | ~ |


## Reference

Anderson, E. A. (1976), A point energy and mass balance model of a snow cover, Tech. Rep. 19, NOAA, Silver Spring, Md.

Andreadis, K. M., D. Liang, L. Tsang, D. P. Lettenmaier, and E. G. Josberger (2008), Characterization of errors in a coupled snow hydrology microwave emission model, J. Hydrometeorol., 9, 149-164.

Hedstrom, N., and J. W. Pomeroy, 1998: Intercepted snow in the boreal forest: Measurement and modeling. Hydrol. Processes, 12, 1611-1625.

Jordan, R. (1991), A one-dimensional temperature model for a snow cover: Technical documentation for SNTHERM89, Tech. Rep. 91-16, Cold Reg. Res. and Eng. Lab., U.S. Army Corps of Eng., Hanover, N. H.

Kobayashi, D. (1987), Snow accumulation on a narrow board, Cold Reg. Sc. Technol., 13, 239-245.

Pfister, R., and M. Schneebeli (1999), Snow accumulation on boards of different sizes and shapes, Hydrol. Processes, 13, 2345-2355.

Storck, P., D. P. Lettenmaier, and S. M. Bolton (2002), Measurement of snow interception and canopy effects on snow accumulation and melt in a mountainous maritime climate, Oregon, United States, Water Resour. Res., 38(11), 1223, doi:10.1029/2002WR001281.

Tarboton, D. G., T. G. Chowdhury, and T. H. Jackson (1995), A spatially distributed energy balance snowmelt model, in Biogeochemistry of Seasonally Snow Covered Catchments, vol. 228, edited by K. A. Tonneson, W. Williams, and M. Tranter, pp. 141-155, Int. Assoc. of Hydrol. Sci., Wallingford, U. K.

Wigmosta, M. S., L. W. Vail, and D. P. Lettenmaier (1994), A distributed hydrology-vegetation model for complex terrain, Water Resour. Res., 30, 1665-1679.



