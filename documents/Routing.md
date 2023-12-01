# Routing

## 1. Overland routing

### 1.1 Unit hydrograph
#### 1.1.1 Introduction
A unit hydrograph is use in which the watershed is decomposed into individual cells. The unit hydrograph is found for each cell and the response at the outlet is summed to produce the watershed runoff hydrograph. 
In the unit hydrograph approach, the time required for surface runoff generated at a given cell to travel to the basin outlet is calculated for each cell. The response function from each cell is a function of this travel time and can consist of both a linear translation component and a
linear storage component. The routing model was developed by
Maidment et al. [1994]

#### 1.1.2 Unit hydrograph
This approach is based on the convolution integral for the watershed as a lumped system which defines the direct runoff hydrograph $q(t)$ as a function of the excess rainfall hyetograph $I(\tau)$ using the unit impulse response function $u(t-\tau)$ as:

$$
    q(t) = \int_0^t I(\tau)u(t-\tau) d \tau
$$

where $I(t)$ and $q(t)$ have the dimension of [L/T], which can be interpreted as the discharge at the outlet per unit of drainage area of the watershed. Then the discharge at the outlet with dimension [L3/T] is given by multiplying the watershed drainage area $A$:

$$
    Q(t) = A \cdot q(t)
$$

Given that the watershed area is subdivided into $J$ subareas (or cells) of area $A_j, j = 1,2,3, ...,J$  that the excess rainfall rate varies by subarea as denoted by $I_j(t)$ and that an individual unit impulse response function $u_j(t-\tau)$ can be calculated for each subarea independent of all other subareas.The response at the outlet can then be found by summing the responses from each subarea:

$$
    Q(t) = \sum_{j=1}^J A_j \cdot \int_0^t I_j(\tau) u_j(t-\tau) d\tau
$$

Chow et al. (1988) present a theory of the unit hydrograph which shows how the unit impulse response
function (instantaneous unit hydrograph) is related to the unit step response function (S-hydrograph) and
to the unit pulse response function (unit hydrograph). By analogy for the subarea response, when the unit
impulse response function is integrated through time, the result is the subarea unit step response function, $g_j(t)$:

$$
    g_j(t) = \int_0^t u_j(\tau)d\tau
$$

which is a dimensionless function with a range from 0 to 1 expressing the proportion of a unit impulse input
occuring at time 0 on cell $j$ which appears at the outlet by time $t$.

If the unit input on subarea $j$ occurs at
a constant rate over a time interval $\Delta t$, the corresponding response at the outlet is given by the subarea unit pulse response function $h_j(t)$:

$$
    h_j(t) = \frac{1}{\Delta t} \cdot
    [g_j(t) - g_j(t- \Delta t)]
$$

In practice, precipitation intensity is not measured - what is measured is the incremental precipitation occurring over a time interval. If the time horizon is broken into intervals of duration $\Delta t$, the excess precipitation,
$P_{i,j}$ in the $i$ th time interval on the $j$ th subarea is given by:

$$
    P_{i,j} = \int_{(i-1)\Delta t}^{i \Delta t} 
    I_j(\tau) d \tau
$$

If discharge at the watershed outlet is defined on the same discrete time horizon, now indexed by $n$, then $Q_n$ is the instantaneous value of $Q(t)$ for $t=n \Delta t, n = 1,2,...,N$. The response at the outlet at time $t = n \Delta t$ to precipitation occurring in subarea $j$ during the $i$ th time interval is given by:

$$
    h_{n-i+1,j} = h_j[(n-i+1) \Delta t]
$$

The total discharge $Q_n$ is found by summing the lagged responses to all precipitation pulses over all subareas:

$$
    Q_n = \sum_{i=1}^n \sum_{j=1}^J A_j P_{i,j} h_{n-i+1,j}
$$

#### 1.1.3 spatial velocity field
It is appropriate to determine the circumstances under which the subarea unit impulse response function
can be calculated independently of all other subareas. These conditions may be stated as follows:

1). The watershed is subdivided into a finite number of non-overlapping subareas which collectively span
the whole drainage area and each subarea is connected to the watershed outlet by a single, continuous
flow path.

2). The impulse response function at the watershed outlet to excess rainfall on the subarea does not depend
on the magnitude of the excess rainfall.

3). In the event that the flow paths from two subareas share some path segments in common, the impulse
response function for flow from one subarea is not affected by the presence in these segments of flow
from the other subarea.


If a flow path from cell $j$ to the outlet traverses $m$ cells, $m = 1,2,..., M_j$, the flow length $L_j$ is defined as
the sum of the flow distances through each cell along the path:

$$
    L_j = \sum_{m=1}^{M_j}l_m
$$

where the cell flow distance $l_m$ is equal to the cell size or 1.41 times the cell size depending on whether the
flow direction is along the coordinate axes or along a diagonal, respectively.

If the velocity in cell $m$ is $V_m$, the flow time $T_j$ from cell $j$ to the outlet can
similarly be computed by summing the flow time through each of the $M_j$ cells on the path as:

$$
    T_j = \sum_{m=1}^{M_j} \frac{l_m}{V_m}
$$

The flow velocity $V_m$ is computed as:

$$
    V_m = \overline{V} \cdot (s^b  A^c) / \overline{s^b  A^c}
$$

where:
- $V_m$: the velocity assigned to the cell
- $s$: local slope of the cell
- $A$: upstream drainage area
- $\overline{V}$：average of the velocity in all cells in the watershed, a function of average slope and land cover characteristics. This can be determined by calibration or assumed 8-9m/min.
- $\overline{s^b  A^c}$：the corresponding watershed
average value of the slope-area term.
- $b$, and $c$ are coefficients determined by calibration (basically, $b=c=0.5$)

It turns out to be useful to limit the velocity values to an
acceptable range [$V_{min}$, $V_{max}$] to ensure that least a minimal velocity from areas of flat slope having little drainage area and to ensure that flow in large streams does not move with unrealistically high velocities. The upper and lower bounds of 220m/min and 1.65m/min can be used. 

#### 1.1.4 Subarea unit hydrograph (combined translation and storage model)

The response over the flow path is considered as each path has a translation and a storage time associated with it, in fixed proportions. The flow leaves a cell and enters a linear channel of translation time $T_s$ followed by a linear reservoir of average residence time $T_r$. Thus if the total flow time from a cell is $T$, then:

$$
    T=T_s + T_r
$$

and the $\beta$ of the residence time in the reservoir to the total flow time is constant:

$$
    \beta = \frac{T_r}{T}
$$

It is assumed here that the value of $\beta$ is the same for all cells in the watershed. The impulse response function for a lagged linear reservoir
is given by:


$$
    u(t) = 0, t < T_s
$$


$$
    u(t)=\frac{1}{T_r} \exp(1-\frac{t-T_s}{T_r}), t \geqslant T_s
$$

The corresponding unit pulse response function $h(t)$ for an input of duration $\Delta t$ is given by:


$$
    h(t) = 0, t < T_s
$$


$$
    h(t) = \frac{1}{\Delta t} [1 - \exp(
        \frac{t-T_s}{T_r}
    )], T_s \leqslant t \leqslant T_s + \Delta t
$$


$$
    h(t) = \frac{1}{\Delta t}
    \exp(
        - \frac{t-T_s}{T_r}
    ) [\exp(\frac{\Delta t}{T_r})-1], t > T_s + \Delta t
$$


where:
- $\beta$: value range 0.5-0.6


### 1.2 2D open channel flow



## 2. Channel routing

### 2.1 Muskingum-Cunge



### 2.2 Kinematic wave




## 3. References

Maidment, D.R., Olivera, F., Calver, A., Eatherall, A. and Fraczek, W.  1996.  Unit hydrograph derived from a spatially distributed velocity field. Hydrological Processes, 10(6), 831-844. doi: https://doi.org/10.1002/(SICI)1099-1085(199606)10:6<831::AID-HYP374>3.0.CO;2-N.



