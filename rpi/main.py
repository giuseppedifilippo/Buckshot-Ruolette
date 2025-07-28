<<<<<<< Updated upstream
=======
from asyncio import timeout
from unittest import case
import random, DCLL, serial
>>>>>>> Stashed changes
from playsound3 import playsound
import serial, LIB
import RPi.GPIO as GPIO
from time import sleep

global nextTurn
nextTurn = False
global currentPlayer
current_player, players = LIB.startup()



#Serial Begin
esp = serial.Serial('COM9', 115200, )
arduino = serial.Serial('/dev/ttyUSB1', 115200, )

<<<<<<< Updated upstream
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

mag = LIB.new_mag()

while True:

    if len(mag) == 0:
        mag = LIB.new_mag()
    curr = mag[0]
    esp.write(curr.encode())
    mag = mag[1:]


    #passa il turno al prossimo giocatore
    if nextTurn:
        current_player = current_player.next
        arduino.write("next".encode())
        nextTurn = False
        GPIO.output(18, GPIO.HIGH)
        sleep(0.1)
        GPIO.output(18, GPIO.LOW)
=======

#crea una nuova sequenza di munizioni
def new_mag():
    base = random.randint(1, 254)
    if base in forbidden:
        return new_mag()
    else:
        return bin(base)
    #manda sequenza al sistema per mostrare le cartucce

def subtract(player, map):
    map[player]["lives"] -= 1
    if map[player]["lives"] == 0:
        map[player]["status"] = False
        #send zapping sequence

#genera la sequenza di giocatori
def startup() :
    el = None
    players = {1: {"lives": 3, "status": True}}
    for i in range(1, int(input("Numero giocatori[2-4] : "))+1 ):
        el = DCLL.insertAtBeginning(el, i)
        el = el.next
        if i == 1 :
            continue
        else :
            players[i] = {"lives": 3, "status": True}
    return el, players


serial_port = '/dev/ttyUSB0'
ser = serial.Serial(serial_port, 9600)
>>>>>>> Stashed changes
