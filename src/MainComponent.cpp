#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(700,500);

    deviceManager.initialise(2, 2, nullptr, true);

    applyButton.setButtonText("Apply");
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

    applyButton.onClick = [this]{
        ApplyAudioDeviceSetup();
    }


}

void MainComponent::ApplyAudioDeviceSetup()
{
    if(inputMenu.getSelectedId() == 1 || outputMenu.getSelectedID() == 1){
        return;
    }

    deviceManager.getAudioDeviceSetup(audioSetup);

    audioSetup.inputDeviceName = inputMenu.getText();
    auidoSetup.outputDeviceName = outputMenu.getText();

    deviceManager.setAudioDeviceSetup(audioSetup, true);
}

void MainComponent::resized()
{
    applyButton.setBounds(150, 250, 100, 40);
    inputMenu.setBounds(100, 150, 200, 30);
    outputMenu.setBounds(100, 200, 200, 30);

}
