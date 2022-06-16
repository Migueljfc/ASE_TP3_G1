import time
import datetime
import smbus
import RPi.GPIO as GPIO


GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.setup(7,GPIO.OUT)

file = open('log.txt', 'w')

# Module variables
i2c_ch = 1
bus = None

# TC74 address on the I2C bus
i2c_address = 0x4d

# Register addresses
reg_temp = 0x00
reg_config = 0x01

# Read temperature registers and calculate Celsius
def read_temp():

    # Read temperature registers
    val = bus.read_i2c_block_data(i2c_address, reg_temp, 2)

    temp_c = (val[0] << 4) | (val[1] >> 4)

    # Convert registers value to temperature (C)
    temp_c = temp_c * 0.0625

    return temp_c

# Initialize I2C (SMBus)
bus = smbus.SMBus(i2c_ch)

total = 10
try:
	total = int(input("Read how many times? (default: 10): "))
except:
	pass
	
duration = 1.5
try:
	duration = float(input("Time between reads? (default: 1.5): "))
except:
	pass


# Print out temperature
for i in range (0,total):
    GPIO.output(7,True)
    time.sleep(duration)
    temperature = read_temp()
    file.write(str(i+1) + ':\t' + str(datetime.datetime.now()) +'\t'+ str(int(temperature)) + "ºC" +'\n')
    print('  ' + str(i+1) + ':\t ' + str(datetime.datetime.now()) +'\t'+ str(int(temperature)) + "ºC")
    GPIO.output(7,False)
    time.sleep(0.5)
    
GPIO.output(7,True)
file.close()

print('File with temperature values created')
print('Exiting...')
