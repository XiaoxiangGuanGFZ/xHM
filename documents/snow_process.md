# Snow accumulation and melt
## Energy balance of snowpack
The model represents the snowpack as a two-layer medium (a thin surface, and a thick deeper layer), and solves an energy and mass balance for the ground surface snowpack similarly to other cold land processes models [Anderson, 1976; Wigmosta et al., 1994; Tarboton et al., 1995]. Energy exchange between the atmosphere, forest canopy and snowpack occurs only with the surface layer. The energy balance of the surface layer is:

$$
  	{\rho_w}{c_s} \frac{d(WT_s)}{dt}=Q_r+Q_s+Q_e+Q_p+Q_m
$$

where:
- $c_s$ is the specific heat of ice
- $\rho_w$ is the density of water, 1000 $kg/m^3$
- $W$ is the water equivalent, If the flux terms are expressed in watts per square meter then $W$ is given in meters.
- $T_s$ is the temperature of the surface layer
- $Q_r$ is the net radiation flux
- $Q_s$ is the sensible heat flux
- $Q_e$ is the laten heat flux
- $Q_p$ is the energy flux adverted to the snowpack by rain or snow
- $Q_m$ is the energy flux given to the pack because of liquid water refreezing or removed from the pack during melt

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

The flux of sensible heat to the snowpack is given by:

$$
    Q_s = \frac{\rho c_p (T_a - T_s)}{r_{a,s}}
$$

where:
- $\rho$ is the air density
- $c_p$ is the specific heat of air
- $T_a$ is the air temperature
- $r_{a,s}$ is the aerodynamic resistance between the snow surface and the near-surface reference height, given by:

$$
r_{a,s}=\frac{[\ln{(\frac{z-d_s}{z_0})}]^2}{k^2 U_z}
$$

where:
- $k$ is von Karman's constant
- $z_0$ is the snow surface roughness
- $d_s$ is the snow depth
- $U_z$ is the wind speed at the near-surface reference height $z$

the flux of latent heat to the snow surface is given by:

$$
Q_e = \lambda_i \rho \frac{0.622}{P_a} \frac{e(T_a) - e_s(T_s)}{r_{a,s}}
$$

where:
- ${\lambda}_i$ is the latent heat of vaporization when liquid water
is present in the surface layer and the latent heat of sublimation in the absence of it
- $P_a$ is the atmospheric pressure
- $e$ and $e_s$ are the vapor and saturation vapor pressure, respectively

Advected energy to the snowpack via precipitation (rain or snow) is given by:

$$
Q_P = \frac{\rho_w c_w T_a P_L + \rho_w c_s T_a P_I}{\Delta t}
$$

where:
- $c_w$ is the specific heat of water
- $P_L$ is depth of water in the liquid phrase
- $P_I$ is the (liquid) water equivalent of the solid phase

## Rainfall-snowfall partition

Precipitation $P$ is partitioned into snowfall and rainfall on the basis of a temperature threshold:

$$
P_s = P, T_a <= T_{min};
$$

$$
P_s = \frac{T_{max} - T_a}{T_{max} - T_{min}} P, T_{min} <= T_a <= T_{max};
$$

$$
P_s = 0, T_a >= T_{max};
$$ 

$$
P_r = P- P_s
$$

The snowpack receives water in both liquid ($P_L$) and solid ($P_I$) phases. In cells without an overstory canopy, this is simply the depths of rainfall ($P_r$) and snowfall ($P_s$), respectively; i.e., $P_L = P_r$, and $P_I = P_s$. Tipically, the air temperature thresholds are taken as -1.1 and 3.3 $\degree C$ .

## Mass balance for snowpack
The total energy available for refreezing liquid water or melting the snowpack over a given time step depends on the net energy exchange at the snow surface:

$$
Q_{net} = (Q_r + Q_s + Q_e + Q_p) \Delta t
$$

If $Q_{net}$ is negative, then energy is being lost by the pack, and liquid water (if present) is refrozen. If $Q_{net}$ is sufficiently negative to refreeze all liquid water, then the pack may cool. If $Q_{net}$ is positive, then the excess energy available after the cold content has been satisfied, produces snowmelt

$$
Q_m \Delta t = min(-Q_{net}, \rho_w \lambda_f W_{liq}), Q_{net} < 0
$$ 

$$
Q_m \Delta t = - (Q_{net} + c_s W_{ice} T_s^t), Q_{net} >= 0
$$

where:
- $W_{liq}$ is the volume per unit area (depth) of liquid water
- $W_{ice}$ is the water equivalent of ice
- and $W = W_{liq} + W_{ice}$

The mass balance of the surface layer is given by:

$$
\Delta W_{liq} = P_r + (\frac{Q_e}{\rho_w \lambda_v} - \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

$$
\Delta W_{ice} = P_s + (\frac{Q_e}{\rho_w \lambda_s} + \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

where:
- $\lambda_v$ is the latent heat of vaporization
- $\lambda_s$ is the latent heat of sublimation
- $\lambda_f$ is the latent heat of fusion [334 kJ/kg]

where $Q_e$ exchanges water with the liquid phase if liquid
water is present and $Q_e$ exchanges water with the ice phase
in the absence of liquid water. If $W_{ice}$ exceeds the maximum
thickness of the surface layer (typically taken as 0.10 m of
SWE), then the excess, along with its cold content, is
distributed to the deeper (pack) layer. Similarly, if Wliq
exceeds the liquid water holding capacity of the surface
layer, the excess is drained to the pack layer. If the temperature of the pack layer is below freezing then liquid
water transferred from the surface layer can refreeze. Liquid
water remaining in the pack above its holding capacity is
immediately routed to the soil as snowpack outflow. The
dynamics of liquid water routing through the snowpack are
not considered in this model because of the relatively coarse
temporal and spatial resolutions of the model (typically 1 to
3 h and 50–100 km2, respectively) [Lundquist and
Dettinger, 2005].
## Snowpack densification
Following a similar approach to Anderson [1976],
compaction (due to snow densification) is calculated as the
sum of two fractional compaction rates representing compaction
due to metamorphism and overburden, respectively:

$$
\frac{\Delta \rho_s}{\Delta t} = (CR_m + CR_o) \rho_s
$$

where:
- $\rho_s$ is the snow density, in unit $kg/m^3$
- $CR_m$ and $CR_o$ are the compaction rates due to metamorphism and overburden, respectively
- $\Delta t$ is time interval in hours

Snowpack depth $d_s$ (unit: m) can be updated by:
$$
d_s \rho_s = W \rho_w
$$


Destructive metamorphism is important for newer snow, and the following empirical function is used:

$$
CR_m = 2.788 \times 10^{-6} c_3 c_4 e^{-0.04(273.15-T_s)}
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
Ri_b = \frac{2gz(T_a-T_s)}{(T_a+ T_s) U_z^2}
$$

with the correction for stable conditions given as:

$$
r_{a,s} = \frac{r_{a,s}}{(1 - Ri_b / Ri_{cr})^2},0<=Ri_b<=Ri_{cr}
$$

and in unstable conditions as:

$$
r_{a,s} = \frac{r_{a,s}}{(1 - 16Ri_b )^{0.5}},Ri_b<0
$$

where:
- $g$ is the the acceleration of gravity ($9.8m/s^2$)
- $U_z$ is the wind speed ($cm/s$) at the height of $z$
- $Ri_{cr}$ is the critical value of the Richardson’s number
(commonly taken as 0.2). 

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
I=fP_s
$$

where:
- $I$ is the water equivalent of snow intercepted during a time step
- $P_s$ is the snowfall over the time step
- $f$ is the efficiency of snow interception (taken as 0.6)[Storck et al., 2002].

The maximum interception capacity is given by:

$$
B=L_R \times m \times LAI
$$

where:
- $LAI$ is the single-sided leaf area index of the canopy
- $m$ (same units as $B$, in meters) is determined on the basis of observations of maximum snow interception capacity (40mm in setup experiment region). 

The leaf area ratio $L_r$ is a step function of temperature:

$$
L_R=4.0, T_a > -1
$$

$$
L_R=1.5T_a + 5.5, -3 < T_a <= -1
$$

$$
L_R=1.0, T_a <= -3
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
index of the canopy. Excess rain becomes throughfall.
- $T_{co}$ is the throughfall excess rain.

The intercepted snowpack can contain both ice and
liquid water. The mass balance for each phase is:

$$
\Delta W_{ice} = I-M + (\frac{Q_e}{\rho_w \lambda_s} + \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

$$
\Delta W_{liq} = P_r - P_{tf} + (\frac{Q_e}{\rho_w \lambda_v} - \frac{Q_m}{\rho_w \lambda_f}) \Delta t
$$

where:
- $M$ is the snow mass release from the canopy
- $P_{tf}$ is canopy throughfall
- $\lambda_s$, $\lambda_v$ and $\lambda_f$ are the latent heat of sublimation, vaporization, and fusion, respectively

Snowmelt is calculated directly from a modified energy balance,
similar to that applied for the ground snowpack, with
canopy temperature being computed by iteratively solving
the intercepted snow energy balance (equation (1)). However,  testing of a fully iterative (on $T_s$) intercepted snow energy balance revealed that the intercepted snow temperature closely followed that predicted by $T_s = min(T_a, 0)$

Given the intercepted snow temperature and air temperature,
snowmelt is calculated directly from equations (8) and (9).
The individual terms of the energy balance are as described
for the ground snowpack model. 

However, the aerodynamic resistance is calculated with respect to the sum of the displacement and roughness lengths of the canopy. Incoming shortwave and longwave radiation are taken as the
values at the canopy reference height. The same formulation
is used for the albedo of the ground snowpack and the snow
on the canopy (equations (15)–(16)), the difference being in
the transmitted radiative fluxes.

Snowmelt in excess of the liquid water holding
capacity of the snow results in meltwater drip ($D$). Mass
release of snow from the canopy occurs if sufficient snow is
available and is related linearly to the production of meltwater
drip:

$$
M=0,W_{ice}<=n
$$

$$
M=0.4D, W_{ice}>n
$$

where $n$ is the residual intercepted snow that can only be
melted (or sublimated) off the canopy (taken as 5 mm on the
basis of observations of residual intercepted load). The ratio
of 0.4 in equation (33) is derived from observations of the
ratio of mass release to meltwater drip [Storck et al., 2002].

Rainfall and snowfall not intercepted by the canopy combine with
mass release and drip to contribute energy and mass to the ground
snowpack. The depth of liquid delivered to the ground snowpack ($P_L$) is equal to: 

$$
P_L = P_r(1-F) + T_{co} + D_r
$$

The (liquid) water equivalent depth of solid phase ($P_I$) is given by:

$$
P_I = P_s(1-f)F + P_s(1-F) + M
$$

where:
- $F$ is the fractional ground cover of the overstory
- $f = [0.08T_a - 0.0016T_a^2]^{-1}$, $f$ is the environmental dependency factor (air tempeature) which influences canopy resistance

## Data required and parameters




## Reference

Anderson, E. A. (1976), A point energy and mass balance model of a snow cover, Tech. Rep. 19, NOAA, Silver Spring, Md.

Andreadis, K. M., D. Liang, L. Tsang, D. P. Lettenmaier, and E. G. Josberger (2008), Characterization of errors in a coupled snow hydrology microwave emission model, J. Hydrometeorol., 9, 149-164.

Jordan, R. (1991), A one-dimensional temperature model for a snow cover: Technical documentation for SNTHERM89, Tech. Rep. 91-16, Cold Reg. Res. and Eng. Lab., U.S. Army Corps of Eng., Hanover, N. H.

Kobayashi, D. (1987), Snow accumulation on a narrow board, Cold Reg. Sc. Technol., 13, 239- 245.

Pfister, R., and M. Schneebeli (1999), Snow accumulation on boards of different sizes and shapes, Hydrol. Processes, 13, 2345- 2355.

Storck, P., D. P. Lettenmaier, and S. M. Bolton (2002), Measurement of snow interception and canopy effects on snow accumulation and melt in a mountainous maritime climate, Oregon, United States, Water Resour. Res., 38(11), 1223, doi:10.1029/2002WR001281.

Tarboton, D. G., T. G. Chowdhury, and T. H. Jackson (1995), A spatially distributed energy balance snowmelt model, in Biogeochemistry of Seasonally Snow Covered Catchments, vol. 228, edited by K. A. Tonneson, W. Williams, and M. Tranter, pp. 141-155, Int. Assoc. of Hydrol. Sci., Wallingford, U. K.

Wigmosta, M. S., L. W. Vail, and D. P. Lettenmaier (1994), A distributed hydrology-vegetation model for complex terrain, Water Resour. Res., 30, 1665- 1679.



