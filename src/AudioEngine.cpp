#include "AudioEngine.h"
#include <cmath>
#include <algorithm>

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

    const int inLatency  = device->getInputLatencyInSamples();
    const int outLatency = device->getOutputLatencyInSamples();
    const double totalMs = (inLatency + outLatency + bufferSize) * 1000.0 / sampleRate;

    juce::Logger::writeToLog("Device: " + device->getName()
        + " | type: " + device->getTypeName()
        + " | rate: " + juce::String(sampleRate)
        + " | in: " + juce::String(inLatency) + " smp"
        + " | out: " + juce::String(outLatency) + " smp"
        + " | buffer: " + juce::String(bufferSize) + " smp"
        + " | total ~" + juce::String(totalMs, 2) + " ms");

    juce::String sizes;
    for (auto s : device->getAvailableBufferSizes())
        sizes << s << " ";
    juce::Logger::writeToLog("Available buffer sizes: " + sizes);

    loggedFirstCallback = false;
    processor.prepare(sampleRate);
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const juce::AudioIODeviceCallbackContext& context)
{
    if (!loggedFirstCallback)
    {
        loggedFirstCallback = true;
        juce::Logger::writeToLog("Audio callback running | numSamples: " + juce::String(numSamples)
            + " | in ch: " + juce::String(numInputChannels)
            + " | out ch: " + juce::String(numOutputChannels));
    }

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

    // Diagnostic: report the input peak every ~3 seconds so silent input is visible in the log.
    for (int c = 0; c < numInputChannels; ++c)
        if (inputChannelData[c] != nullptr)
            for (int s = 0; s < numSamples; ++s)
                peakSinceLog = std::max(peakSinceLog, std::abs(inputChannelData[c][s]));

    samplesSinceLog += numSamples;
    if (sampleRate > 0.0 && samplesSinceLog >= (int) (sampleRate * 3.0))
    {
        juce::Logger::writeToLog("Input peak (last 3s): " + juce::String(peakSinceLog, 4)
            + " | in ch: " + juce::String(numInputChannels)
            + " | out ch: " + juce::String(numOutputChannels));
        samplesSinceLog = 0;
        peakSinceLog = 0.0f;
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
    juce::Logger::writeToLog("Audio device stopped.");
}