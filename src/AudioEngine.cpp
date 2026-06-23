#include "AudioEngine.h"

AudioEngine::AudioEngine()
{

}

AudioEngine::~AudioEngine()
{

}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    bufferSize = device->getCurrentBufferSizeSamples();
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const juce::AudioIODeviceCallbackContext& context)
{

    for(int channel = 0; channel < numOutputChannels; channel++){
        if(channel < numInputChannels && inputChannelData[channel] != nullptr){
            for(int sample = 0; sample < numSamples; sample++){
                outputChannelData[channel][sample] = inputChannelData[channel][sample];
            }
        }
        else{
            for(int sample = 0; sample < numSamples; sample++){
                outputChannelData[channel][sample] = 0.0f;
            }
        }
    }

}

void AudioEngine::audioDeviceStopped()
{

}