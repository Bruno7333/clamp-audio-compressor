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
            // Send juce::Logger output to a file (also still goes to the debugger).
            fileLogger.reset(juce::FileLogger::createDefaultAppLogger("Clamp", "clamp.log", "Clamp started"));
            juce::Logger::setCurrentLogger(fileLogger.get());

            myMainWindow.reset(new MyApplicationWindow());
        }

        void shutdown() override
        {
            myMainWindow.reset();

            juce::Logger::setCurrentLogger(nullptr);
            fileLogger.reset();
        }
        
    
    private:
        std::unique_ptr<MyApplicationWindow> myMainWindow;
        std::unique_ptr<juce::FileLogger> fileLogger;
    
};


START_JUCE_APPLICATION(AudioLimiterApp)
