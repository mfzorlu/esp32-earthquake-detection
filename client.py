import paho.mqtt.client as mqtt # type: ignore
import requests
import json
import configparser

# read from credentials.txt
config = configparser.ConfigParser()
config.read("credentials.txt")

# Telegram informations
BOT_TOKEN = config["telegram"]["BOT_TOKEN"]
CHAT_ID   = config["telegram"]["CHAT_ID"]

# MQTT informations
MQTT_BROKER   = config["mqtt"]["BROKER"]
MQTT_PORT     = int(config["mqtt"]["PORT"])
MQTT_TOPIC    = config["mqtt"]["TOPIC"]
MQTT_USERNAME = config["mqtt"]["USERNAME"]
MQTT_PASSWORD = config["mqtt"]["PASSWORD"]

def send_telegram(message):
    url  = f"https://api.telegram.org/bot{BOT_TOKEN}/sendMessage"
    data = {"chat_id": CHAT_ID, "text": message}
    requests.post(url, data=data)

def on_connect(client, userdata, flags, rc):
    print("MQTT connected:", rc)
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    print("payload: ", payload)
    print("MQTT mesaj:", payload)
    try:
        data    = json.loads(payload)
        message = f"ESP32 Data:\nX: {data['x']} Y: {data['y']} Z: {data['z']}"
        print("message: ", message)
    except:
        message = f"ESP32 Raw: {payload}"
    send_telegram(message)

client = mqtt.Client(callback_api_version=2)
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.tls_set()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()
