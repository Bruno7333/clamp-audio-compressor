#include "CompressorProcessor.h"
#include <cmath>

void CompressorProcessor::setThreshold(float newThresholdDb)
{
    thresholdDb = newThresholdDb;
}

void CompressorProcessor::setRatio(float newRatio)
{
    ratio = newRatio;
}

void CompressorProcessor::setAttack(float newAttackMs)
{
    attackMs = newAttackMs;
    updateCoefficients();
}

void CompressorProcessor::setRelease(float newReleaseMs)
{
    releaseMs = newReleaseMs;
    updateCoefficients();
}

void CompressorProcessor::setMakeup(float newMakeupDb)
{
    makeupDb = newMakeupDb;
}

void CompressorProcessor::updateCoefficients()
{
    if (sampleRate <= 0.0)
        return;

    const double attackSeconds  = attackMs  / 1000.0;
    const double releaseSeconds = releaseMs / 1000.0;

    attackCoeff  = (float) std::exp(-1.0 / (attackSeconds  * sampleRate));
    releaseCoeff = (float) std::exp(-1.0 / (releaseSeconds * sampleRate));
}

void CompressorProcessor::setMode(Mode newMode){
    mode = newMode;
}

void CompressorProcessor::prepare(double newSampleRate){
    sampleRate = newSampleRate;
    updateCoefficients();
}

void CompressorProcessor::process(float* const* channels, int numChannels, int numSamples){
    float makeupGain = juce::Decibels::decibelsToGain(makeupDb);

    for(int sample = 0; sample <numSamples; sample++){
        float maxMag = 0.0f;

        for(int c = 0; c < numChannels; c++)
            maxMag = std::max(maxMag, std::abs(channels[c][sample]));

        float levelDb = juce::Decibels::gainToDecibels(maxMag);
        float gainDb = 0.0f;

        switch (mode){
            case Compressor:
                if(levelDb > thresholdDb)
                    gainDb = (thresholdDb - levelDb) * (1.0f - (1.0f / ratio));
                break;
            case Limiter:
                if(levelDb > thresholdDb)
                    gainDb = thresholdDb - levelDb;
                break;
            case ConstantVolume:
                if(levelDb > noiseFloorDb)
                    gainDb = thresholdDb - levelDb;
                break;


        }


        float targetGain = juce::Decibels::decibelsToGain(gainDb);

        float coeff = (targetGain < envelope) ? attackCoeff : releaseCoeff; // ternary, if condition use first, else use second
        envelope = coeff * envelope + (1.0f - coeff) * targetGain;
        float totalGain = envelope * makeupGain;

        for (int c = 0; c < numChannels; ++c)
            channels[c][sample] *= totalGain;
    }

}
