//
// Created by Landon Viator on 11/1/25.
//

#pragma once

namespace viator::globals
{
    struct Oversampling
    {
        static inline const juce::StringArray items = {"Off", "2X", "4X", "8X", "16X" };
    };

    enum class MacroLearnState
    {
        kOff,
        kOn
    };

    struct ActionCommands
    {
        static inline const juce::String macroToggled { "macroToggled" };
        static inline const juce::String editorAdded { "editorAdded" };
        static inline const juce::String modulesLoaded { "Loaded" };
        static inline const juce::String processorAdded { "processorAdded" };
        static inline const juce::String editorDeleted { "editorDeleted" };
    };

    struct WidgetProperties
    {
        static inline const juce::String macroKey { "macroKey" };
    };
}
