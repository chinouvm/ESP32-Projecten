# Subscriber
import paho.mqtt.client as mqtt

client = mqtt.Client()
client.username_pw_set("chinou", "277353")
client.connect("77.170.41.205", 1883)
alarm = True


def on_connect(client, userdata, flags, rc):
    print("Connected to a broker!")
    client.subscribe("mqtt/esp32")


def on_message(client, userdata, message):
    print(f"PAYLOAD: {message.payload.decode()}")
    if message.payload.decode() == "OPEND" and alarm:
        print("INBREKER!!! WEEWOO WEEWOO")
        # TODO: Logica wanneer alarm afgaat, geluid afspelen etc.


while True:
    client.on_connect = on_connect
    client.on_message = on_message
    client.loop_forever()
