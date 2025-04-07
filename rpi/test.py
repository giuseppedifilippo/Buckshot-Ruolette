import paho.mqtt.client as mqtt

# Configurazione MQTT
broker = "192.168.50.1"  # IP del Raspberry Pi, che è l'Access Point
port = 1883
topic = "test/topic"

# Funzione callback per ricevere i messaggi
def on_message(client, userdata, message):
    print(f"Messaggio ricevuto: {message.payload.decode()}")

# Creare un client MQTT
client = mqtt.Client()

# Collegarsi al broker
client.connect(broker, port, 60)

# Impostare la funzione callback
client.on_message = on_message

# Iscriversi al topic
client.subscribe(topic)

# Pubblicare un messaggio
client.publish(topic, "Ciao dall'Raspberry Pi!")

# Esegui il ciclo di gestione degli eventi MQTT
client.loop_forever()
