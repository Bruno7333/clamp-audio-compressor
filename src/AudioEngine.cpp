#include "AudioEngine.h"

AudioEngine::AudioEngine()
{

}

AudioEngine::~AudioEngine()
{

}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{

}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const juce::AudioIODeviceCallbackContext& context)
{

}

void AudioEngine::audioDeviceStopped()
{

}