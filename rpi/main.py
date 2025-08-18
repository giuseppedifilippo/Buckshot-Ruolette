#COSE DA FARE
#--gestire sequenza endgame, vittoria e rematch



import serial, LIB
from playsound3 import playsound

from rpi.LIB import recalibrate

# variabili globali necessarie al gameplay
global nextTurn
nextTurn = False
global currentPlayer
round = 1
BGM_track1 = r"sounds/General_Release.mp3"
BGM_track2 = r"sounds/03. Before Every Load.mp3"

#avvio della BGM
BGM = playsound(BGM_track1, False)

# Serial Begin
esp = serial.Serial('/dev/ttyUSB0', 115200, )
esp.open()
arduino = serial.Serial('/dev/ttyUSB1', 115200, )
arduino.open()


# chiama la funzione che crea la struttura dati per le info sui giocatori
current_player, numeroGiocatori = LIB.startup()

# crea una nuova sequenza e la manda all arduino
mag = LIB.new_mag()
curr_shell = mag[0]
LIB.relay(curr_shell)
morti = 0

while True:
    #se il caricatore è vuoto genera una nuova sequenza
    if len(mag) == 0:
        mag = LIB.new_mag()

    # busy wait per la pressione del colpo
    incoming = esp.read_until(size=4).decode()
    if incoming[:2] == "st":
        if curr_shell == 0:
            playsound(r"sounds/audio_temp gunshot_blank.wav", False)
            if incoming[3] == 1:
                nextTurn = False
        else:
            playsound(r"sounds/audio_temp gunshot_live.wav", False)
            morti += LIB.subtract(current_player, int(incoming[3]))
            nextTurn = True
        esp.flush()
    incoming = esp.read_until(size=4).decode()
    if incoming[:2] == "rk":
        mag = mag[1:]
        curr_shell = mag[0]
        LIB.relay(curr_shell)

    # controlla se procedere al round successivo,
    if morti == numeroGiocatori - 1:
        round += 1
        morti = 0
        LIB.reset(current_player, 6)
        if lastround :
            input("partita terminata, premere invio per rematch")
        esp.write("rs 6".encode())
    if round == 3:
        BGM.stop()
        BGM = playsound(BGM_track2, False)
        lastround = True

    # passa il turno al prossimo giocatore assicurandosi di saltare giocatori inattivi/morti e comanda la rotazione del base rotante
    if nextTurn:
        #rotation_string = 0
        while True:
            current_player = current_player.next
            if current_player.lives != 0:
                #rotation_string = 1
                break
            else:
                current_player = current_player.next
                #rotation_string += 1
        #arduino.write(f"rt {rotation_string}".encode())
        recalibrate()
        nextTurn = False

