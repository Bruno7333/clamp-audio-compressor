#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400,500);

    inputManager.initialise(2, 0, nullptr, true);
    outputManager.initialise(0, 2, nullptr, true);

    pressMeButton.setButtonText("Press Me");
    addAndMakeVisible(pressMeButton);

    inputMenu.addItem("Choose Input Device", 1);
    outputMenu.addItem("Choose Output Device", 1);

    auto* inputType = inputManager.getCurrentDeviceTypeObject();
    auto* outputType = outputManager.getCurrentDeviceTypeObject();

    if (inputType != nullptr){
        auto inputNames = inputType->getDeviceNames(true);

        for (int i = 0; i < inputNames.size(); i++){
            inputMenu.addItem(inputNames[i], i + 2);
        }
    }
    if (outputType != nullptr){
        auto outputNames = outputType->getDeviceNames(false);

        for (int i = 0; i < outputNames.size(); i++){
            outputMenu.addItem(outputNames[i], i + 2);
        }
    }

    outputMenu.setSelectedId(1);
    inputMenu.setSelectedId(1);

    addAndMakeVisible(inputMenu);
    addAndMakeVisible(outputMenu);


}

void MainComponent::resized()
{
    pressMeButton.setBounds(150, 250, 100, 40);
    inputMenu.setBounds(100, 150, 200, 30);
    outputMenu.setBounds(100, 200, 200, 30);

}
