from asyncio import timeout
from unittest import case

import DCLL
import random
forbidden = [2,3,7,15,31,61,127]
random.seed()
def new_mag() :
    base = random.randint(1,254)
    if base in forbidden :
        return new_mag()
    else :
        return bin(base)

