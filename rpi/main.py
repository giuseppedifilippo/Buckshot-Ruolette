from asyncio import timeout
from unittest import case
import random, DCLL
import
#mappa che tiene tutte le variabili sui giocatori
players = {1: {}, 2: {}, 3: {}, 4: {}}
stats = {"lives": 0, "status": True}

forbidden = [2, 3, 7, 15, 31, 61, 127]
random.seed()


#crea una nuova sequenza di munizioni
def new_mag():
    base = random.randint(1, 254)
    if base in forbidden:
        return new_mag()
    else:
        return bin(base)


def subtract(player):
    players[player]["lives"] -= 1
    if players[player]["lives"] == 0:
        stats["status"] = False
        #send zapping sequence

#genera la sequenza di giocatori
el = None
for i in range(int(input("Numero giocatori[2-4] : "))):
    el = DCLL.insertAtBeginning(el, i)
    el = el.next


