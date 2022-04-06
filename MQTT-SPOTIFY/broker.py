# pip install hbmqtt paho-mqtt asyncio
# pip install "websockets==8.1"

import json
import requests
from secrets import base64, refresh_token
import logging
import asyncio
import requests
from hbmqtt.broker import Broker
from hbmqtt.client import MQTTClient, ClientException
from hbmqtt.mqtt.constants import QOS_1

# SPOTIFY API ZOOI
pauseurl = "https://api.spotify.com/v1/me/player/pause?device_id=5eb75b3a7444f02c0c2e45cbbfe7d1b7723c508b"
resumeurl = "https://api.spotify.com/v1/me/player/play?device_id=5eb75b3a7444f02c0c2e45cbbfe7d1b7723c508b"


def refresh():
    res = requests.post(
        "https://accounts.spotify.com/api/token",
        data={"grant_type": "refresh_token", "refresh_token": refresh_token},
        headers={"Authorization": "Basic " + base64},
    )

    resObject = res.json()
    return resObject["access_token"]


def pauseSong(url):
    refresh()
    res = requests.put(
        url,
        headers={
            "Content-Type": "application/json",
            "Authorization": "Bearer " + refresh(),
        },
    )
    print(res)


def resumeSong(url):
    refresh()
    res = requests.put(
        url,
        headers={
            "Content-Type": "application/json",
            "Authorization": "Bearer " + refresh(),
        },
    )
    print(res)


# MQTT ZOOI

logger = logging.getLogger(__name__)

config = {
    "listeners": {"default": {"type": "tcp", "bind": "0.0.0.0:1883"}},  # 0.0.0.0:1883
    "sys_interval": 10,
    "topic-check": {"enabled": False, "acl": {"anonymous": ["mqtt/deur"]}},
}

broker = Broker(config)


@asyncio.coroutine
def startBroker():
    yield from broker.start()


@asyncio.coroutine
def brokerGetMessage():
    C = MQTTClient()
    yield from C.connect("mqtt://localhost:1883/")
    yield from C.subscribe([("mqtt/deur", QOS_1)])
    logger.info("Subscribed!")
    try:
        for i in range(1, 100):
            message = yield from C.deliver_message()
            packet = message.publish_packet
            print("PAYLOAD: " + packet.payload.data.decode("utf-8"))
            if packet.payload.data.decode("utf-8") == "OPEN":
                print("PAUSE")
                pauseSong(pauseurl)
            elif packet.payload.data.decode("utf-8") == "DICHT":
                print("RESUME")
                resumeSong(resumeurl)
    except ClientException as ce:
        logger.error("Client exception : %s" % ce)


if __name__ == "__main__":
    formatter = "[%(asctime)s] :: %(levelname)s :: %(name)s :: %(message)s"
    logging.basicConfig(level=logging.INFO, format=formatter)
    asyncio.get_event_loop().run_until_complete(startBroker())
    asyncio.get_event_loop().run_until_complete(brokerGetMessage())
    asyncio.get_event_loop().run_forever()
