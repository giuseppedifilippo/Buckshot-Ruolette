
from playsound3 import playsound
import serial, LIB
esp = serial.Serial('/dev/ttyUSB0', 115200, )
arduino = serial.Serial('/dev/ttyUSB1', 115200, )


mag = LIB.new_mag()
count = 0
while True:
    lista, giocatori = LIB.startup()
    if  len(mag) == 0:
        mag = LIB.new_mag()

    esp.write(mag[0].encode())
    mag = mag[1:]
    mex = esp.readline().decode()
    if mex[0] == "1":
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_live.wav", block=False)
    elif mex[0] == "0":
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_blank.wav", block=False)
    else:
        print("errore")

    LIB.subtract(1, giocatori)
    print(giocatori)