#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "CompressorProcessor.h"

class AudioEngine : public juce::AudioIODeviceCallback
{
    public:
        AudioEngine();
        ~AudioEngine() override;

        void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

        void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                              int numInputChannels,
                                              float* const* outputChannelData,
                                              int numOutputChannels,
                                              int numSamples,
                                              const juce::AudioIODeviceCallbackContext& context) override;

        void audioDeviceStopped() override;

    private:
            double sampleRate { 0.0 };
            int bufferSize { 0 };
            CompressorProcessor processor;


};