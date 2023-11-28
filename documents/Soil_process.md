
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
        8\phi \cdot K(\theta_s) \cdot \psi_b
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

Wigmostae t al. [1994] describe an explicit grid cell by grid
cell approach to route saturated subsurface flow. The theory
and computational scheme are developed for point elevation
data on an orthogonal grid. Grid cells are centered
at each elevation point (e.g., solid square surrounding the
elevation point $i, j$ in Figure 1). Each grid cell can exchange
water with its eight adjacent neighbors. Directions between a
node and its neighbors are assigned the index $k$ and numbered
from 0 to 7 in a clockwise direction beginning with the upper
left-hand node. For example, $k - 2$ corresponds to the direction
between elevation point $i, j$ and point $i + 1, j - 1$. Local
hydraulic gradients are approximated by local water table
slopes. On steep slopes with thin, permeable soils, hydraulic
gradients may be approximated by local ground surface slopes.

The rate of saturated subsurface flow at time $t$ from cell $i, j$
to its down-gradient neighbors may be calculated under Dupuit-
Forchheimer assumptions [Freeze and Cherry, 1979] as

$$
    q_{i,j,k} = - T_{i,j,k} \cdot \tan \beta_{i,j,k} \cdot w_{i,j,k}, 
    \beta_{i,j,k} < 0
$$

$$
    q_{i,j,k} = 0, \beta_{i,j,k} \geqslant 0
$$

where:
- $q_{i,j,k}$: the flow rate from cell $i,j$ in the $k$ flow direction 
- $T_{i,j,k}$: the transmissivity at cell $i,j$ corresponding to the $k$ direction
- $\beta_{i,j,k}$: the water table slope in the $k$ direction
- $w_{i,j,k}$: the width of flow

The power law transmissivity function may be specified as:

$$
    T_{i,j,k} = \frac{
        K_{oi,j,k} \cdot D_{i,j}
    }{
        n_{i,j}
    } \cdot
    (
        1 - \frac{z_{i,j}}{D_{i,j}}
    )^{n_{i,j}}
$$

where:
- $K_{oi,j,k}$: the saturated hydraulic conductivity at the soil surface in cell $i,j$ in the $k$ idrection
- $D_{i,j}$: the soil thickness at cell $i,j$
- $n_{i,j}$: the local power law exponent

By substituting, yields:

$$
    q_{i,j,k} = \gamma_{i,j,k} \cdot h_{i,j}
$$

where (for $\beta_{i,j,k} < 0$):

$$
    \gamma_{i,j,k} = - \frac{
        w_{i,j,k}K_{oi,j,k}D_{i,j}
    }{
        n_{i,j}
    } \cdot \tan \beta_{i,j,k}
$$

The total saturated subsurface outflow from cell $i,j$ ($Q_{out_{i,j}}$) is calculated as:

$$
    Q_{out_{i,j}} = h_{i,j} \cdot \sum_{k=0}^7 \gamma_{i,j,k}
$$

For model application, it is more efficient to reformulate $q_{i,j,k}(t)$ as:

$$
    q_{i,j,k}(t) = F_{i,j,k}Q_{out_{i,j,k}}
$$

where:

$$
    F_{i,j,k} = \frac{
        \gamma_{i,j,k}
    }{
        \sum_{k=0}^{7} \gamma_{i,j,k} 
    }
$$

The total inflow to cell $i,j$ from up-gradient cells ($Q_{in_{i,j}}$) is given by:

$$
    Q_{in_{i,j}} = \sum_{k=0}^7 F_{i,j,k} \cdot Q_{out_{i,j,k}}
$$

where in this case $k$ represents the source grid cell location. 

The change in $z_{i,j}$ over the time step is given by

$$
    \Delta z_{i,j} = \frac{1}{\phi_{i,j}} \cdot
    [
        \frac{
            Q_{out_{i,j}} - Q_{in_{i,j}} 
        }{
            A_{i,j}
        } - R_{i,j}
    ] \cdot \Delta t
$$

where:
- $\phi_{i,j}$: the local effective porosity
- $A_{i,j}$: the grid cell area (horizontal projection)
- $R_{i,j}$: percolation from unsaturated soil zone

Negative values of $z_{i,j}$ represent "exfiltration" of subsurface water to the surface, available for overland flow routing.

## References:

Brooks, R. H., and A. T. Corey, Hydraulic properties of porous
media, Hydrol. Pap., 3, Colo. State Univ., Fort Collins, 1964.

Freeze, R. A., and J. A. Cherry, Groundwater, Prentice-Hall, Englewood
Cliffs, N.J., 1979.

Entekhabi, D., and P.S. Eagleson, Land surface hydrology parameterization for atmospheric general circulation models: Inclusion of subgrid scale spatial variability and screening with a simple climate model, Rep. 325, Ralph M. Parsons Lab., 195 pp., Mass. Inst. of Technol., Cambridge, 1989.

Wigmosta, M. S., L. W. Vail, and D. P. Lettenmaier, A distributed
hydrology-vegetatiomn odel for complext errain, Water Resour. Res.,
30(6), 1665-1679, 1994.
