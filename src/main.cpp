#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"

class MyApplicationWindow : public juce::DocumentWindow 
{
    public:
        MyApplicationWindow() : juce::DocumentWindow("Clamp",
                                        juce::Colours::lightgrey,
                                        juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton)
        {
            setResizable(false, false);
            setContentOwned(new MainComponent(), true);
            centreWithSize(700, 500);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
};

class AudioLimiterApp : public juce::JUCEApplication
{
    public:
        const juce::String getApplicationName() override
        {
            return "Clamp";
        }
        const juce::String getApplicationVersion() override
        {
            return "0.1.0";
        }
        void initialise(const juce::String& commandLine) override
        {
            myMainWindow.reset(new MyApplicationWindow());
        }

        void shutdown() override
        {
            myMainWindow.reset();
        }
        
    
    private:
        std::unique_ptr<MyApplicationWindow> myMainWindow;
    
};


START_JUCE_APPLICATION(AudioLimiterApp)
