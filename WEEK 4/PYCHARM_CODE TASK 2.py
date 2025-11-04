

import serial
import time
import numpy as np

# === Configuration ===
arduino = serial.Serial("COM4", 9600, timeout=1)
time.sleep(2)

AUTHORIZED_ID = "0013069760"

print("RFID + Motion Access System Running...\n")

def read_mpu_data(samples=50):
    """Collects motion data from Arduino"""
    ax_list, ay_list, az_list, gx_list, gy_list, gz_list = [], [], [], [], [], []

    for _ in range(samples):
        arduino.write(b'M')  # Request MPU6050 data
        line = arduino.readline().decode().strip()
        if line:
            try:
                ax, ay, az, gx, gy, gz = map(int, line.split(','))
                ax_list.append(ax)
                ay_list.append(ay)
                az_list.append(az)
                gx_list.append(gx)
                gy_list.append(gy)
                gz_list.append(gz)
            except:
                continue
        time.sleep(0.05)

    return np.array(gx_list), np.array(gy_list), np.array(gz_list)

def detect_circular_motion(gx, gy, gz):
    """Simple heuristic: large variations in gyro data indicate motion"""
    threshold = 3000  # adjust based on real motion
    variation = np.std(gx) + np.std(gy) + np.std(gz)
    print(f"Motion Variation Score: {variation:.2f}")
    return variation > threshold

# === Main Loop ===
while True:
    try:
        card = input("Tap RFID card: ").strip()
        print("Card Scanned:", card)

        if card == AUTHORIZED_ID:
            print("✅ Authorized. Please perform a circular motion...")
            gx, gy, gz = read_mpu_data(samples=60)

            if detect_circular_motion(gx, gy, gz):
                print("🌀 Motion detected → Access Granted.")
                arduino.write(b'A')
            else:
                print("⚠️ No circular motion detected → Access Denied.")
                arduino.write(b'D')

        else:
            print("❌ Unauthorized Card.")
            arduino.write(b'D')

    except KeyboardInterrupt:
        print("\nExiting...")
        arduino.close()
        break
