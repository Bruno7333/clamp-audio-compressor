#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "CompressorLookAndFeel.h"
#include "AudioEngine.h"

class MainComponent : public juce::Component,
                      private juce::Timer
{
    public:
        MainComponent();
        ~MainComponent() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        // Declared first so it outlives every child component that uses it.
        CompressorLookAndFeel lookAndFeel;
        AudioEngine engine;

        // Text shown above sections / controls
        juce::Label titleLabel;
        juce::Label routingHeader;
        juce::Label compHeader;
        juce::Label inputLabel;
        juce::Label outputLabel;

        juce::TextButton applyButton;
        juce::ComboBox inputMenu;
        juce::ComboBox outputMenu;

        // Compressor parameter sliders
        juce::Slider thresholdSlider;
        juce::Slider ratioSlider;
        juce::Slider attackSlider;
        juce::Slider releaseSlider;
        juce::Slider makeupSlider;

        juce::Label thresholdLabel;
        juce::Label ratioLabel;
        juce::Label attackLabel;
        juce::Label releaseLabel;
        juce::Label makeupLabel;

        // Compressor buttons
        juce::TextButton bypassButton;
        juce::TextButton resetButton;

        juce::AudioDeviceManager deviceManager;
        juce::AudioDeviceManager::AudioDeviceSetup audioSetup;

        void applyAudioDeviceSetup();
        void timerCallback() override;
        void resetToDefault();
        void applyCompressorSettings();

        // Panel rectangles, computed in resized() and drawn in paint().
        juce::Rectangle<int> routingPanelBounds;
        juce::Rectangle<int> compPanelBounds;

        static constexpr double defaultThreshold = -18.0;
        static constexpr double defaultRatio     = 4.0;
        static constexpr double defaultAttack    = 10.0;
        static constexpr double defaultRelease   = 100.0;
        static constexpr double defaultMakeup    = 0.0;

        // Stored compressor settings, only updated when Apply is clicked.
        // Initialized to defaults so nothing changes until the user applies.
        double currentThreshold = defaultThreshold;
        double currentRatio     = defaultRatio;
        double currentAttack    = defaultAttack;
        double currentRelease   = defaultRelease;
        double currentMakeup    = defaultMakeup;
        bool   currentBypass    = false;
};