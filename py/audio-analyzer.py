import pyaudio
import wave
import threading


import numpy as np

import notes

# Set the parameters
FORMAT = pyaudio.paInt32  # Sample size and format
CHANNELS = 1  # Mono audio
RATE = 44100  # Sample rate (Hz)
CHUNK = 16384  # Buffer size (number of frames)

# Initialize PyAudio
audio = pyaudio.PyAudio()

# Open the stream
stream = audio.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

print("Recording started...")
print("Press Enter to stop recording")

# Create a list to store the recorded audio frames
frames = []

# Create an event to signal when to stop recording
stop_event = threading.Event()

# Function to stop recording
def stop_recording():
    input()  # Wait for the Enter key press
    stop_event.set()



def snap_to_value(dictionary, value):
    keys = list(dictionary.keys())
    values = list(dictionary.values())

    # Check if dictionary keys are of type float
    if any(not isinstance(key, float) for key in keys):
        raise ValueError("Dictionary keys must be of type float")

    if value <= keys[0]:
        return dictionary[keys[0]]
    elif value >= keys[-1]:
        return dictionary[keys[-1]]
    else:
        closest_key = keys[0]
        closest_distance = float('inf')
        
        for key in keys:
            if abs(key - value) < closest_distance:
                closest_key = key
                closest_distance = abs(key - value)
        
        return dictionary[closest_key]
# Start a separate thread to listen for the stop signal
stop_thread = threading.Thread(target=stop_recording)
stop_thread.start()

# Continuously read audio data and append it to the frames list until stop signal received
while not stop_event.is_set():
    data = stream.read(CHUNK)


    sound = np.frombuffer(data, dtype=np.int32)
    sound = sound / 2.0**15


    length_in_s = CHUNK / RATE
    #print("length in s: ", length_in_s)

    fft_spectrum = np.fft.rfft(sound)
    freq = np.fft.rfftfreq(sound.size, d=1./RATE)
    fft_spectrum_abs = np.abs(fft_spectrum)

    smallest_f = None

    for i,f in enumerate(fft_spectrum_abs):
        if f > 5000000: #looking at amplitudes of the spikes higher than 350 
            #print('frequency = {} Hz with amplitude {} '.format(np.round(freq[i],1),  np.round(f)))
            smallest_f = freq[i]
            print(smallest_f)
            break
    if smallest_f is not None:
        print(snap_to_value(notes.notes, smallest_f))
    for i,f in enumerate(freq):
        if f < 21 or f > 500:# (2)
            fft_spectrum[i] = 0.0

    frames.append(data)

# Stop and close the stream
stream.stop_stream()
stream.close()
audio.terminate()
audio_array = np.frombuffer(b''.join(frames), dtype=np.int16)


print("Recording stopped.")


# Save the recorded audio to a WAV file
WAVE_OUTPUT_FILENAME = "output.wav"
wave_file = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
wave_file.setnchannels(CHANNELS)
wave_file.setsampwidth(audio.get_sample_size(FORMAT))
wave_file.setframerate(RATE)
wave_file.writeframes(b''.join(frames))
wave_file.close()

print(f"Audio saved to {WAVE_OUTPUT_FILENAME}.")

