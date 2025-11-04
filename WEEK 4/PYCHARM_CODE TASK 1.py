

import serial
import matplotlib.pyplot as plt
import numpy as np
from collections import deque
import time
import warnings

# Suppress harmless divide-by-zero warnings
warnings.filterwarnings("ignore", category=RuntimeWarning)

# === SETTINGS ===
PORT = 'COM4'          # Change this to your Arduino port
BAUD = 9600
MAX_POINTS = 100       # Number of points to keep on screen
SMOOTH_WINDOW = 5      # Moving average window
TRAIL_ALPHA = 0.3      # Trail transparency

# === SERIAL INIT ===
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # allow Arduino reset

# === PLOT INIT ===
plt.ion()
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_xlim(-20000, 20000)
ax.set_ylim(-20000, 20000)
ax.set_xlabel('Accel X')
ax.set_ylabel('Accel Y')
ax.set_title('MPU6050 Real-Time Motion Tracker')
ax.grid(True)

# Main moving point (red)
point, = ax.plot([], [], 'ro', markersize=6)
# Fading trail (light red line)
trail, = ax.plot([], [], 'r-', alpha=TRAIL_ALPHA)

x_vals, y_vals = deque(maxlen=MAX_POINTS), deque(maxlen=MAX_POINTS)

# === Smoothing Function ===
def smooth(values, new_val, window=SMOOTH_WINDOW):
    """Return a moving average of recent values."""
    if not values:
        return new_val
    recent = list(values)[-window:] + [new_val]
    return np.mean(recent)

# === Circle Detection (Optional) ===
def detect_circle(x, y, threshold=0.7):
    if len(x) < 20:
        return False
    if np.std(x) < 1e-6 or np.std(y) < 1e-6:
        return False
    x_norm = (x - np.mean(x)) / (np.std(x) + 1e-9)
    y_norm = (y - np.mean(y)) / (np.std(y) + 1e-9)
    corr = np.corrcoef(x_norm, np.roll(y_norm, 5))[0, 1]
    return corr > threshold

print(f"Reading from {PORT}...")
print("Press Ctrl + C to stop.")

# === Main Loop ===
try:
    while True:
        raw = ser.readline().decode(errors='ignore').strip()
        if ',' in raw:
            vals = raw.split(',')
            if len(vals) >= 2:
                try:
                    ax_val = int(vals[0])
                    ay_val = int(vals[1])

                    # Smooth values
                    ax_smooth = smooth(x_vals, ax_val)
                    ay_smooth = smooth(y_vals, ay_val)

                    # Append to deque
                    x_vals.append(ax_smooth)
                    y_vals.append(ay_smooth)

                    # Update trail and moving point
                    trail.set_data(x_vals, y_vals)
                    point.set_data([x_vals[-1]], [y_vals[-1]])

                    plt.draw()
                    plt.pause(0.01)

                    # Detect circular motion
                    if detect_circle(np.array(x_vals), np.array(y_vals)):
                        print("🔵 Circular motion detected!")

                except ValueError:
                    pass

except KeyboardInterrupt:
    print("\nStopped by user.")

finally:
    ser.close()
    plt.ioff()
    plt.show()
    print("Serial connection closed.")


