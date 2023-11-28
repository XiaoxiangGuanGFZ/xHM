
# Soil processes

## 1. Infiltration

## 2. Unsaturaed soil moisture movement
The dynamics of unsaturated moisture movement are simulated
using a two-layer model. The thickness of the upper
zone ($d_1$) is equal to the average rooting depth of the
understory vegetation. The lower zone extends
from $d_1$ to the averageo verstory rooting depth. The understory
can only extract water from the upper zone, while the
overstory can remove water from both zones. Overstory
transpiration from each soil zone is calculated using(7), then
multiplied by the overstory root fraction in that zone. Soil
evaporation is restricted to the upper zone.


The massb alancefo r theu ppera ndl owerz onesis given by

$$
    d_1(\theta_1^{t + \Delta t} - \theta_1^t) = P_0 - P_1(\theta_1) - E_{to} - E_{tu} - E_s + V_{sat} - V_r
$$

$$
    d_2(\theta_2^{t + \Delta t} - \theta_2^t) = P_1(\theta_1) - P_2(\theta_2) - E_{to} + V_{sat}
$$

where:
- $d_1$ and $d_2$: upper and lower soil layer thickness, [m] 
- $\theta_j$: average soil moisture
- $P_0$: the volume of infiltrated rainfall or snowmelt, [m]
- $E_s$: the volume of evaporated soil moisture, [m]
- $V_{sat}$: the volume of water supplied by a rising water table, [m]
- $V_r$: the volume of return flow (generated when a rising water table reaches the ground surface)
- $P_1$ and $P_2$: downward volumes of water discharged from the upper and lower soil zones over the time step, respectively, [m]

### 2.1 Percolation
The discharge volumes $P_1$ and $P_2$ are based on average soil moisture conditions during the time step, $\Delta_t$. Discharge is calculated via Darcy's law assuming a unit hydraulic gradient:

$$
    p_j = K_\upsilon(\theta_j)
$$

where:
$K_\upsilon(\theta_j)$ is the soil vertical unsaturated hydraulic conductivity and $J$ equals 1 for the upper layer and 2 for the lower layer.

The Brooks-Corey [Brooks and Corey, 1964]
equation is used to calculate hydraulic conductivity:

$$
    K_\upsilon(\theta_j) = K_\upsilon(\theta_s)
    [\frac{
        \theta - \theta_r
    }{
        \phi - \theta_r
    }]^{2/m+3}
$$

where:
- $m$: the pore size distribution index
- $\phi$: the soil porosity
- $\theta_r$: residual soil moisture content
- $\theta_s$: the saturated moisture content, taken equal to $\phi$

the discharge volume over the time step is given as:

$$
    P_j = [\frac{K_\upsilon(\theta_j^t) + K_\upsilon(\theta_j^*)}{2}] \cdot \Delta t
$$

where: $\theta_j^* = \theta_j^t + P_{j-1}/d_j$.


### 2.2 Desorption
Soil water evaporation from the upper soil layer $E_s$ is given as a function of the potential evaporation demand at the soil surface $E_{ps}$ and the $_e$, the desorption volume. 

The sorptivity (the rate at which the soil loses moisture) is calculated using the method presented by Entekhabi and Eagleson [1989]:

$$
    S_e = [\frac{
        8\phi K(\theta_s) * \psi_b
    }{
        3(1+3m)(1+4m)
    }]^{1/2} \cdot
    [\frac{\theta}{\phi}]^{1/2m+2}
$$

The desorption volume ($F_e$) is obtained by 

$$
 F_e = S_e \cdot \Delta t^{1/2}
$$

where:
- $\psi_b$: the soil bubbling pressure

## 3. Saturation subsurface flow


