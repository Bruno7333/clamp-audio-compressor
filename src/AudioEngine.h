#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "CompressorProcessor.h"
#include <atomic>

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

        // Called from the GUI (message) thread. Thread-safe via atomics.
        void setThreshold(float newThresholdDb);
        void setRatio(float newRatio);
        void setAttack(float newAttackMs);
        void setRelease(float newReleaseMs);
        void setMakeup(float newMakeupDb);
        void setBypass(bool shouldBypass);
        void setMode(int newMode);

    private:
            double sampleRate { 0.0 };
            int bufferSize { 0 };
            CompressorProcessor processor;

            // Diagnostic: track the loudest input sample, logged every few seconds.
            int samplesSinceLog { 0 };
            float peakSinceLog { 0.0f };
            bool loggedFirstCallback { false };

            // Written by the GUI thread, read by the audio thread.
            std::atomic<float> threshold { -18.0f };
            std::atomic<float> ratio     { 4.0f };
            std::atomic<float> attack    { 10.0f };
            std::atomic<float> release   { 100.0f };
            std::atomic<float> makeup    { 0.0f };
            std::atomic<bool>  bypass    { false };
            std::atomic<int>   mode      { 0 };
};