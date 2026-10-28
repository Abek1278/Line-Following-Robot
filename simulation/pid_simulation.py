"""
Line Follower PID Simulation (from Chapter 9)
Demonstrates the kinematic response of a line following robot
and the effect of Kp and Kd on overshoot and settling time.
"""

import math

def run(Kp, Kd, base=100, L=0.07, track=0.09, pitch=0.015, k=0.005, tau=0.05):
    """
    Simulates a 4-second run of the robot starting 3 cm off the line.
    
    Parameters:
      Kp: Proportional gain
      Kd: Derivative gain
      base: Base PWM speed
      L: Sensor arm length (m)
      track: Wheel track width (m)
      pitch: Sensor array pitch (m)
      k: Motor speed scaling factor
      tau: Motor mechanical response time constant (s)
    """
    y, th, vl, vr, last = 0.03, 0.0, k * base, k * base, 0.0   # Start 3 cm left of line
    dt, over, bad = 0.001, 0.0, 0.0
    
    for n in range(4000):                                      # 4 seconds at 1 kHz
        ys = y + L * math.sin(th)                              # Sensor position
        err = max(-3500, min(3500, ys / pitch * 1000))        # Error: + means line is to the right
        corr = Kp * err + Kd * (err - last)
        last = err
        
        left  = max(0, min(180, base + corr))
        right = max(0, min(180, base - corr))
        
        # Simulated motor inertia lag
        vl += (k * left  - vl) * dt / tau
        vr += (k * right - vr) * dt / tau
        
        # Kinematics update
        th += (vr - vl) / track * dt
        y  += (vl + vr) / 2 * math.sin(th) * dt
        
        over = max(over, -ys * 1000)                           # Overshoot in mm
        if abs(ys) > 0.002:                                    # Settled threshold (2 mm)
            bad = n * dt
            
    return round(over, 1), round(bad, 2)

if __name__ == "__main__":
    print(f"{'Kp':<8} {'Kd':<8} {'Overshoot (mm)':<16} {'Settled (s)':<12}")
    print("-" * 46)
    test_cases = [
        (0.01, 0.0),
        (0.05, 0.0),
        (0.10, 0.0),
        (0.40, 0.0),
        (0.10, 0.6),
        (0.40, 2.0),
        (0.10, 3.0),
    ]
    for Kp, Kd in test_cases:
        overshoot, settled = run(Kp, Kd)
        print(f"{Kp:<8} {Kd:<8} {overshoot:<16} {settled:<12}")
