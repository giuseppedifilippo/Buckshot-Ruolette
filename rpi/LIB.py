
import random, DCLL
import serial
arduino = serial.Serial('/dev/ttyUSB1', 115200, )

forbidden = [2, 3, 7, 15, 31, 61, 127]
random.seed()


#crea una nuova sequenza di munizioni
def new_mag():
    base = random.randint(1, 254)
    if base in forbidden:
        return new_mag()
    else:
        arduino.write(bin(base).encode())
        return bin(base)
    #manda sequenza al sistema per mostrare le cartucce


def subtract(player, map):
    map[player]["lives"] -= 1
    if map[player]["lives"] == 0:
        map[player]["status"] = False
        #send zapping sequence
        arduino.write(("rm" + str(player)).encode())

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

#controlla che ci siano le condizioni per la vittoria di un giocatore e poi resetta il gioco
def check(mappa) :
    count = 0
    for k,v in range(mappa) :
        if v.lives == 0 :
            count+=1

    if count == len(mappa) -1 :
        print("winner")
        #aggiungere parte per resettare il gioco
        return True #nel caso rimane solo uno in vita manda questo check per dire al main di resettare il gioco



    return False #nel caso rimane più di un giocatore in vita continua il flusso n







