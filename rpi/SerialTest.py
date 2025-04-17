import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

while True:
    inp = input("inserire messaggio")
    ser.write(inp.encode())