#!/usr/bin/env python3


import serial
import LIB
import threading
import time
from playsound3 import playsound
from typing import Optional, Tuple

class AudioManager:
    """Gestisce l'audio di sottofondo in un thread separato"""
    
    def __init__(self):
        self.current_thread = None
        self.stop_requested = threading.Event()
        self.current_track = None
        self.is_playing = False
        
    def play_track(self, track_path: str, loop: bool = True) -> None:
        """Avvia la riproduzione di una traccia audio"""
        self.stop()  # Ferma eventuali tracce in corso
        
        self.stop_requested.clear()
        self.current_track = track_path
        self.current_thread = threading.Thread(
            target=self._play_loop,
            args=(track_path, loop),
            daemon=True
        )
        self.current_thread.start()
        
    def _play_loop(self, track_path: str, loop: bool) -> None:
        """Loop di riproduzione audio nel thread separato"""
        self.is_playing = True
        
        try:
            while not self.stop_requested.is_set():
                print(f"Riproduzione: {track_path}")
                playsound(track_path, block=True)
                
                if not loop or self.stop_requested.is_set():
                    break
                    
                # Piccola pausa tra le ripetizioni
                time.sleep(0.1)
                
        except Exception as e:
            print(f"Errore riproduzione audio: {e}")
        finally:
            self.is_playing = False
            
    def stop(self) -> None:
        """Ferma la riproduzione audio"""
        if self.current_thread and self.current_thread.is_alive():
            print("Fermando audio di sottofondo...")
            self.stop_requested.set()
            
            # Attendi che il thread finisca (con timeout)
            self.current_thread.join(timeout=2.0)
            
            if self.current_thread.is_alive():
                print("Warning: Thread audio non si è fermato nel timeout previsto")
                
        self.is_playing = False
        
    def is_thread_alive(self) -> bool:
        """Controlla se il thread audio è attivo"""
        return self.current_thread and self.current_thread.is_alive()


class GameManager:
    """Classe principale per gestire il gameplay"""
    
    # Costanti di gioco
    SOUNDS = {
        'bgm_track1': "sounds/General_Release.mp3",
        'bgm_track2': "sounds/03. Before Every Load.mp3",
        'gunshot_blank': "sounds/audio_temp gunshot_blank.wav",
        'gunshot_live': "sounds/audio_temp gunshot_live.wav"
    }
    
    FINAL_ROUND = 3
    RESET_LIVES = 6
    
    def __init__(self):
        """Inizializza il game manager"""
        self.setup_serial_connections()
        self.reset_game_state()
        self.audio_manager = AudioManager()
        self.sound_effects_lock = threading.Lock()  # Per evitare sovrapposizioni negli effetti sonori
        
    def setup_serial_connections(self) -> None:
        """Configura le connessioni seriali"""
        try:
            self.esp = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
            self.arduino = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
        except serial.SerialException as e:
            print(f"Errore connessione seriale: {e}")
            raise
            
    def reset_game_state(self) -> None:
        """Resetta lo stato del gioco"""
        self.round = 1
        self.morti = 0
        self.next_turn = False
        self.last_round = False
        
    def start_game(self) -> None:
        """Inizia una nuova partita"""
        print("=== AVVIO NUOVO GIOCO ===")
        
        # Inizializza giocatori
        self.current_player, self.numero_giocatori = LIB.startup()
        
        # Inizializza sequenza munizioni
        self.mag = LIB.new_mag()
        self.curr_shell = int(self.mag[2])  # Skip '0b' prefix
        LIB.relay(self.curr_shell)
        
        # Avvia BGM
        self.start_background_music()
        
        print(f"Gioco iniziato con {self.numero_giocatori} giocatori")
        print(f"Round {self.round} - Sequenza munizioni generata")
        
    def start_background_music(self) -> None:
        """Avvia la musica di sottofondo"""
        try:
            track = self.SOUNDS['bgm_track1'] if self.round < self.FINAL_ROUND else self.SOUNDS['bgm_track2']
            print(f"Avvio BGM: {track}")
            self.audio_manager.play_track(track, loop=True)
        except Exception as e:
            print(f"Errore avvio BGM: {e}")
            
    def process_shot(self) -> None:
        """Elabora un colpo sparato"""
        try:
            # Leggi dati dall'ESP
            incoming = self.esp.read_until(size=4).decode()
            
            if not incoming.startswith("st"):
                return
                
            target_player = int(incoming[3]) if len(incoming) > 3 else 0
            
            if self.curr_shell == 0:  # Blank
                self.handle_blank_shot(target_player)
            else:  # Live
                self.handle_live_shot(target_player)
                
            self.esp.flush()
            
        except (UnicodeDecodeError, ValueError, IndexError) as e:
            print(f"Errore lettura dati ESP: {e}")
            
    def handle_blank_shot(self, target_player: int) -> None:
        """Gestisce un colpo a salve"""
        print("BLANK! Nessun danno")
        self.play_sound_effect(self.SOUNDS['gunshot_blank'])
        # Con blank, il giocatore mantiene il turno se si spara addosso
        self.next_turn = (target_player != self.get_current_player_id())
        
    def handle_live_shot(self, target_player: int) -> None:
        """Gestisce un colpo vero"""
        print(f"LIVE! Giocatore {target_player} colpito")
        self.play_sound_effect(self.SOUNDS['gunshot_live'])
        
        # Sottrai vita e controlla se il giocatore è morto
        is_dead = LIB.subtract(self.current_player, target_player)
        if is_dead:
            self.morti += 1
            print(f"Giocatore {target_player} eliminato!")
            
        self.next_turn = True
        
    def play_sound_effect(self, sound_path: str) -> None:
        """Riproduce un effetto sonoro in un thread separato"""
        def play_effect():
            with self.sound_effects_lock:
                try:
                    playsound(sound_path, block=True)
                except Exception as e:
                    print(f"Errore riproduzione effetto sonoro: {e}")
                    
        effect_thread = threading.Thread(target=play_effect, daemon=True)
        effect_thread.start()
        
    def get_current_player_id(self) -> int:
        """Ottiene l'ID del giocatore corrente"""
        return self.current_player.data
        
    def advance_magazine(self) -> None:
        """Avanza alla prossima cartuccia"""
        try:
            incoming = self.esp.read_until(size=4).decode()
            
            if incoming.startswith("rack"):
                self.mag = self.mag[1:]  # Rimuovi cartuccia usata
                
                if len(self.mag) <= 2:  # Solo '0b' rimasto
                    self.reload_magazine()
                else:
                    self.curr_shell = int(self.mag[2])
                    LIB.relay(self.curr_shell)
                    
        except (UnicodeDecodeError, IndexError) as e:
            print(f"Errore avanzamento caricatore: {e}")
            
    def reload_magazine(self) -> None:
        """Ricarica il caricatore con nuove munizioni"""
        print("Caricatore vuoto - Ricarica...")
        self.mag = LIB.new_mag()
        self.curr_shell = int(self.mag[2])
        LIB.relay(self.curr_shell)
        print("Nuovo caricatore pronto")
        
    def check_round_end(self) -> bool:
        """Controlla se il round è terminato"""
        if self.morti >= self.numero_giocatori - 1:
            self.end_round()
            return True
        return False
        
    def end_round(self) -> None:
        """Termina il round corrente"""
        print(f"=== FINE ROUND {self.round} ===")
        
        if self.last_round:
            self.end_game()
            return
            
        # Passa al round successivo
        self.round += 1
        self.morti = 0
        
        # Reset vite giocatori
        LIB.reset(self.current_player, self.RESET_LIVES)
        
        # Comunica reset all'Arduino
        try:
            self.esp.write(f"rs {self.RESET_LIVES}".encode())
        except serial.SerialException as e:
            print(f"Errore comunicazione reset: {e}")
            
        # Cambia musica per il round finale
        if self.round == self.FINAL_ROUND:
            self.switch_to_final_music()
            
        print(f"=== INIZIO ROUND {self.round} ===")
        
    def switch_to_final_music(self) -> None:
        """Cambia alla musica del round finale"""
        try:
            print("ROUND FINALE - Cambio musica...")
            self.audio_manager.play_track(self.SOUNDS['bgm_track2'], loop=True)
            self.last_round = True
            print("Musica finale avviata")
        except Exception as e:
            print(f"Errore cambio musica: {e}")
            
    def end_game(self) -> None:
        """Termina la partita"""
        print("=== PARTITA TERMINATA ===")
        
        # Ferma la musica
        self.audio_manager.stop()
        
        # Trova il vincitore
        winner = self.find_winner()
        if winner is not None:
            print(f"VINCITORE: Giocatore {winner.data}")
        
        # Richiedi rematch
        input("Premere INVIO per iniziare una nuova partita...")
        self.restart_game()
        
    def find_winner(self) -> Optional[object]:
        """Trova il giocatore vincitore"""
        current = self.current_player
        for _ in range(self.numero_giocatori):
            if current.lives > 0:
                return current
            current = current.next
        return None
        
    def restart_game(self) -> None:
        """Riavvia il gioco"""
        self.reset_game_state()
        self.start_game()
        
    def advance_turn(self) -> None:
        """Passa al turno del prossimo giocatore"""
        if not self.next_turn:
            return
            
        print("Cambio turno...")
        
        # Trova il prossimo giocatore vivo
        attempts = 0
        while attempts < self.numero_giocatori:
            self.current_player = self.current_player.next
            if self.current_player.lives > 0:
                break
            attempts += 1
            
        if attempts >= self.numero_giocatori:
            print("Errore: nessun giocatore vivo trovato!")
            return
            
        print(f"Turno del giocatore {self.current_player.data}")
        
        # Ricalibra sensore
        try:
            LIB.recalibrate()
        except Exception as e:
            print(f"Errore ricalibrazione: {e}")
            
        self.next_turn = False
        
    def run_game_loop(self) -> None:
        """Loop principale del gioco"""
        print("=== INIZIO GAME LOOP ===")
        
        try:
            while True:
                # Elabora colpi
                self.process_shot()
                
                # Avanza caricatore
                self.advance_magazine()
                
                # Controlla fine round
                if self.check_round_end():
                    continue
                    
                # Gestisci cambio turno
                self.advance_turn()
                
        except KeyboardInterrupt:
            print("\nGioco interrotto dall'utente")
            self.cleanup()
        except Exception as e:
            print(f"Errore nel game loop: {e}")
            self.cleanup()
            
    def cleanup(self) -> None:
        """Pulizia risorse"""
        print("Pulizia risorse...")
        
        # Ferma audio
        self.audio_manager.stop()
        
        # Attendi che tutti i thread audio terminino
        if self.audio_manager.is_thread_alive():
            print("Attendendo terminazione thread audio...")
            time.sleep(1)
            
        # Chiudi connessioni seriali
        try:
            if hasattr(self, 'esp') and self.esp.is_open:
                self.esp.close()
            if hasattr(self, 'arduino') and self.arduino.is_open:
                self.arduino.close()
        except Exception as e:
            print(f"Errore chiusura seriali: {e}")
            
        # Spegni relè
        LIB.relay(0)


def main():
    """Funzione principale"""
    try:
        game = GameManager()
        game.start_game()
        game.run_game_loop()
    except Exception as e:
        print(f"Errore fatale: {e}")
    finally:
        print("Programma terminato")


if __name__ == "__main__":
    main()
