#include <portaudio.h>
#include <iostream>
#include <bitset>
#include <fftw3.h>
#include <cmath>



#define fs 96000

float findDominantFrequency(float* signal, int size, int sampleRate) {
    // Create a complex input array for FFT
    fftw_complex* input = fftw_alloc_complex(size);
    for (int i = 0; i < size; ++i) {
        input[i][0] = signal[i];
        input[i][1] = 0.0;
    }

    // Create the output array for FFT result
    fftw_complex* output = fftw_alloc_complex(size);

    // Create the FFT plan
    fftw_plan plan = fftw_plan_dft_1d(size, input, output, FFTW_FORWARD, FFTW_ESTIMATE);

    // Execute the FFT
    fftw_execute(plan);

    // Find the index of the dominant frequency component
    float maxMagnitude = 0.0;
    int dominantIndex = 0;
    for (int i = 0; i < size; ++i) {
        float magnitude = std::hypot(output[i][0], output[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            dominantIndex = i;
        }
    }

    // Calculate the corresponding frequency
    float dominantFrequency = dominantIndex * (sampleRate / static_cast<double>(size));

    // Clean up resources
    fftw_destroy_plan(plan);
    fftw_free(input);
    fftw_free(output);

    return dominantFrequency;
}


int audioCallback(const void *inputBuffer, void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo *timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData)
{
    // Cast the input buffer to the appropriate data type
    const float *in = static_cast<const float *>(inputBuffer);

    // Process the audio data as needed
    // Here, you can analyze or manipulate the audio samples
    int dataSize = sizeof(in) / sizeof(float);
    int sampleRate = fs;  // Sample rate of the audio
    float signal [framesPerBuffer];
    for (unsigned int i = 0; i < framesPerBuffer; ++i)
    {
        float sample = in[i];
        signal[i] = in[i];
        // Process the audio sample
        // Example: Print the sample value
        //std::cout << sample << std::endl;
    }
    float freq = findDominantFrequency(signal, dataSize, sampleRate);
    std::cout << "dominant frequency:" << freq << std::endl;
    //std::cout << inputBuffer << endl;
    // Return paContinue to continue streaming audio
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
    inputParameters.device = 9;
    inputParameters.channelCount = 1;  // Mono input
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    // Open the audio stream
    PaStream *stream;
    err = Pa_OpenStream(&stream, &inputParameters, NULL, fs , fs / 2, paClipOff, audioCallback, NULL);
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
