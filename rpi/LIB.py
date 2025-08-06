
import random, DCLL
import serial
arduino = serial.Serial('/dev/ttyUSB1', 115200, )

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
        arduino.write(random.sample(seq, len(seq)).encode())
        return seq
    #manda sequenza al sistema per mostrare le cartucce


def subtract(node, num):
    target = node
    for i in range(num) :
        if i == num :
            target.lives -=1
        else :
            target= target.next
    arduino.write(f"zp {target.data}".encode())


#genera la sequenza di giocatori
def startup() :
    el = None
    for i in range(1, int(input("Numero giocatori[2-4] : "))+1 ):
        el = DCLL.insertAtBeginning(el, i, 6, True)
        el = el.next
    return el
