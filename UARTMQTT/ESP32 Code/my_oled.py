from machine import Pin, SoftI2C
from lib import ssd1306
from lib import gfx
from time import sleep

# Initialize I2C and OLED display
i2c = SoftI2C(scl=Pin(33), sda=Pin(32))  # Match pins with your hardware

oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)
graphics = gfx.GFX(oled_width, oled_height, oled.pixel)

def print_data(msg):
    # Convert byte array to string
    my_string = msg.decode('utf-8')

    # Split string by spaces
    my_strings = my_string.split(" ")

    # Convert string values to float
    my_values = [float(item) for item in my_strings]

    # Extract values (assuming order: temp, hall, ready_flag)
    temp = my_values[0]
    hall = my_values[1]
    ready_flag = int(my_values[2])  # 0 or 1

    # Determine ready status
    ready_status = "Yes" if ready_flag == 1 else "No"

    # Clear screen
    oled.fill(0)

    # Print values on the OLED
    oled.text(f"Temp: {temp:.2f}", 0, 0)
    oled.text(f"Hall: {hall:.2f}", 0, 10)
    oled.text(f"Ready: {ready_status}", 0, 20)

    # Show the updated OLED screen
    oled.show()