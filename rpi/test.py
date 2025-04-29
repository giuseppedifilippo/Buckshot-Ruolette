import pygame

pygame.mixer.init()
#bgm = pygame.mixer.Sound(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\General_Release.mp3")

pygame.mixer.music.load(r"C:\Users\giuseppe\Desktop\Buckshot-Ruolette\rpi\sounds\General_Release.mp3")
while True:

    pygame.mixer.music.play(-1)