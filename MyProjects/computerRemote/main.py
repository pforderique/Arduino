import json
import serial

from controller import Controller

computer = Controller()
arduino = serial.Serial(port='COM3', baudrate=115200, timeout=.1)

with open('ircodes.json') as f:
    ir_codes = json.load(f)

commands = {
    # main controls
    'POWER': computer.power,
    'OK': computer.enter,
    'MENU': computer.windows,

    # volume controls
    'MUTE': computer.mute,
    'VOL_UP': computer.volume_up,
    'VOL_DOWN': computer.volume_down,
 
    # cursor controls
    'UP': computer.up,
    'DOWN': computer.down,
    'LEFT': computer.left,
    'RIGHT':computer.right,
    'PMODE': computer.tab,

    # typing controls
    'DASH': computer.backspace,
    'ZERO': computer.space,      
    'ONE': computer.key_group1,
    'TWO': computer.key_group2,
    'THREE': computer.key_group3,
    'FOUR': computer.key_group4,
    'FIVE': computer.key_group5,
    'SIX': computer.key_group6,
    'SEVEN': computer.key_group7,
    'EIGHT': computer.key_group8,
    'NINE': computer.key_group9,

    # window controls
    'EXIT': computer.close_window,
}

print(id(computer.key_group1), id(computer.key_group2))

while True:
    ir_code = arduino.readline().decode("utf-8").strip('\r\n')
    # if ir_code: print(ir_code)
    if ir_code not in ir_codes or ir_codes[ir_code] == 'HOLD':
        continue

    command = ir_codes[ir_code]
    print(command)

    commands.get(command, lambda: None)() # call the command
