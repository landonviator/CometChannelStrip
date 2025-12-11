//
// Created by Landon Viator on 11/24/25.
//

#pragma once

namespace viator::gui_utils
{
    class Colors final
    {
    public:

        static juce::Colour editor_bg_color()      { return juce::Colours::black.brighter(0.15f); }
        static juce::Colour editor_minor_bg_color()      { return juce::Colours::black.brighter(0.25f); }
        static juce::Colour text()      { return {249, 250, 251}; }
        static juce::Colour meter_green()      { return {59, 151, 151}; }
        static juce::Colour meter_yellow()      { return {255, 204, 0}; }
        static juce::Colour meter_red()      { return {247, 55, 79}; }
    };
}
