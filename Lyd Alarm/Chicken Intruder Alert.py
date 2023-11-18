import pyaudio
import struct
import time
import winsound
import keyboard
from datetime import datetime
import os
from twilio.rest import Client
import paho.mqtt.publish as publish

# Set up PyAudio
p = pyaudio.PyAudio()

# Open stream
stream = p.open(
    format=pyaudio.paInt16,
    channels=1,
    rate=44100,
    input=True,
    input_device_index = 0,  # Use default input device
    frames_per_buffer=1024
)

# Twilio Setup
account_sid = 'mySID'
auth_token = 'myAUTH'
client = Client(account_sid, auth_token)

# Set up counter and time, and sleepPeriod (How long the alarm will wait before reactivating)
counter = 0
start_time = time.time()
end_time_for_interval = time.time() + 10
sleepPeriod = 45*60


# Variables

volume_level_limit = 0.2     # Volume level limit for alarm to go off
HIGH_occurence_limit = 20    # number of HIGH occurences for alarm to activate
time_limit = 20              # time limit for HIGH occurences in seconds


# MQTT Setup
mqtt_broker = '178.164.41.198'
port = 1883
topic = 'soundsensor'


def alertUser():
    #Published information to mqtt. Vi lagrer som timestamp. (Må først konvertere)
    payload = str(int(datetime.now().timestamp()))
    publish.single(topic, payload, hostname = mqtt_broker, port = port)

    # Creates a SMS with the following message.
    message = client.messages.create(
        from_ = '+19086692486',
        body = 'Your smart chicken house has detected potential predator activity on ' + datetime.now().strftime("%Y-%m-%d at %H:%M:%S") + '. Please check the coop immediately.',
        to = '+4799343638'
    )

    # Creates a call with the following voice message.
    call = client.calls.create(
        twiml = '<Response><Say>Your smart chicken house has detected potensial preditor activity at ' +  str(datetime.now().strftime("%H:%M:%S")) + '. Please check the coop immediately.</Say></Response>',
        to = '+4799343638',
        from_ = '+19086692486'
    )


# Continuously read and print volume level
while True:
    data = stream.read(1024)
    volume = max(struct.unpack("1024h", data))  # Modify number of bytes passed to struct.unpack
    volume = volume / (2**15)  # Normalize between -1 and 1
    volume = volume * 100  # Scale between 0 and 100

    # Check if volume exceeds volume level "L" and increment counter
    if volume > volume_level_limit:
        print(f'Volume: {volume:.2f}' + ", STØY at " + datetime.now().strftime("%H:%M:%S\n"))
        counter += 1

    # Check if counter is 1, if it is the timer will start and listen for X other HIGH sounds.
    # Also defining end time as a variable consisting of start time added with 10 seconds.

    if counter == 1:
        end_time_for_interval = time.time() + time_limit
        counter = 2

    # Generell tidsplassering, SET
    start_time = time.time()

    if start_time > end_time_for_interval:
        counter = 0
        print("RESET at " + datetime.now().strftime("%H:%M:%S\n"))
        end_time_for_interval = start_time + 3600*24
        
    # Check if counter is 30 or larger, and if so a alarm will play. Counter will reset, and start time will reset.

    if counter >= HIGH_occurence_limit:
        winsound.PlaySound("noise", winsound.SND_ASYNC)
        print("\nINTRUDER ALARM at " + datetime.now().strftime("%H:%M:%S\n"))
        alertUser()
        print("\nAlarmen vil være på standby i " + str(sleepPeriod/60) + " minutter før den begynner igjen.")
        time.sleep(sleepPeriod)
        counter = 0  # Reset counter


    if keyboard.is_pressed("esc"):   # Ser om "Esc" blir trykket, isåfall vil koden stoppe
        print("\nProgrammet har avsluttet suksessfullt.\n")
        time.sleep(5)
    
    # Lite delay for strømsparing
    time.sleep(0.05)
 
