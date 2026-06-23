#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cmath>

// A modern, dark "Catppuccin"-inspired look and feel for the compressor UI.
class CompressorLookAndFeel : public juce::LookAndFeel_V4
{
    public:
        // Shared palette
        static constexpr juce::uint32 colBase    = 0xff1e1e2e; // window background (top)
        static constexpr juce::uint32 colCrust   = 0xff11111b; // window background (bottom)
        static constexpr juce::uint32 colPanel    = 0xff181825; // panel fill
        static constexpr juce::uint32 colSurface  = 0xff313244; // borders / slider track
        static constexpr juce::uint32 colText     = 0xffcdd6f4; // primary text
        static constexpr juce::uint32 colSubtext  = 0xffa6adc8; // secondary text
        static constexpr juce::uint32 colAccent   = 0xff89b4fa; // accent (blue)

        CompressorLookAndFeel()
        {
            // Sliders
            setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(colAccent));
            setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(colSurface));
            setColour(juce::Slider::thumbColourId,               juce::Colours::white);
            setColour(juce::Slider::textBoxTextColourId,         juce::Colour(colText));
            setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colours::transparentBlack);
            setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);

            // Combo boxes
            setColour(juce::ComboBox::backgroundColourId, juce::Colour(colSurface));
            setColour(juce::ComboBox::textColourId,       juce::Colour(colText));
            setColour(juce::ComboBox::outlineColourId,    juce::Colour(0xff45475a));
            setColour(juce::ComboBox::arrowColourId,      juce::Colour(colAccent));

            // Popup menus (combo box drop-downs)
            setColour(juce::PopupMenu::backgroundColourId,            juce::Colour(colSurface));
            setColour(juce::PopupMenu::textColourId,                  juce::Colour(colText));
            setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(colAccent));
            setColour(juce::PopupMenu::highlightedTextColourId,       juce::Colour(colCrust));

            // Buttons
            setColour(juce::TextButton::buttonColourId,   juce::Colour(colSurface));
            setColour(juce::TextButton::buttonOnColourId, juce::Colour(colAccent));
            setColour(juce::TextButton::textColourOffId,  juce::Colour(colText));
            setColour(juce::TextButton::textColourOnId,   juce::Colour(colCrust));

            // Labels
            setColour(juce::Label::textColourId, juce::Colour(colText));
        }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider& slider) override
        {
            auto bounds    = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8.0f);
            auto radius    = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            auto centreX   = bounds.getCentreX();
            auto centreY   = bounds.getCentreY();
            auto toAngle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto lineW     = juce::jmin(7.0f, radius * 0.25f);
            auto arcRadius = radius - lineW * 0.5f;

            // Filled knob body
            g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId).darker(0.4f));
            g.fillEllipse(juce::Rectangle<float>(arcRadius * 1.4f, arcRadius * 1.4f).withCentre({ centreX, centreY }));

            // Background track arc
            juce::Path backgroundArc;
            backgroundArc.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f,
                                        rotaryStartAngle, rotaryEndAngle, true);
            g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
            g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Value arc
            if (slider.isEnabled())
            {
                juce::Path valueArc;
                valueArc.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f,
                                       rotaryStartAngle, toAngle, true);
                g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
                g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            }

            // Pointer thumb
            auto thumbWidth = lineW * 1.6f;
            juce::Point<float> thumbPoint(centreX + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                          centreY + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
        }

        void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                  const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override
        {
            auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
            auto corner = 8.0f;

            auto baseColour = backgroundColour;
            if (shouldDrawButtonAsDown)
                baseColour = baseColour.darker(0.2f);
            else if (shouldDrawButtonAsHighlighted)
                baseColour = baseColour.brighter(0.15f);

            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, corner);

            g.setColour(juce::Colour(0xff45475a));
            g.drawRoundedRectangle(bounds, corner, 1.0f);
        }
};
