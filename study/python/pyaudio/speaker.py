"""
PyAudio Example: Mac OS X-only: Play a wave file with channel maps.
"""

import pyaudio
import wave
import sys
import math
import audioop
#import sounddevice as sd
from collections import deque

chunk = 1024

PyAudio = pyaudio.PyAudio

#if len(sys.argv) < 2:
#    print("Plays a wave file.\n\nUsage: %s filename.wav" % sys.argv[0])
#    sys.exit(-1)

#wf = wave.open(sys.argv[1], 'rb')

p = PyAudio()

# standard L-R stereo
channel_map = (0, 1)

# reverse: R-L stereo
# channel_map = (1, 0)

# no audio
# channel_map = (-1, -1)

# left channel audio --> left speaker; no right channel
# channel_map = (0, -1)

# right channel audio --> right speaker; no left channel
# channel_map = (-1, 1)

# left channel audio --> right speaker
# channel_map = (-1, 0)

# right channel audio --> left speaker
# channel_map = (1, -1)
# etc...

try:
    stream_info = pyaudio.PaMacCoreStreamInfo(
        flags=pyaudio.PaMacCoreStreamInfo.paMacCorePlayNice, # default
        channel_map=channel_map)
except AttributeError:
    print("Sorry, couldn't find PaMacCoreStreamInfo. Make sure that "
          "you're running on Mac OS X.")
    sys.exit(-1)

print("Stream Info Flags:", stream_info.get_flags())
print("Stream Info Channel Map:", stream_info.get_channel_map())

# open stream
stream = p.open(
    format=pyaudio.paInt16,
    channels=1,
    rate=18000,
    output=True,
    output_host_api_specific_stream_info=stream_info)

while True:
  try:
    cur_data = stream.read(chunk)
    slid_win.append(math.sqrt(abs(audioop.avg(cur_data, 4))))
    #print(cur_data)

    if (sum([x > THRESHOLD for x in slid_win]) > 0):
      print('I heart something!')
      success = True
      break
    if time.time() - listening_start_time > 60:
      print('I don\'t hear anything already 20 seconds!')
      break
  except IOError:
    print('IOError')
    break

stream.stop_stream()
stream.close()

p.terminate()