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

    processor.prepare(sampleRate);
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

    // Pull the latest GUI values into the processor (audio thread reads atomics).
    processor.setThreshold(threshold.load());
    processor.setRatio(ratio.load());
    processor.setAttack(attack.load());
    processor.setRelease(release.load());
    processor.setMakeup(makeup.load());
    processor.setMode(static_cast<CompressorProcessor::Mode>(mode.load()));

    // When bypassed, leave the pass-through output untouched.
    if(!bypass.load())
        processor.process(outputChannelData, numOutputChannels, numSamples);
}

void AudioEngine::setThreshold(float newThresholdDb) { threshold.store(newThresholdDb); }
void AudioEngine::setRatio(float newRatio)           { ratio.store(newRatio); }
void AudioEngine::setAttack(float newAttackMs)       { attack.store(newAttackMs); }
void AudioEngine::setRelease(float newReleaseMs)     { release.store(newReleaseMs); }
void AudioEngine::setMakeup(float newMakeupDb)       { makeup.store(newMakeupDb); }
void AudioEngine::setBypass(bool shouldBypass)       { bypass.store(shouldBypass); }
void AudioEngine::setMode(int newMode)               { mode.store(newMode); }

void AudioEngine::audioDeviceStopped()
{

}