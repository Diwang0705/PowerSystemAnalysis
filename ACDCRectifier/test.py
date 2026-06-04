import numpy as np
import matplotlib.pyplot as plt
from rectifier import ACDCRectifier

def run_test():
    conv = ACDCRectifier(Vm=110, freq=50, dt=1e-4)
    steps = int(0.1/conv.dt)
    Vac_A_list = np.zeros(steps)
    Vac_B_list = np.zeros(steps)
    Vac_C_list = np.zeros(steps)
    Vdc_list = np.zeros(steps)

    for k in range(steps):
        out = conv.step()
        Vac_A_list[k] = out["Vac_A"]
        Vac_B_list[k] = out["Vac_B"]
        Vac_C_list[k] = out["Vac_C"]
        Vdc_list[k] = out["Vdc"]

    #    plot results
    plt.figure(figsize=(10,6))
    plt.plot(Vac_A_list, label='phase A AC Input (Vac)')
    plt.plot(Vac_B_list, label='phase B AC Input (Vac)')
    plt.plot(Vac_C_list, label='phase C AC Input (Vac)')
    plt.plot(Vdc_list, label='DC Output (Vdc)')
    plt.title("AC-DC Converter Simulation")
    plt.xlabel("Time step")
    plt.ylabel("Value")
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__=="__main__":
    run_test()