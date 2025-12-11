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

        juce::Font getComboBoxFont(juce::ComboBox &box) override
        {
            const auto font_size = static_cast<float>(box.getHeight()) * 0.5f;
            return viator::gui_utils::Fonts::regular(font_size);
        }
    };
}
