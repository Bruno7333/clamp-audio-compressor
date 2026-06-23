#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

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
};