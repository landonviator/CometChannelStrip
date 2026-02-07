//
// Created by Landon Viator on 11/26/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace viator::laf
{
    class ButtonLAF : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonBackground(juce::Graphics &g,
                                  juce::Button &button,
                                  const juce::Colour &backgroundColour,
                                  const bool shouldDrawButtonAsHighlighted,
                                  const bool shouldDrawButtonAsDown) override
        {
            constexpr auto cornerSize = 3.0f;
            const auto bounds = button.getLocalBounds().toFloat();

            auto baseColour = backgroundColour.withMultipliedSaturation(2.0f).withMultipliedAlpha(1.0f);

            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = button.findColour(juce::TextButton::ColourIds::buttonOnColourId);

            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, cornerSize);

            g.setColour(button.findColour(juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }

        void drawButtonText(juce::Graphics &g,
                            juce::TextButton &button,
                            bool /*isMouseOverButton*/,
                            bool /*isButtonDown*/) override
        {
            const auto font = getTextButtonFont(button, button.getHeight());
            g.setFont(font);

            auto textCol = button.findColour(button.getToggleState()
                                                 ? juce::TextButton::textColourOnId
                                                 : juce::TextButton::textColourOffId);

            if (!button.isEnabled())
                textCol = textCol.withMultipliedAlpha(0.5f);

            auto r = button.getLocalBounds();

            r = r.translated(0, -1);

            g.setColour(textCol);
            g.drawFittedText(button.getButtonText(), r, juce::Justification::centred, 1);
        }

        juce::Font getTextButtonFont(juce::TextButton & /*button*/, const int buttonHeight) override
        {
            return {juce::FontOptions(static_cast<float>(buttonHeight) * 0.45f, juce::Font::bold)};
        }
    };
}
