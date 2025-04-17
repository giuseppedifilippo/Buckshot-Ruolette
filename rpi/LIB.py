
import random, DCLL


forbidden = [2, 3, 7, 15, 31, 61, 127]
random.seed()


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