"""
PyAudio Example: Make a wire between input and output (i.e., record a
few samples and play them back immediately).
This is the callback (non-blocking) version.
"""

import pyaudio
import time
import sys
import math
import audioop
# import sounddevice as sd
from collections import deque

# sd.query_devices()
# print(sd.default.device[0], sd.default.device[1])
# sd.default.device[0] = 1 # 改成这个之后就直接监听内置声音


WIDTH = 2
CHANNELS = 2
RATE = 18000
CHUNK = 1024  # CHUNKS of bytes to read each time from mic
SILENCE_LIMIT = 1  # Silence limit in seconds. The max ammount of seconds where
THRESHOLD = 1000  # The threshold intensity that defines silence
FORMAT = pyaudio.paInt16

if sys.platform == 'darwin':
    CHANNELS = 1

p = pyaudio.PyAudio()


def callback(in_data, frame_count, time_info, status):
    return (in_data, pyaudio.paContinue)


stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

cur_data = ''  # current chunk  of audio data
rel = RATE / CHUNK
print(rel)

slid_win = deque(maxlen=SILENCE_LIMIT * int(rel))
print(slid_win)

success = False

listening_start_time = time.time()
print(listening_start_time)

while True:
    try:
        cur_data = stream.read(CHUNK)
        slid_win.append(math.sqrt(abs(audioop.avg(cur_data, 4))))
        # print(cur_data)

        if sum([x > THRESHOLD for x in slid_win]) > 0:
            print('I heart something!')
            success = True
            break
        if time.time() - listening_start_time > 60:
            print('I don\'t hear anything already 20 seconds!')
            break
    except IOError:
        print('IOError')
        break

stream.close()

p.terminate()
