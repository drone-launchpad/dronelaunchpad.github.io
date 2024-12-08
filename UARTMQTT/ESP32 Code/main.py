# Derived from: 
# * https://github.com/peterhinch/micropython-async/blob/master/v3/as_demos/auart.py
# * https://github.com/tve/mqboard/blob/master/mqtt_async/hello-world.py
# * https://github.com/peterhinch/micropython-mqtt
# * https://github.com/embedded-systems-design/external_pycopy-lib


import ssl

from mqtt_as.mqtt_as import MQTTClient
from mqtt_as.mqtt_local import wifi_led, blue_led, config
import uasyncio as asyncio
from machine import UART
import time
import logging
logging.basicConfig(level=logging.DEBUG)
from config import *
import my_oled
from lib import ssd1306
from lib import gfx
from time import sleep


MAXTX = 12

uart = UART(2, 9600,tx=17,rx=16)
uart.init(9600, bits=8, parity=None, stop=1,flow=0) # init with given parameters

async def receiver():
    b = b''
    sreader = asyncio.StreamReader(uart)
    while True:
        res = await sreader.read(1)
        if res==b'\r':
            await client.publish(TOPIC_PUB, b, qos=1)

            print('published', b)
            b = b''
        else:
            b+=res

# Subscription callback
def sub_cb(topic, msg, retained):

    print(f'Topic: "{topic.decode()}" Message: "{msg.decode()}" Retained: {retained}')
    my_oled.print_data(msg)
    uart.write(msg)
    uart.write('\r\n')
    time.sleep(.01)


# Demonstrate scheduler is operational.
async def sensors():
    s = True
    while True:
        await asyncio.sleep_ms(500)
        blue_led(s)
        s = not s

async def wifi_han(state):
    wifi_led(not state)
    print('Wifi is ', 'up' if state else 'down')
    await asyncio.sleep(1)

# If you connect with clean_session True, must re-subscribe (MQTT spec 3.1.2.4)
async def conn_han(client):
    await client.subscribe(TOPIC_SUB, 1)

async def main(client):
    try:
        await client.connect()
    except OSError:
        print('Connection failed.')
        return
    n = 0
    while True:
        await asyncio.sleep(5)
        print('publish', n)
        # If WiFi is down the following will pause for the duration.
        await client.publish(TOPIC_HB, '{} {}'.format(n, client.REPUB_COUNT), qos = 1)
        n += 1

# Define configuration

config['server'] = MQTT_SERVER
config['ssid']     = WIFI_SSID
config['wifi_pw']  = WIFI_PASSWORD

config['ssl']  = True
# read in DER formatted certs & user key
with open('certs/student_key.pem', 'rb') as f:
    key_data = f.read()
with open('certs/student_crt.pem', 'rb') as f:
    cert_data = f.read()
with open('certs/ca_crt.pem', 'rb') as f:
    ca_data = f.read()
ssl_params = {}
ssl_params["cert"] = cert_data
ssl_params["key"] = key_data
ssl_params["cadata"] = ca_data
ssl_params["server_hostname"] = MQTT_SERVER
ssl_params["cert_reqs"] = ssl.CERT_REQUIRED
config["time_server"] = MQTT_SERVER
config["time_server_timeout"] = 10

config['ssl_params']  = ssl_params

config['subs_cb'] = sub_cb
config['wifi_coro'] = wifi_han
config['connect_coro'] = conn_han
config['clean'] = True
config['user'] = MQTT_USER
config["password"] = MQTT_PASSWORD

# Set up client
MQTTClient.DEBUG = True  # Optional
client = MQTTClient(config)

asyncio.create_task(receiver())
try:
    asyncio.run(main(client))
finally:
    client.close()  # Prevent LmacRxBlk:1 errors
    asyncio.new_event_loop()


async def main(client):
    await client.connect()
    asyncio.create_task(receiver())
    while True:
        await asyncio.sleep(1)

# config.subs_cb = callback
# config.connect_coro = conn_callback

client = MQTTClient(config)
loop = asyncio.get_event_loop()
loop.run_until_complete(main(client))