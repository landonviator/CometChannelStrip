//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Fonts.h"

namespace viator::gui::laf
{
    class MenuLAF final : public juce::LookAndFeel_V4
    {
    public:

        void drawComboBox (juce::Graphics& g, const int width, const int height, bool,
                                   int, int, int, int, juce::ComboBox& box) override
        {
            const auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
            const juce::Rectangle<int> boxBounds (0, 0, width, height);

            auto baseColour = box.findColour (juce::ComboBox::backgroundColourId).withMultipliedSaturation(2.0f).withMultipliedAlpha(1.0f);

            if (box.isMouseOverOrDragging())
                baseColour = gui_utils::Colors::light_bg().withAlpha(0.5f);

            g.setColour (baseColour);
            g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

            g.setColour (box.findColour (juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

            const juce::Rectangle<int> arrowZone (width - 30, 0, 20, height);
            juce::Path path;
            path.startNewSubPath (static_cast<float>(arrowZone.getX()) + 3.0f, static_cast<float>(arrowZone.getCentreY()) - 2.0f);
            path.lineTo (static_cast<float>(arrowZone.getCentreX()), static_cast<float>(arrowZone.getCentreY()) + 3.0f);
            path.lineTo (static_cast<float>(arrowZone.getRight()) - 3.0f, static_cast<float>(arrowZone.getCentreY()) - 2.0f);

            g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
            g.strokePath (path, juce::PathStrokeType (2.0f));

            g.setFont(getComboBoxFont(box));
            const auto text = box.getItemText(box.getSelectedItemIndex());
            g.drawFittedText(text, 0, 0, width, height, juce::Justification::centredLeft, 1);
        }

        juce::Font getComboBoxFont(juce::ComboBox &box) override
        {
            const auto font_size = static_cast<float>(box.getHeight()) * 0.5f;
            return viator::gui_utils::Fonts::regular(font_size);
        }

        void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
        {
        }
    };
}
