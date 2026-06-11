#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

class MainComponent : public juce::Component {
    public:
        MainComponent();

        void resized() override;

    private:
        juce::TextButton pressMeButton;
        juce::ComboBox inputMenu;
        juce::ComboBox outputMenu;
        juce::AudioDeviceManager inputManager;
        juce::AudioDeviceManager outputManager;
};