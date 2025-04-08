#questo file non fa altro che riprodurre il suono di sottofondo del gioco

from playsound3 import playsound
from pathlib import Path
from sys import platform
import pygame
if platform == "win32" :
    win_path = r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\General_Release.mp3"
    universal_path = Path(win_path)
elif platform == "linux" :
    linux_path = r"/home/pi/Buckshot-Ruolette/rpi/sounds/General_Release.mp3"
    universal_path = Path(linux_path)
#aggiungere l altro elif nel caso sia linux
playsound(universal_path)

