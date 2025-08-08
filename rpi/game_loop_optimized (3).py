import random
import serial
import threading
import platform
from pathlib import Path
from time import sleep
from playsound3 import playsound
import tkinter as tk
from tkinter import messagebox
import DCLL

# === CONFIG ===
SOUND_PATHS = {
    "bgm": "sounds/General_Release.mp3",
    "final": "sounds/03. Before Every Load.mp3",
    "blank": "sounds/audio_temp gunshot_blank.wav",
    "live": "sounds/audio_temp gunshot_live.wav"
}

ESP_PORT = 'COM9'
ARDUINO_PORT = '/dev/ttyUSB1'
BAUDRATE = 115200
FINAL_ROUND = 3
ROUND_PAUSE_SECONDS = 3

arduino = serial.Serial(ARDUINO_PORT, BAUDRATE)
forbidden = [2, 3, 7, 15, 31, 61, 127]
random.seed()

# === FUNZIONI UTILI ===

def play_bgm_loop(sound_path, stop_event):
    while not stop_event.is_set():
        playsound(sound_path, False)

def get_sound_path(key):
    path = SOUND_PATHS.get(key, "")
    if not path:
        raise ValueError(f"Sound key '{key}' non valido.")
    return Path(path) if platform.system() == "Linux" else Path(path.replace("/", "\\"))

def init_serial(port):
    conn = serial.Serial(port, BAUDRATE)
    if not conn.is_open:
        conn.open()
    return conn

def new_mag():
    base = random.randint(1, 254)
    if base in forbidden:
        return new_mag()
    else:
        seq = bin(base)
        arduino.write(random.sample(seq, len(seq)).encode())
        return seq

def subtract(node, num):
    target = node
    dead = 0
    for i in range(num):
        if i == num:
            target.lives -= 1
            if target.lives == 0:
                dead = 1
            break
        else:
            target = target.next
    arduino.write(f"zp {target.data}".encode())
    return dead

def startup_gui(root):
    global giocatori, vite_iniziali
    giocatori = tk.IntVar()
    vite_iniziali = tk.IntVar()

    tk.Label(root, text="Numero giocatori (2-4)").pack()
    tk.Entry(root, textvariable=giocatori).pack()
    tk.Label(root, text="Numero vite (1-6)").pack()
    tk.Entry(root, textvariable=vite_iniziali).pack()
    tk.Button(root, text="Avvia", command=root.quit).pack()
    root.mainloop()
    root.destroy()

    el = None
    for i in range(0, 4):
        if i < giocatori.get():
            el = DCLL.insertAtBeginning(el, i, vite_iniziali.get(), True)
        else:
            el = DCLL.insertAtBeginning(el, i, vite_iniziali.get(), False)
        el = el.next
    return el, giocatori.get()

def reset_players(head_player):
    current = head_player
    while True:
        current.status = True
        current = current.next
        if current == head_player:
            break

def next_active_player(curr_player):
    rotations = 0
    while True:
        curr_player = curr_player.next
        if curr_player.status:
            break
        rotations += 1
    return curr_player, rotations + 1

# === MAIN LOOP ===

def game_loop():
    root = tk.Tk()
    root.title("Controller Gioco")
    root.geometry("500x400")

    status_frame = tk.Frame(root)
    status_frame.pack(fill="both", expand=True)
    status_label = tk.Label(status_frame, text="In attesa di iniziare...", font=("Arial", 14))
    status_label.pack(pady=10)

    current_player, numero_giocatori = startup_gui(root)
    head_player = current_player

    bgm_stop = threading.Event()
    threading.Thread(target=play_bgm_loop, args=(get_sound_path("bgm"), bgm_stop), daemon=True).start()
    esp = init_serial(ESP_PORT)
    round_number = 1

    while round_number <= FINAL_ROUND:
        mag = new_mag()
        curr_shell = mag[0]
        dead = 0
        next_turn = False

        status_label.config(text=f"Round {round_number} - Giocatori attivi: {numero_giocatori - dead}")
        root.update()

        while dead < numero_giocatori - 1:
            if not mag:
                mag = new_mag()

            incoming = esp.read_until(size=4).decode()
            if incoming.startswith("st"):
                player_index = int(incoming[3])
                if curr_shell == 0:
                    playsound(get_sound_path("blank"), False)
                    next_turn = player_index != 1
                else:
                    playsound(get_sound_path("live"), False)
                    dead += subtract(current_player, player_index)
                    next_turn = True
                status_label.config(text=f"Round {round_number} - Giocatori attivi: {numero_giocatori - dead}")
                root.update()
                esp.flush()

            incoming = esp.read_until(size=4).decode()
            if incoming.startswith("rk"):
                mag = mag[1:]
                curr_shell = mag[0] if mag else 0

            if next_turn:
                current_player, rotations = next_active_player(current_player)
                arduino.write(f"rt {rotations}".encode())
                next_turn = False

        messagebox.showinfo("Round Terminato", f"Round {round_number} finito. Giocatori eliminati: {dead}/{numero_giocatori - 1}")
        sleep(ROUND_PAUSE_SECONDS)

        if round_number == FINAL_ROUND:
            bgm_stop.set()
            playsound(get_sound_path("final"), False)
            messagebox.showinfo("Fine Gioco", "Partita terminata. Tutti tranne uno sono stati eliminati.")
        else:
            round_number += 1
            reset_players(head_player)

    if messagebox.askyesno("Rematch", "Vuoi giocare di nuovo?"):
        game_loop()

if __name__ == "__main__":
    game_loop()
