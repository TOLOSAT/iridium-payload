# if you don't have all the libraries, just pip install what you need :
# pip install TLE-tools
# pip install astropy
# pip install poliastro
# pip install tqdm

# works best in spyder IDE (best & simple IDE in my opinion <3)
# also works fine in VScode

# documentation work reading :
# https://docs.poliastro.space/en/stable/#
# https://tletools.readthedocs.io/en/latest/index.html
# https://pyorbital.readthedocs.io/en/latest/#

# also this cool website with lots of neat equations
# https://www.omnicalculator.com


#%% general library import
import numpy as np
import matplotlib.pyplot as plt

from scipy import interpolate

# TLE tools import
from tletools import TLE

# poly astro library import
from astropy import units as u
import poliastro as pa

# to plot at current time
from astropy.time import Time

# just a progress bar
from tqdm import tqdm

# make the code run FASTER
from numba import jit

#%% TOLOSAT definition
now = Time.now()  # get the actual time

tolosat_orb = pa.twobody.orbit.Orbit.from_classical(
    pa.bodies.Earth,    # main attractor
    6878 * u.km,        # Semi-major axis
    0.002 * u.one,      # Eccentricity
    97.4 * u.deg,       # Inclination
    0.187 * u.rad,      # RAAN (Right ascension of the ascending node)
    np.pi/2 * u.rad,    # Argument of the pericenter
    0 * u.deg,          # true anomaly
    now                 # epoch (time)
)


#%% TLE to orbit conversion
# this cell convert list of TLE to list of poliastro.orbit
# download some fresh TLE : http://celestrak.com/NORAD/elements/

iridium_TLE = TLE.load('iridiumNEXT.txt')  # load TLE

iridium_orb = []  # initialization
for i in range(len(iridium_TLE)):  # convertion
    iridium_orb.append(iridium_TLE[i].to_orbit())


#%% synchronize every iridium satellite on given time t

def updateIridium(t):
    # propagate the position at given time t
    for i in range(len(iridium_orb)):
        iridium_orb[i] = iridium_orb[i].propagate(t)


updateIridium(now)  # synchronise every iridium to the current time


#%% inter-satellite distance calculation definition

@jit
def dist_between(a, b):
    return np.sqrt((a[0]-b[0])**2 +
                   (a[1]-b[1])**2 +
                   (a[2]-b[2])**2)


#%% relative speed to doppler shift converter

@jit
def vel2doppler_shift(v, f):
    # f = f₀(v + vr)/(v + vs)
    # v : wave speed
    # vr : receiver velocity (we consider it to be null)
    # vs : source velocity

    f0 = f
    c = 299792458  # speed of light in vacuum
    f_shift = (f0 * c)/(c + v)

    return f_shift - f0


#%% distance to corresponding signal attenuation converter

@jit
def dist2att(d, f):
    # attenuation (dB) = 20 * log10(d) + 20 * log10(f) + 20 * log10(4π / c)
    dist = d
    freq = f
    c = 299792458  # speed of light in vacuum (in m/s)

    loss = 20*np.log10(dist) + 20 * \
        np.log10(freq) + 20*np.log10(4*np.pi/c)

    return -loss


#%% function to determine visibility
@jit
def angleBetween(a, b):
    # array as input (ex : tolosat_orb.r.value and iridium_orb[0].r.value)
    # angle = arccos[(xa * xb + ya * yb + za * zb) / (√(xa2 + ya2 + za2) * √(xb2 + yb2 + zb2))]
    num = a[0]*b[0] + a[1]*b[1] + a[2]*b[2]
    denum = np.sqrt(a[0]**2 + a[1]**2 + a[2]**2) * \
        np.sqrt(b[0]**2 + b[1]**2 + b[2]**2)

    return np.arccos(num/denum)

@jit
def theySeeEachOther(a, b, alpha):
    # check if B is behind the horizon from A
    # by calculating the middle point and check if it's above surface
    mean_point = (a + b)/2
    alt_mean_point = np.sqrt(
        mean_point[0]**2 + mean_point[1]**2 + mean_point[2]**2)
    criteria_1 = alt_mean_point > 6371 * 1e3  # earth diameter

    # check if B is in a cone under A by checking the angle between
    # the vector from earth to A and
    # the vector from A to B
    vec_a = a
    vec_b = b - a
    criteria_2 = np.abs(angleBetween(vec_a, vec_b)) < alpha/2

    return criteria_1 & criteria_2  # return true if both criteria are true


#%% SIMULATION

step = 10 * u.minute # time between each step
nb_step = 6*24*7 # number of step
time_array = now + step * np.arange(nb_step)  # list of dates

dt = step.to(u.s).value

nb_sat = len(iridium_orb)

positions = np.zeros((3,nb_step, nb_sat+1))

print("\ncalculating positions...")
for t in tqdm(range(nb_step)):
    
    updateIridium(time_array[t])
    tolosat_orb = tolosat_orb.propagate(time_array[t])
    
    positions[:,t,0] = tolosat_orb.r.to(u.m).value
    
    for sat in range(nb_sat):
        positions[:,t,sat+1] = iridium_orb[sat].r.to(u.m).value
        
# interpolation to speed up calculation

subStep = 50
positions_interp = np.zeros((3,nb_step*subStep,nb_sat+1))

dt_interp = dt/subStep

x = np.arange(nb_step)
print("\ninterpolating positions...")
for sat in tqdm(range(nb_sat+1)):
    y = positions[:, :, sat]
    f0 = interpolate.interp1d(x, y[0,:], 'cubic')
    f1 = interpolate.interp1d(x, y[1,:], 'cubic')
    f2 = interpolate.interp1d(x, y[2,:], 'cubic')
    
    x_new = np.linspace(0, x[-1], len(x)*subStep)
    positions_interp[0, :, sat] = f0(x_new)
    positions_interp[1, :, sat] = f1(x_new)
    positions_interp[2, :, sat] = f2(x_new)
    

result = np.zeros((nb_step*subStep, nb_sat, 6))
# result description
# [time, satellite, :]
# categories
# [:,:,1] = attenuation loss (dB)
# [:,:,2] = relative velocity (m/s)
# [:,:,3] = doppler shift (Hz)
# [:,:,4] = doppler shift rate (Hz/s)
# [:,:,5] = reachable (y/n)

print("\ncalculating distance and attenuation...")
for t in tqdm(range(nb_step*subStep)):
    for sat in range(nb_sat):
        dist = dist_between(positions_interp[:,t,0], positions_interp[:,t,sat+1])
        result[t, sat, 0] = dist
        result[t, sat, 1] = dist2att(dist,  1.62125 * 1e9)
        
# the following derivations are done using finite difference method :
# https://www.wikiwand.com/en/Finite_difference_coefficient

print("\ncalculating relative velocities and doppler shift...")
for t in tqdm(np.arange(1, (nb_step*subStep)-1)):
    for sat in range(len(iridium_orb)):
        # the idea is to derivate the distance between tolosat and the other satellite
        # to get the relative velocity, then convert this velocity to doppler shift
        result[t, sat, 2] = 0.5*(result[t+1, sat, 0] - result[t-1, sat, 0])/dt_interp
        result[t, sat, 3] = vel2doppler_shift(
            result[t, sat, 2], 1.62125 * 1e9)

for sat in range(nb_sat):
    result[0, sat, 2] = (result[1, sat, 0] - result[0, sat, 0])/dt_interp
    result[0, sat, 3] = vel2doppler_shift(
        result[0, sat, 2], 1.62125 * 1e9)

for sat in range(nb_sat):
    result[-1, sat, 2] = (result[-1, sat, 0] - result[-2, sat, 0])/dt_interp
    result[-1, sat, 3] = vel2doppler_shift(
        result[-1, sat, 2], 1.62125 * 1e9)


print("\ncalculating doppler shift rate...")
for t in tqdm(np.arange(1, (nb_step*subStep)-1)):
    for sat in range(len(iridium_orb)):
        result[t, sat, 4] = 0.5*(result[t+1, sat, 3] - result[t-1, sat, 3])/dt_interp

for sat in range(nb_sat):
    result[0, sat, 4] = (result[1, sat, 3] - result[0, sat, 3])/dt_interp

for sat in range(nb_sat):
    result[-1, sat, 4] = (result[-1, sat, 3] - result[-2, sat, 3])/dt_interp


# the following calculation check if all the connectivity criteria are met
print("\ndetermine the reachability...")
for t in tqdm(range((nb_step*subStep))):
    for sat in range(nb_sat):
        result[t, sat, 5] = int((np.abs(result[t, sat, 3]) < 37500) &
                                (np.abs(result[t, sat, 4]) < 345) &
                                theySeeEachOther(positions_interp[:,t,0], positions_interp[:,t,sat+1], np.radians(120)))

nb_sat_reachable = np.zeros((nb_step*subStep))
for t in range((nb_step*subStep)):
    nb_sat_reachable[t] = np.sum(result[t, :, 5])

#%% PLT the number of reachable satellite

plt.figure()
plt.plot(np.arange(nb_step*subStep)*dt_interp/3600, nb_sat_reachable)
plt.ylabel('number of satellite reachable')
plt.xlabel('time (h)')
plt.title('coverage : ' + str(int(100*np.mean(nb_sat_reachable))) + '%')
plt.grid()

