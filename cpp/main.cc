#include <iostream>
#include <portaudio.h>
#include <math.h>
#include "./include/gnuplot_i.hpp"
#include <fftw3.h>
#include <vector>
#include <algorithm>
#include "notes.hh"


#define SAMPLERATE 44100
#define CHUNK  16384
#define CHANNELS 1
#define FORMAT paInt32
#define RESOLUTION SAMPLERATE / CHUNK
#define THRESHOLD 50000000000

std::string get_note(double freq){
    std::vector<double> keys;

    for (auto note : notes){
        keys.push_back(note.first);
    }

    for (int i = 0; i < keys.size() - 1; i++){
        double current = keys[i];
        double next = keys[i + 1];

        if (freq > next){
            continue;
        }

        if ((next - freq) > (freq - current)){
            return notes[current];
        } else {
            return notes[next];
        }

    }
}

int audioCallback(const void *inputBuffer, void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo *timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData)
{
    // Cast the input buffer to the appropriate data type
    const int *in = static_cast<const int *>(inputBuffer);
	double sound [CHUNK];
	fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * CHUNK);

	std::vector<double> best_freq; 

	for (int i = 0; i < CHUNK; i++){
		sound[i] = in[i] / 2^15;
	}

	fftw_plan plan = fftw_plan_dft_r2c_1d(CHUNK, sound, output, FFTW_ESTIMATE);
	fftw_execute(plan);

	for (int i = 0; i <= CHUNK / 2; ++i) {
        double magnitude = std::sqrt(output[i][0] * output[i][0] + output[i][1] * output[i][1]);
        double frequency = i * RESOLUTION;
        if (magnitude > THRESHOLD){
			best_freq.push_back(frequency);
			//std::cout << "Frequency bin " << i << ": Frequency = " << frequency << " Hz, Amplitude = " << magnitude << std::endl;
		}
        
    }
	std::sort(best_freq.begin(), best_freq.end());
	if (best_freq.size() == 0){
		return paContinue;
	}
    std::cout << best_freq[0] << std::endl;
    std::cout << get_note(best_freq[0]) << std::endl;

	fftw_destroy_plan(plan);
	fftw_free(output);

    return paContinue;
}


int main()
{


	


    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cout << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Set up the audio stream parameters
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = CHANNELS;  // Mono input
    inputParameters.sampleFormat = FORMAT;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    // Open the audio stream
    PaStream *stream;
    err = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLERATE, CHUNK, paClipOff, audioCallback, NULL);
    if (err != paNoError)
    {
        std::cout << "Pa_OpenStream failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Start the audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cout << "Pa_StartStream failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    std::cout << "Reading live audio... Press Enter to stop." << std::endl;
    std::cin.get();  // Wait for user input

    // Stop and close the audio stream
    err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        std::cout << "Pa_StopStream failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        std::cout << "Pa_CloseStream failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

	


    // Terminate PortAudio
    err = Pa_Terminate();
    if (err != paNoError)
    {
        std::cout << "PortAudio termination failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    return 0;
}