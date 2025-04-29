import serial
ser = serial.Serial('COM9', 115200)

while True:
    inp = input("inserire messaggio")
    ser.write(inp.encode())
    print(ser.readline())