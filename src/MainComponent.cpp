#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(700,500);
    setLookAndFeel(&lookAndFeel);

    juce::AudioDeviceManager::AudioDeviceSetup preferredSetup;
    preferredSetup.bufferSize = 144;
    preferredSetup.sampleRate = 48000.0;

    deviceManager.initialise(2, 2, nullptr, true, {}, &preferredSetup);
    // Shared low-latency mode (~30 ms with this driver stack). WASAPI Exclusive Mode
    // reaches a 144-sample buffer (~15 ms) but both VB-Cable and HIFI-CABLE stall in
    // exclusive capture (callback fires once, then no data events). ASIO would be the
    // next step for sub-30 ms.
    deviceManager.setCurrentAudioDeviceType("Windows Audio (Low Latency Mode)", true);

    // Re-request the small buffer on the low-latency device type.
    deviceManager.getAudioDeviceSetup(preferredSetup);
    preferredSetup.bufferSize = 144;
    preferredSetup.sampleRate = 48000.0;
    const juce::String setupError = deviceManager.setAudioDeviceSetup(preferredSetup, true);

    if (setupError.isNotEmpty())
        juce::Logger::writeToLog("Device setup error: " + setupError);
    if (deviceManager.getCurrentAudioDevice() == nullptr)
        juce::Logger::writeToLog("No audio device is running after startup.");

    deviceManager.addAudioCallback(&engine);

    // Title + section headers
    titleLabel.setText("Clamp: Dynamic Range Compressor", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(26.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    routingHeader.setText("ROUTING", juce::dontSendNotification);
    routingHeader.setFont(juce::Font(13.0f, juce::Font::bold));
    routingHeader.setColour(juce::Label::textColourId, juce::Colour(CompressorLookAndFeel::colAccent));
    addAndMakeVisible(routingHeader);

    compHeader.setText("COMPRESSOR", juce::dontSendNotification);
    compHeader.setFont(juce::Font(13.0f, juce::Font::bold));
    compHeader.setColour(juce::Label::textColourId, juce::Colour(CompressorLookAndFeel::colAccent));
    addAndMakeVisible(compHeader);

    inputLabel.setText("Input Device", juce::dontSendNotification);
    inputLabel.setFont(juce::Font(12.0f));
    inputLabel.setColour(juce::Label::textColourId, juce::Colour(CompressorLookAndFeel::colSubtext));
    addAndMakeVisible(inputLabel);

    outputLabel.setText("Output Device", juce::dontSendNotification);
    outputLabel.setFont(juce::Font(12.0f));
    outputLabel.setColour(juce::Label::textColourId, juce::Colour(CompressorLookAndFeel::colSubtext));
    addAndMakeVisible(outputLabel);

    applyButton.setButtonText("Apply");
    applyButton.setColour(juce::TextButton::buttonColourId, juce::Colour(CompressorLookAndFeel::colAccent));
    applyButton.setColour(juce::TextButton::textColourOffId, juce::Colour(CompressorLookAndFeel::colCrust));
    addAndMakeVisible(applyButton);

    inputMenu.addItem("Choose Virtual Audio Device", 1); // Input for program
    outputMenu.addItem("Choose Headphones", 1); // Output from the program

    auto* deviceType = deviceManager.getCurrentDeviceTypeObject();

    if (deviceType != nullptr){
        auto inputNames = deviceType->getDeviceNames(true);

        for (int i = 0; i < inputNames.size(); i++){
            inputMenu.addItem(inputNames[i], i + 2);
        }
    }
    if (deviceType != nullptr){
        auto outputNames = deviceType->getDeviceNames(false);

        for (int i = 0; i < outputNames.size(); i++){
            outputMenu.addItem(outputNames[i], i + 2);
        }
    }

    outputMenu.setSelectedId(1);
    inputMenu.setSelectedId(1);

    addAndMakeVisible(inputMenu);
    addAndMakeVisible(outputMenu);

    applyButton.onClick = [this]()
    {
        applyAudioDeviceSetup();
        applyCompressorSettings();
    };

    // Compressor parameter knobs (UI only, no functionality yet)
    auto setupKnob = [this](juce::Slider& s, double min, double max, double interval,
                            double def, const juce::String& suffix)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setRange(min, max, interval);
        s.setValue(def);
        s.setTextValueSuffix(suffix);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
        s.setDoubleClickReturnValue(true, def);
        addAndMakeVisible(s);
    };

    setupKnob(thresholdSlider, -60.0,    0.0, 0.1, defaultThreshold, " dB");
    setupKnob(ratioSlider,       1.0,   20.0, 0.1, defaultRatio,     ":1");
    setupKnob(attackSlider,      0.1,  100.0, 0.1, defaultAttack,    " ms");
    setupKnob(releaseSlider,    10.0, 1000.0, 1.0, defaultRelease,   " ms");
    setupKnob(makeupSlider,      0.0,   24.0, 0.1, defaultMakeup,    " dB");

    // Knob name labels (centred above each knob)
    auto setupName = [this](juce::Label& l, const juce::String& text)
    {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };

    setupName(thresholdLabel, "Threshold");
    setupName(ratioLabel,     "Ratio");
    setupName(attackLabel,    "Attack");
    setupName(releaseLabel,   "Release");
    setupName(makeupLabel,    "Makeup");

    // Processing mode selector. IDs are 1-based; engine uses 0-based.
    modeMenu.addItem("Compressor",      1);
    modeMenu.addItem("Limiter",         2);
    modeMenu.addItem("Constant Volume", 3);
    modeMenu.setSelectedId(1, juce::dontSendNotification);
    modeMenu.onChange = [this]()
    {
        engine.setMode(modeMenu.getSelectedId() - 1);
    };
    addAndMakeVisible(modeMenu);

    bypassButton.setButtonText("Bypass");
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]()
    {
        engine.setBypass(bypassButton.getToggleState());
    };
    addAndMakeVisible(bypassButton);

    resetButton.setButtonText("Reset to Defaults");
    resetButton.onClick = [this]()
    {
        resetToDefault();
    };
    addAndMakeVisible(resetButton);
}

MainComponent::~MainComponent()
{
    // Detach the custom look and feel before it is destroyed.
    setLookAndFeel(nullptr);
    deviceManager.removeAudioCallback(&engine);
}

void MainComponent::applyAudioDeviceSetup()
{
    if(inputMenu.getSelectedId() == 1 || outputMenu.getSelectedId() == 1){
        applyButton.setButtonText("Please Select an Option");
        startTimer(2500);
        return;
    }

    deviceManager.getAudioDeviceSetup(audioSetup);

    audioSetup.inputDeviceName = inputMenu.getText();
    audioSetup.outputDeviceName = outputMenu.getText();
    audioSetup.bufferSize = 144;
    audioSetup.sampleRate = 48000.0;

    const juce::String error = deviceManager.setAudioDeviceSetup(audioSetup, true);

    if (error.isNotEmpty())
    {
        juce::Logger::writeToLog("Device setup error: " + error);
        applyButton.setButtonText("Device Error - See Log");
        startTimer(2500);
        return;
    }

    // Read back what the device manager actually opened (may differ from the request).
    juce::AudioDeviceManager::AudioDeviceSetup actual;
    deviceManager.getAudioDeviceSetup(actual);
    juce::Logger::writeToLog("Applied routing | in: " + actual.inputDeviceName
        + " | out: " + actual.outputDeviceName
        + " | rate: " + juce::String(actual.sampleRate)
        + " | buffer: " + juce::String(actual.bufferSize));

    applyButton.setButtonText("Changes Applied");
    startTimer(1500);

}

void MainComponent::paint(juce::Graphics& g)
{
    // Vertical background gradient
    juce::ColourGradient grad(juce::Colour(CompressorLookAndFeel::colBase),  0.0f, 0.0f,
                              juce::Colour(CompressorLookAndFeel::colCrust), 0.0f, (float) getHeight(), false);
    g.setGradientFill(grad);
    g.fillRect(getLocalBounds());

    // Rounded section panels
    auto drawPanel = [&g](juce::Rectangle<int> r)
    {
        if (r.isEmpty())
            return;

        g.setColour(juce::Colour(CompressorLookAndFeel::colPanel));
        g.fillRoundedRectangle(r.toFloat(), 12.0f);
        g.setColour(juce::Colour(CompressorLookAndFeel::colSurface));
        g.drawRoundedRectangle(r.toFloat(), 12.0f, 1.0f);
    };

    drawPanel(routingPanelBounds);
    drawPanel(compPanelBounds);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);

    // Title
    titleLabel.setBounds(area.removeFromTop(40));
    area.removeFromTop(8);

    // Reserve the bottom button row first
    auto buttonRow = area.removeFromBottom(48);
    area.removeFromBottom(12);

    // Routing panel
    routingPanelBounds = area.removeFromTop(104);
    auto routing = routingPanelBounds.reduced(16);
    routingHeader.setBounds(routing.removeFromTop(18));
    routing.removeFromTop(4);

    const int colGap = 16;
    const int colW   = (routing.getWidth() - colGap) / 2;
    auto leftCol  = routing.removeFromLeft(colW);
    routing.removeFromLeft(colGap);
    auto rightCol = routing;

    inputLabel.setBounds(leftCol.removeFromTop(16));
    inputMenu.setBounds(leftCol.removeFromTop(30));
    outputLabel.setBounds(rightCol.removeFromTop(16));
    outputMenu.setBounds(rightCol.removeFromTop(30));

    area.removeFromTop(12);

    // Compressor panel fills the remaining space
    compPanelBounds = area;
    auto comp = compPanelBounds.reduced(16);

    // Header row: section title on the left, mode dropdown on the right.
    auto compTop = comp.removeFromTop(30);
    modeMenu.setBounds(compTop.removeFromRight(180));
    compHeader.setBounds(compTop.withSizeKeepingCentre(compTop.getWidth(), 18));
    comp.removeFromTop(8);

    juce::Slider* knobs[5] = { &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider, &makeupSlider };
    juce::Label*  names[5] = { &thresholdLabel,  &ratioLabel,  &attackLabel,  &releaseLabel,  &makeupLabel  };

    const int cellW = comp.getWidth() / 5;
    for (int i = 0; i < 5; ++i)
    {
        auto cell  = (i == 4) ? comp : comp.removeFromLeft(cellW);
        auto inner = cell.reduced(4);
        names[i]->setBounds(inner.removeFromTop(18));
        knobs[i]->setBounds(inner);
    }

    // Bottom buttons: Bypass | Apply | Reset
    const int btnGap = 16;
    const int btnW   = (buttonRow.getWidth() - 2 * btnGap) / 3;
    bypassButton.setBounds(buttonRow.removeFromLeft(btnW));
    buttonRow.removeFromLeft(btnGap);
    applyButton.setBounds(buttonRow.removeFromLeft(btnW));
    buttonRow.removeFromLeft(btnGap);
    resetButton.setBounds(buttonRow);
}

void MainComponent::timerCallback()
{
    applyButton.setButtonText("Apply");
    stopTimer();
}

void MainComponent::applyCompressorSettings()
{
    currentThreshold = thresholdSlider.getValue();
    currentRatio     = ratioSlider.getValue();
    currentAttack    = attackSlider.getValue();
    currentRelease   = releaseSlider.getValue();
    currentMakeup    = makeupSlider.getValue();
    currentBypass    = bypassButton.getToggleState();

    // Push the committed values into the audio engine.
    engine.setThreshold((float) currentThreshold);
    engine.setRatio((float) currentRatio);
    engine.setAttack((float) currentAttack);
    engine.setRelease((float) currentRelease);
    engine.setMakeup((float) currentMakeup);
    engine.setBypass(currentBypass);
}

void MainComponent::resetToDefault()
{
    thresholdSlider.setValue(defaultThreshold);
    ratioSlider.setValue(defaultRatio);
    attackSlider.setValue(defaultAttack);
    releaseSlider.setValue(defaultRelease);
    makeupSlider.setValue(defaultMakeup);

}
