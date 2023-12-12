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

# Open microphone stream.
# Important notice. Be sure to set the microphone to default device, and close all other applications that uses the microphone to prevent 
stream = p.open(
    format=pyaudio.paInt16,
    channels=1,              # Number of channels
    rate=44100,              # Samplingrate in Herz
    input=True,
    input_device_index = 0,  # Use default input device
    frames_per_buffer=1024  
)

# Twilio Setup. Requires an active Twilio account. In this project the free 30 day trial is being used. twilio.com
#Account SID and auth token are values given by Twilio when account is activated.
account_sid = 'Your Twilio sid' 
auth_token = 'Your Twilio Auth-token'
client = Client(account_sid, auth_token)

# Set up counter and time, and sleepPeriod (How long the alarm will wait before reactivating)
counter = 0
start_time = time.time()
end_time_for_interval = time.time() + 10
sleepPeriod = 45*60


# Variables, with example values. For the alarm to go off with this setup the volume level needs to surpass 10% 15 times within 20 seconds. 
# For getting best results: Set the HIGH_occurence_limit very high and test the code in the chicken coop to see which values is appropriate. This will depend the chicken coop and the microphone.


volume_level_limit = 10       # Volume level limit for alarm to go off
HIGH_occurence_limit = 15     # number of HIGH occurences for alarm to activate
time_limit = 20               # time limit for HIGH occurences in seconds


# MQTT Setup
mqtt_broker = '178.164.41.198' #MQTT server IP-Adress
port = 1883                    #MQTT server TCP port
topic = 'soundsensor'          #MQTT topic


def alertUser():
    #Published information to mqtt, convert it then save it as timestamp
    payload = str(int(datetime.now().timestamp()))
    publish.single(topic, payload, hostname = mqtt_broker, port = port)

    # Creates a call with the following voice message.
    call = client.calls.create(
        twiml = '<Response><Say>Your smart chicken house has detected potensial preditor activity at ' +  str(datetime.now().strftime("%H:%M:%S")) + '. Please check the coop immediately.</Say></Response>',
        to = 'Your telephone number', #Formatted with +country code, then number: F.I: "+47 12345678"
        from_ = 'Twilio telephone number' #The Twilio telephone number is linked with your account on the website. 
    )

    # Creates a SMS with the following message.
    message = client.messages.create(
        from_ = 'Twilio telephone number',
        body = 'Your smart chicken house has detected potential predator activity on ' + datetime.now().strftime("%Y-%m-%d at %H:%M:%S") + '. Please check the coop immediately.',
        to = 'Users telephone number'
    )

# Continuously read and print volume level
while True:
    data = stream.read(1024)
    volume = max(struct.unpack("1024h", data))  # Modify number of bytes passed to struct.unpack
    volume = volume / (2**15)  # Normalize between -1 and 1
    volume = volume * 100  # Scale volume between 0 and 100

    # Check if volume exceeds volume level limit and add to counter
    if volume > volume_level_limit:
        #print(f'Volume: {volume:.2f}' + ", STØY at " + datetime.now().strftime("%H:%M:%S\n"))
        counter += 1

    # Check if counter is 1, if it is the timer will start and listen for other HIGH sounds.
    # Also defining end time as a variable consisting of start time added with 10 seconds.

    if counter == 1:
        end_time_for_interval = time.time() + time_limit
        counter = 2

    # General timeplacement
    start_time = time.time()

    if start_time > end_time_for_interval:
        counter = 0
        #print("RESET at " + datetime.now().strftime("%H:%M:%S\n"))
        end_time_for_interval = start_time + 3600*24
        
    # Check if counter is larger than the high occurance limit, and if so the alarm will go off. Sound is played off, then alarting user, counter will reset, and start time will reset.

    if counter >= HIGH_occurence_limit:
        winsound.PlaySound("noise", winsound.SND_ASYNC)
        #print("\nINTRUDER ALARM at " + datetime.now().strftime("%H:%M:%S\n"))
        alertUser()
        #print("\nAlarmen vil være på standby i " + str(sleepPeriod/60) + " minutter før den begynner igjen.")
        time.sleep(sleepPeriod)
        counter = 0  # Reset counter


    if keyboard.is_pressed("esc"):   # If "esc" is pressed, program will stop.
        #print("\nProgrammet har avsluttet suksessfullt.\n")
        time.sleep(5)
    
    # Adding a small delay for energysaving
    time.sleep(0.05)
 
