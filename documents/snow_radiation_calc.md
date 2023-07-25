# Radiation flux on snowpack

## Incoming shortwave (solar) radiation
the net flux of solar energy entering the snowpack is:

$$
S_{net} = S_{in} - S_{out} = S_{in} (1 - \alpha)
$$

where $\alpha$ is the shortwave reflectance or albedo of snowpack and $S_{in}$ is the flux of solar energy incident on the snowpack surface (insolation). Because of the difficulty in obtaining reliable insolation measurements at appropriate space and time scales, the $S_{in}$ is usually calculated beginning with the values of the flux of solar radiation arriving at the outer edge of the earth's atmosphere, the extraterrestrial solar radiation ( $S_{ET}$ ), which is an astronomically determined function of latitude, declination angle of sun and the zenith angle of the sun. 

$$
S_{ET} = \frac{
    24(60)
}{\pi} G_{sc} d_r [\omega_s \sin(\varphi) \sin(\delta) + \cos(\varphi) \cos(\delta) \sin(\omega_s)]
$$

$$
d_r = 1+0.33 \cos(\frac{2 \pi}{365} J)
$$

$$
\delta = 0.408 \sin(\frac{2 \pi}{365} J - 1.39)
$$

$$
\omega_s = \arccos( - \tan(\varphi) \tan(\delta))
$$

where:
- $G_{sc}$ is the solar constant and equals to $0.082 MJ/(m^2 \times \min)$
- $d_r$ is the inverse relative distance between earth and sun
- $\omega_s$ is the sunset hour angle (rad)
- $\varphi$ the latitude of the weather station (rad)
- $\delta$ is the solar decimation (rad)
- $J$ is the number of day in a year, 1 for 1th January and 365 (or 366) for 31th December.

### Clear-sky shortwave radiation

$$
S_{CS} = (a_s + b_s) S_{ET}
$$

where, $a_s$ and $b_s$ are two empirical coefficients, with default values of 0.25 and 0.5.

### Effect of cloud cover
the effect of cloud cover on insolation can be estimated based on "percent possible sunshine": where available, daily observations of fraction of daylight hours receiving bright sunshine. The diminishing effect is formulated empirically as 

$$
\tau_C = a_s + b_s \frac{n}{N}
$$

$$
N = \frac{24}{\pi} \omega_s
$$

where
- $n$ is actual sunshine duration recorded in a day (hours)
- $N$ is the maximum possible duration of sunshine (hours)


### Effect of forest canopy
the effect of forest canopy on insolation is related to the ratio of the horizontally projected area of forest canopy to the total area of interest and the thickness and type of canopy. The leaf-area index ( $LAI$ ) is used to reflects both canopy extent and thickness, which is the ratio of total leaf area to ground-surface area. 

$$
\tau_F = (1-a_F) (1-a_s) \exp(-LAI)
$$

where:
- $a_F$ is the albedo of the vegetation
- $a_s$ is the albedo of the snow under the canopy (assumed to be 0.5)


### Effect of slope and aspect
as the grid cells in the model setup are assumed to horizontal, the effect of slope and aspect on solar insolation is neglected here.

$$
\tau_s = 1
$$

### Summary

the received shortwave (solar) radiation of snowpack is formulated as:

$$
S = S_{CS} \times \tau_C \times \tau_F \times \tau_s
$$

## Incoming longwave radiation 

$$
L_{in} = \varepsilon_{at} \cdot \sigma \cdot {T_{at}}^4
$$

where:
- $\varepsilon_{at}$ is the integrated effective emissivity of the atmosphere and forest canopy
- $\sigma$ is the Stefan-Boltzmann constant, $\sigma = 4.90 \times 10^{-9} \cdot MJ/(m^2 \cdot K^4 \cdot d)$
- $T_{at}$ is the effective radiating temperature of the atmosphere and canopy, in unit of $K$

The major problem is to find expressions for $\varepsilon_{at}$ and $T_{at}$, or equivalently, to estimate the value of $L_{in}$ under various conditions of cloudiness and forest cover.


### Clear-sky longwave radiation

$$
L_{in} = \varepsilon_{clr} \cdot \sigma \cdot {T_{a}}^4
$$

where:
- $T_{a}$ is near-surface air temperature, [K]
- $\varepsilon_{clr}$ is the clear-sky emissivity,  estimated as: 

$$
\varepsilon_{clr} = 0.83 - 0.18 \exp(-1.54 \cdot e_a)
$$

where $e_a$ is the near-surface vapor pressure, in unit of kPa and $e_a > 0.285$ kPa. 

### Effect of cloud cover

$$
\varepsilon_{sky} = (1-\frac{n}{N}) + \frac{n}{N} \cdot \varepsilon_{clr}
$$

where:
- $n/N$ is the ratio of actual insolation to clear-sky insolation. 

Abramowitz et al. (2012) found that under both clear and cloudy conditions, $L_{in}$ can be estimated with a simple empirical relation well without considering the clouds correcting:

$$
L_{in} = 2.7 e_a + 0.245 T_a - 45.14
$$

where $L_{in}$ is in $MJ/(m^2 \cdot d)$ and $e_a$ is in kPa.


### Effect of forest canopy

Like clouds, trees are very nearly bloackbodies with respect to longwave radiation, and they can be considered to be emitting radiant energy at a rate determined by their temperature. Since their temperature is close to the near-surface air temperature, their effect on the total integrated atmospheric emissivity can be modeled as:

$$
\varepsilon_{at} = (1-F) \varepsilon_{sky} + F
$$

where $F$ is the fractional forest cover.

### Summary

When the forest cover is absent, $\varepsilon_{at} = \varepsilon_{sky}$ are smaller than 1; however, $\varepsilon_{at} = 1$ for complete forest cover ( $F = 1$ ). 

## Reference

Abramowitz G., L. Pouyanné, and H. Ajami (2012). On the information content of surface meteorology for downward atmospheric long-wave radiation synthesis. Geographical Research Letters 39(5), doi: https://doi.org/10.1029/2011GL050726

