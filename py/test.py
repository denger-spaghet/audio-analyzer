




import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile

plt.rcParams['figure.dpi'] = 100
plt.rcParams['figure.figsize'] = (9, 7)

sampFreq, sound = wavfile.read('data/e1.wav')
sound = sound / 2.0**15


length_in_s = sound.shape[0] / sampFreq

fft_spectrum = np.fft.rfft(sound)
freq = np.fft.rfftfreq(sound.size, d=1./sampFreq)
fft_spectrum_abs = np.abs(fft_spectrum)

for i,f in enumerate(fft_spectrum_abs):
    if f > 800: #looking at amplitudes of the spikes higher than 350 
        print('frequency = {} Hz with amplitude {} '.format(np.round(freq[i],1),  np.round(f)))

for i,f in enumerate(freq):
    if f < 21 or f > 20000:# (2)
        fft_spectrum[i] = 0.0



plt.plot(freq[:300], np.abs(fft_spectrum[:300]))
plt.xlabel("frequency, Hz")
plt.ylabel("Amplitude, units")
plt.show()

