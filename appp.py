import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re
from matplotlib import style

# Set the style for better visual appearance
style.use('fivethirtyeight')

# COM port and baud rate (match Arduino)
ser = serial.Serial('COM5', 9600)

# Data storage
aqi_vals = []
co2_vals = []
nh3_vals = []
toluene_vals = []
times = []
counter = 0  # Time counter

# Set up the figure for plotting
fig, ax = plt.subplots(figsize=(10, 6))
fig.suptitle('Air Quality Monitor', fontsize=16)

def parse_arduino_data():
    """Parse incoming serial data to extract values based on the Arduino output format"""
    data = {}
    buffer = ""
    
    # Wait for the start of data
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        print(f"Read: {line}")  # Debug output
        
        if line == "===== AIR QUALITY =====":
            # Found the header, now read the next 7 lines to get all data
            for _ in range(7):
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                print(f"Data line: {line}")  # Debug output
                buffer += line + "\n"
            
            # Extract the values using regex
            general_aqi_match = re.search(r'General AQI: ([\d.]+)', buffer)
            co2_match = re.search(r'CO2: ([\d.]+)', buffer)
            nh3_match = re.search(r'NH3: ([\d.]+)', buffer)
            toluene_match = re.search(r'Toluene: ([\d.]+)', buffer)
            
            if general_aqi_match:
                data['AQI'] = float(general_aqi_match.group(1))
            if co2_match:
                data['CO2'] = float(co2_match.group(1))
            if nh3_match:
                data['NH3'] = float(nh3_match.group(1))
            if toluene_match:
                data['Toluene'] = float(toluene_match.group(1))
                
            break  # Exit the loop once we've processed a full data set
            
    return data

def animate(i):
    """This function will be called periodically to update the plot"""
    global counter
    
    try:
        if ser.in_waiting > 0:
            data = parse_arduino_data()
            print(f"Parsed data: {data}")  # Debug output
            
            if 'AQI' in data:
                # Append data to respective lists
                aqi_vals.append(data['AQI'])
                co2_vals.append(data.get('CO2', 0))
                nh3_vals.append(data.get('NH3', 0))
                toluene_vals.append(data.get('Toluene', 0))
                times.append(counter)
                counter += 1
                
                # Limit the number of points to 50 for performance
                if len(times) > 50:
                    aqi_vals.pop(0)
                    co2_vals.pop(0)
                    nh3_vals.pop(0)
                    toluene_vals.pop(0)
                    times.pop(0)
                
                # Clear and update the plot
                ax.clear()
                ax.set_xlabel("Time (samples)")
                ax.set_ylabel("Level")
                
                ax.plot(times, aqi_vals, label='General AQI', color='red', marker='o', linestyle='-', linewidth=2)
                ax.plot(times, co2_vals, label='CO₂ (ppm)', color='green', marker='s', linestyle='-')
                ax.plot(times, nh3_vals, label='NH₃ (ppm)', color='blue', marker='^', linestyle='-')
                ax.plot(times, toluene_vals, label='Toluene (ppm)', color='purple', marker='x', linestyle='-')
                
                ax.legend(loc='upper left')
                ax.grid(True)
                
                # Set appropriate y-axis limits with some padding
                all_values = aqi_vals + co2_vals + nh3_vals + toluene_vals
                if all_values:  # Make sure there are values
                    min_val = min(value for value in all_values if value > 0) * 0.8
                    max_val = max(all_values) * 1.2
                    ax.set_ylim(min_val, max_val)
                
                plt.tight_layout()
    except Exception as e:
        print(f"Error during animation: {e}")

# Create the animation object and store it in a variable
ani = animation.FuncAnimation(fig, animate, interval=2000, cache_frame_data=False)

# Show the plot window
plt.show()

# Make sure to close the serial connection when done
try:
    plt.show()  # This blocks until the window is closed
finally:
    ser.close()
    print("Serial connection closed")