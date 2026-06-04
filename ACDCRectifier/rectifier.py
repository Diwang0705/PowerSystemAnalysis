import numpy as np
import time

class ACDCRectifier:
#
#    AC to DC rectifier component modeling
#    Math formula:
#    V_ac = Vm * sin(w*t)
#    V_dc = |V_ac|
#
    def __init__(self, Vm = 110, freq = 50, dt = 1e-4):
        self.Vm   = Vm
        self.freq = freq
        self.w    = 2*np.pi*freq
        self.dt   = dt
        self.t    = 0.0

    def step(self):
    #   compute one simulation step
        start = time.time()
        Vac_A = self.Vm * np.sin(self.w * self.t)                   # Phase A AC input
        Vac_B = self.Vm * np.sin(self.w * self.t - 2*np.pi/3)       # Phase B AC input
        Vac_C = self.Vm * np.sin(self.w * self.t + 2*np.pi/3)       # Phase C AC input
        Vdc = max(Vac_A, Vac_B, Vac_C) - min(Vac_A, Vac_B, Vac_C)   # DC output
        self.t += self.dt

        return {
            "Vac_A": float(Vac_A),
            "Vac_B": float(Vac_B),
            "Vac_C": float(Vac_C),
            "Vdc": float(Vdc)
        }