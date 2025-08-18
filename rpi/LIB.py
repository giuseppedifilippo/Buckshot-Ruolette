
import random, DCLL, serial
from gpiozero import LED
from gpiozero import Button
arduino = serial.Serial('/dev/ttyUSB1', 115200, )
esp = serial.Serial('/dev/ttyUSB0', 115200, )
ralay_muzzle = 18
rele = LED(ralay_muzzle)
base = Button(19)
forbidden = [2, 3, 7, 15, 31, 61, 127]
random.seed()

#data una sequenza corretta di munizioni ne estrae il numero di blank e live e le manda all arduino per mostrarlo


#crea una nuova sequenza di munizioni
def new_mag():
    base = random.randint(1, 254)
    if base in forbidden:
        return new_mag()
    else:
        seq = bin(base)
        arduino.write(random.sample(seq, len(seq)).encode())#la sequenza viene randomizzataa ancora e mandata all arduino per mostrarle ai giocatori
        return seq
    #manda sequenza al sistema per mostrare le cartucce

#sottrae una vita dal giocatore colpito e manda il messaggio per dare la scossa al giocatore colpito
#node: puntatore al nodo corrente della lista concatenata
#id: identificatore del giocatore colpito
def subtract(node, id):
    target = node
    dead = 0
    for i in range(id) :
        if i == id :
            target.lives -= 1
            if target.lives == 0 :
                dead = 1
            break
        else :
            target = target.next
    arduino.write(f"zp {target.data}".encode())
    return dead


#genera la sequenza di giocatori e le informazioni relative a loro in base all input
def startup() :
    el = None
    giocatori = int(input("inserire numero giocatori (2-4)"))
    vite_iniziali = int(input("inserire numero vite (1-6)"))
    for i in range(0,4) :
        if i < giocatori :
            el = DCLL.insertAtBeginning(el, i, vite_iniziali, True)
        else :
            el = DCLL.insertAtBeginning(el, i, vite_iniziali, False)
        el = el.next
    return el, giocatori

#controlla il rele che guida l accensione dell finto muzzle flash
def relay(val) :
    if val == 1 :
        rele.on()
    else :
        rele.off()

def reset(node, lives) :
    curr = node
    while True :
        if curr.status :
            curr.lives = lives
        if curr != node :
            break


#controlla che il fucile sia sulla base prima di ricalibrare il sensore
def recalibrate() :
    base.wait_for_active()
    esp.write("rc".encode())