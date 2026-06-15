#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"

class MyApplicationWindow : public juce::DocumentWindow 
{
    public:
        MyApplicationWindow() : juce::DocumentWindow("AudioLimiter",
                                        juce::Colours::lightgrey,
                                        juce::DocumentWindow::allButtons)
        {
            setResizable(false, false);
            setContentOwned(new MainComponent(), true);
            centreWidthSize(700, 500);
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
            return "AudioLimiter";
        }
        const juce::String getApplicationVersion() override
        {
            return "0.1.0";
        }
        void initialise(const juce::String& commandLine) override
        {
            myMainWindow.reset(new MyApplicationWindow());
            myMainWindow->setBounds(100, 100, 400, 500);
            myMainWindow->setVisible(true);
        }

        void shutdown() override
        {
            myMainWindow.reset();
        }
        
    
    private:
        std::unique_ptr<MyApplicationWindow> myMainWindow;
    
};


START_JUCE_APPLICATION(AudioLimiterApp)
