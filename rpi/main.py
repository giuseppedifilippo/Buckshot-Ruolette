from playsound3 import playsound
import serial, LIB
import RPi.GPIO as GPIO
from time import sleep

global nextTurn
nextTurn = False
global currentPlayer,mag
current_player, players = LIB.startup()
mag = LIB.new_mag()


#Serial Begin
esp = serial.Serial('COM9', 115200, )
arduino = serial.Serial('/dev/ttyUSB1', 115200, )

#ISR che si avvia quando viene premuto il grilleto
def isr():
    if curr == 1:
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_live.wav", block=False)
    elif curr == 0:
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_blank.wav", block=False)
    else:
        print("errore")
        return
    #aggiungere sequenza che in base al numero del bersaglio colpito consulta la dcll e trova il giocatore colpito
    mex = int(esp.readline().decode())
    temp = currentPlayer
    for i in range(1, mex + 1):
        temp = temp.next


    LIB.subtract(temp.data, players)
    nextTurn = True
    return

#pin setting
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.add_event_detect(17, GPIO.FALLING, callback=isr(), bouncetime=200)
GPIO.setup(18, GPIO.OUT)



while True:
    LIB.check(players)
    if len(mag) == 0:
        mag = LIB.new_mag()
    curr = mag[0]
    esp.write(curr.encode())


    #passa il turno al prossimo giocatore
    if nextTurn:
        mag = mag[1:]
        current_player = current_player.next
        arduino.write("next".encode())
        nextTurn = False
        GPIO.output(18, GPIO.HIGH)
        sleep(0.1)
        GPIO.output(18, GPIO.LOW)