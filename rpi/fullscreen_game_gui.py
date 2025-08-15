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

class NumberPad:
    def __init__(self, parent, target_var, min_val, max_val, label_text):
        self.target_var = target_var
        self.min_val = min_val
        self.max_val = max_val
        self.current_value = ""
        
        # Frame principale per il tastierino
        self.frame = tk.Frame(parent, bg='#2c2c2c', relief='raised', bd=3)
        
        # Label per mostrare cosa stiamo inserendo
        self.title_label = tk.Label(self.frame, text=label_text, 
                                   font=('Arial', 24, 'bold'), 
                                   fg='white', bg='#2c2c2c')
        self.title_label.grid(row=0, column=0, columnspan=3, pady=20)
        
        # Display per il valore corrente
        self.display = tk.Label(self.frame, text="0", 
                               font=('Arial', 36, 'bold'), 
                               fg='lime', bg='black', 
                               width=10, relief='sunken', bd=3)
        self.display.grid(row=1, column=0, columnspan=3, pady=10, padx=20, sticky='ew')
        
        # Bottoni numerici (1-9)
        for i in range(1, 10):
            row = (i-1) // 3 + 2
            col = (i-1) % 3
            btn = tk.Button(self.frame, text=str(i), 
                           font=('Arial', 24, 'bold'),
                           width=4, height=2,
                           bg='#4a4a4a', fg='white',
                           activebackground='#6a6a6a',
                           command=lambda num=i: self.number_click(num))
            btn.grid(row=row, column=col, padx=5, pady=5)
        
        # Bottone 0
        btn_0 = tk.Button(self.frame, text="0", 
                         font=('Arial', 24, 'bold'),
                         width=4, height=2,
                         bg='#4a4a4a', fg='white',
                         activebackground='#6a6a6a',
                         command=lambda: self.number_click(0))
        btn_0.grid(row=5, column=1, padx=5, pady=5)
        
        # Bottone Cancella
        btn_clear = tk.Button(self.frame, text="C", 
                             font=('Arial', 24, 'bold'),
                             width=4, height=2,
                             bg='#cc4444', fg='white',
                             activebackground='#dd5555',
                             command=self.clear_click)
        btn_clear.grid(row=5, column=0, padx=5, pady=5)
        
        # Bottone Conferma
        btn_ok = tk.Button(self.frame, text="✓", 
                          font=('Arial', 24, 'bold'),
                          width=4, height=2,
                          bg='#44cc44', fg='white',
                          activebackground='#55dd55',
                          command=self.confirm_click)
        btn_ok.grid(row=5, column=2, padx=5, pady=5)
        
        # Label per i limiti
        limit_text = f"({min_val}-{max_val})"
        self.limit_label = tk.Label(self.frame, text=limit_text,
                                   font=('Arial', 16),
                                   fg='yellow', bg='#2c2c2c')
        self.limit_label.grid(row=6, column=0, columnspan=3, pady=10)
    
    def number_click(self, num):
        if len(self.current_value) < 2:  # Massimo 2 cifre
            self.current_value += str(num)
            self.update_display()
    
    def clear_click(self):
        self.current_value = ""
        self.update_display()
    
    def confirm_click(self):
        if self.current_value:
            value = int(self.current_value)
            if self.min_val <= value <= self.max_val:
                self.target_var.set(value)
                self.display.config(bg='#004400')  # Verde scuro per conferma
            else:
                # Valore fuori range - flash rosso
                self.display.config(bg='#440000')
                self.frame.after(500, lambda: self.display.config(bg='black'))
        else:
            # Nessun valore inserito
            self.display.config(bg='#440000')
            self.frame.after(500, lambda: self.display.config(bg='black'))
    
    def update_display(self):
        display_text = self.current_value if self.current_value else "0"
        self.display.config(text=display_text, bg='black')

def startup_gui(root):
    int giocatori, vite_iniziali
    
    # Configura finestra a schermo intero
    root.attributes('-fullscreen', True)
    root.configure(bg='#1a1a1a')
    
    # Variabili per i valori
    giocatori = tk.IntVar()
    vite_iniziali = tk.IntVar()
    
    # Frame principale
    main_frame = tk.Frame(root, bg='#1a1a1a')
    main_frame.pack(expand=True, fill='both')
    
    # Titolo
    title_label = tk.Label(main_frame, text="CONFIGURAZIONE GIOCO", 
                          font=('Arial', 48, 'bold'), 
                          fg='white', bg='#1a1a1a')
    title_label.pack(pady=50)
    
    # Frame per i tastierini
    keypad_frame = tk.Frame(main_frame, bg='#1a1a1a')
    keypad_frame.pack(expand=True)
    
    # Tastierino per giocatori
    players_pad = NumberPad(keypad_frame, giocatori, 2, 4, "NUMERO GIOCATORI")
    players_pad.frame.grid(row=0, column=0, padx=50, pady=20)
    
    # Tastierino per vite
    lives_pad = NumberPad(keypad_frame, vite_iniziali, 1, 6, "NUMERO VITE")
    lives_pad.frame.grid(row=0, column=1, padx=50, pady=20)
    
    # Frame per i bottoni di controllo
    control_frame = tk.Frame(main_frame, bg='#1a1a1a')
    control_frame.pack(pady=50

    
    # Bottone per avviare il gioco
    def start_game():
        if giocatori.get() == 0 or vite_iniziali.get() == 0:
            messagebox.showerror("Errore", "Inserisci tutti i valori richiesti!")
            return
        root.quit()
    
    start_btn = tk.Button(control_frame, text="AVVIA GIOCO", 
                         font=('Arial', 36, 'bold'),
                         width=15, height=2,
                         bg='#00aa00', fg='white',
                         activebackground='#00cc00',
                         command=start_game)
    start_btn.pack(side='left', padx=20)
    
    # Bottone per uscire
    exit_btn = tk.Button(control_frame, text="ESCI", 
                        font=('Arial', 36, 'bold'),
                        width=8, height=2,
                        bg='#aa0000', fg='white',
                        activebackground='#cc0000',
                        command=root.destroy)
    exit_btn.pack(side='left', padx=20)
    
    # Tasto ESC per uscire dalla modalità fullscreen
    def toggle_fullscreen(event=None):
        root.attributes('-fullscreen', False)
    
    root.bind('<Escape>', toggle_fullscreen)
    
    # Avvia la GUI
    root.mainloop()
    
    try:
        root.destroy()
    except:
        pass
    #spostare in startup gui

def reset_players(head_player):
    current = head_player
    while True:
        current.lives = 6
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
    
    # Configura la finestra principale a schermo intero
    root.attributes('-fullscreen', True)
    root.configure(bg='#0a0a0a')
    
    # Tasto ESC per uscire dalla modalità fullscreen
    def toggle_fullscreen(event=None):
        root.attributes('-fullscreen', False)
    
    root.bind('<Escape>', toggle_fullscreen)

    # Frame principale per il gioco
    game_frame = tk.Frame(root, bg='#0a0a0a')
    game_frame.pack(fill="both", expand=True)
    
    # Titolo del gioco
    title_label = tk.Label(game_frame, text="CONTROLLER GIOCO", 
                          font=("Arial", 60, "bold"), 
                          fg='white', bg='#0a0a0a')
    title_label.pack(pady=50)
    
    # Status frame
    status_frame = tk.Frame(game_frame, bg='#0a0a0a')
    status_frame.pack(fill="both", expand=True)
    
    status_label = tk.Label(status_frame, text="In attesa di iniziare...", 
                           font=("Arial", 36), 
                           fg='lime', bg='#0a0a0a')
    status_label.pack(expand=True)

    # Ottieni configurazione dal setup
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

        status_text = f"ROUND {round_number}\nGIOCATORI ATTIVI: {numero_giocatori - dead}"
        status_label.config(text=status_text)
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
                status_text = f"ROUND {round_number}\nGIOCATORI ATTIVI: {numero_giocatori - dead}"
                status_label.config(text=status_text)
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