#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class CompressorProcessor
{
    public:
        enum Mode { Compressor, Limiter, ConstantVolume };

        void prepare(double newSampleRate);
        void process(float* const* channels, int numChannels, int numSamples);
        void setMode(Mode newMode);

        void setThreshold(float newThresholdDb);
        void setRatio(float newRatio);
        void setAttack(float newAttackMs);
        void setRelease(float newReleaseMs);
        void setMakeup(float newMakeupDb);

    private:
        void updateCoefficients();

        Mode mode { Mode::Compressor };
        static constexpr float noiseFloorDb { -60.0f };

        double sampleRate { 0.0 };

        // Parameters
        float thresholdDb { -40.0f };
        float ratio       { 10.0f };
        float attackMs    { 10.0f };
        float releaseMs   { 100.0f };
        float makeupDb    { 20.0f };

        // Precomputed ballistics coefficients (set in prepare / setAttack / setRelease)
        float attackCoeff  { 0.0f };
        float releaseCoeff { 0.0f };

        // Smoothed gain carried between samples/blocks (1.0 = no reduction)
        float envelope { 1.0f };
};