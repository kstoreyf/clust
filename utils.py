import numpy as np


def real_to_zspace(position, velocity, L, redshift, Omega_m, w):
    position = np.array(position)
    velocity = np.array(velocity)
    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    position = (position + velocity*(1+redshift)/(E*100))%L
    return position