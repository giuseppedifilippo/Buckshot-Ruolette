from playsound3 import playsound
import serial, LIB
import RPi.GPIO as GPIO

global nextTurn
nextTurn = False
global currentPlayer
current_player, players = LIB.startup()

#ISR che si avvia quando viene premuto il grilleto
def ISR():
    if curr == 1:
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_live.wav", block=False)
    elif curr == 0:
        playsound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\audio_temp gunshot_blank.wav", block=False)
    else:
        print("errore")
        return
    #aggiungere sequenza che in base al numero del bersaglio colpito consulta la dcll e trova il giocatore colpito
    mex = int(esp.readline().decode())

    target = 0
    for i in range(1, mex + 1):
        target = i

    LIB.subtract(target, players)
    nextTurn = True
    return

#pin setting
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.add_event_detect(17, GPIO.FALLING, callback=ISR, bouncetime=200)
GPIO.setup(18, GPIO.OUT)

#Serial Begin
esp = serial.Serial('COM9', 115200, )
arduino = serial.Serial('/dev/ttyUSB1', 115200, )

mag = LIB.new_mag()

while True:

    if len(mag) == 0:
        mag = LIB.new_mag()
    curr = mag[0]
    esp.write(curr.encode())
    mag = mag[1:]

    p
    #cambia il giocatore attuale
    if nextTurn:
        current_player = current_player.next
        arduino.write("next".encode())
        nextTurn = False
        #aggiunge messaggio all esp per ricalibrare l MPU tramite interrupt
