#COSE DA FARE
#--far runnare la BGM in thread secondario e in loop
#--aggiungere funzione per il reset delle vite e la ripresa per il round successivo
#--gestire sequenza endgame, vittoria e rematch



from asyncio import timeout
from unittest import case
import random, DCLL, serial
from playsound3 import playsound
import serial, LIB
import RPi.GPIO as GPIO
from time import sleep
from playsound3 import playsound
from pathlib import Path
from sys import platform

# variabili globali necessarie al gameplay
global nextTurn
nextTurn = False
global currentPlayer
round = 1

# aggiungere seconda soundtrack per il round finale
if platform == "win32":
    win_path = r"sounds\General_Release.mp3"
    universal_path = Path(win_path)
elif platform == "linux":
    linux_path = r"sounds/General_Release.mp3"
    universal_path = Path(linux_path)
BGM = playsound(universal_path, False)

# Serial Begin
esp = serial.Serial('COM9', 115200, )
esp.open()
arduino = serial.Serial('/dev/ttyUSB1', 115200, )
arduino.open()
# chiama la funzione che crea la struttura dati per le info sui giocatori
current_player, numeroGiocatori = LIB.startup()
# crea una nuova sequenza e la manda all arduino
mag = LIB.new_mag()
curr_shell = mag[0]
dead = 0

while True:
    # modificare in modo tale che questa cosa avviene quando viene premuto il grilletto
    if len(mag) == 0:
        mag = LIB.new_mag()
    # esp.write(curr.encode())

    # busy wait per la pressione del colpo
    incoming = esp.read_until(size=4).decode()
    if incoming[:2] == "st":
        if curr_shell == 0:
            playsound(r"sounds\audio_temp gunshot_blank.wav", False)
            if incoming[3] == 1:
                nextTurn = False
        else:
            playsound(r"sounds\audio_temp gunshot_live.wav", False)
            dead += LIB.subtract(current_player, int(incoming[3]))
            nextTurn = True
        esp.flush()
    incoming = esp.read_until(size=4).decode()
    if incoming[:2] == "rk":
        mag = mag[1:]
        curr_shell = mag[0]
    # controlla se procedere al round successivo, aggiungere reset dei giocatori
    if dead == numeroGiocatori - 1:
        round += 1
        dead = 0
    if round == 3:
        BGM.stop()
        finalBGM = playsound(r"sounds\03. Before Every Load.mp3", False)

    # aggiungere check perchè il fucile sia posato sulla base
    # passa il turno al prossimo giocatore assicurandosi di saltare giocatori inattivi/morti e comanda la rotazione del base rotante
    if nextTurn:
        rotation_string = 0
        while True:
            current_player = current_player.next
            if current_player.status:
                rotation_string = 1
                break
            else:
                current_player = current_player.next
                rotation_string += 1
        arduino.write(f"rt {rotation_string}".encode())
        nextTurn = False
        # non ricordo cosa faccia ma serve a qualcosa che non ricordo
        GPIO.output(18, GPIO.HIGH)
        sleep(0.1)
        GPIO.output(18, GPIO.LOW)

# aggiungere Round Check
