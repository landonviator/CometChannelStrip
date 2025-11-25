//
// Created by Landon Viator on 7/17/25.
//

#pragma once

#include <juce_core/juce_core.h>
#include "../GUI/Widgets/MacroSlider.h"

namespace viator::engine
{
    class MacroMap
    {
    public:
        MacroMap() = default;

        void update(const juce::AudioProcessorValueTreeState& treeState, const juce::String& parameterID, float newValue)
        {
            const auto found = m_macro_map.find(parameterID);
            if (found == m_macro_map.end())
                return;

            for (const auto& targetID : found->second)
            {
                if (auto* param = treeState.getParameter(targetID))
                    param->setValueNotifyingHost(newValue);
            }
        }

        void addMacroAssignment(const juce::String& paramID)
        {
            const bool isInvalid = m_current_macro_key.isEmpty() || paramID.isEmpty() || m_current_macro_key == paramID;
            const bool isLearningDisabled = (m_macro_learn_state != MacroState::kOn);

            if (isInvalid || isLearningDisabled)
                return;

            m_macro_map[m_current_macro_key].insert(paramID);
        }

        void removeMacroAssignment(const juce::String& paramID)
        {
            if (m_current_macro_key.isEmpty())
                return;

            const auto it = m_macro_map.find(m_current_macro_key);
            if (it == m_macro_map.end())
                return;

            auto& targets = it->second;
            targets.erase(paramID);

            if (targets.empty())
                m_macro_map.erase(it);
        }

        void setMacroLearnState(const viator::globals::MacroLearnState newState)
        {
            m_macro_learn_state = newState;
        }

        void macroStateChanged(const juce::String& macro_id)
        {
            m_current_macro_key = macro_id;
        }

        [[nodiscard]] std::unordered_set<juce::String> getAssignmentsForCurrentMacro() const
        {
            if (m_current_macro_key.isEmpty())
                return {};

            const auto it = m_macro_map.find(m_current_macro_key);
            return it != m_macro_map.end() ? it->second : std::unordered_set<juce::String>{};
        }

        [[nodiscard]] juce::ValueTree saveMacroState() const
        {
            juce::ValueTree macroTree("Macros");

            for (const auto& [macroID, paramIDs] : m_macro_map)
            {
                juce::ValueTree macroNode("Macro");
                macroNode.setProperty("id", macroID, nullptr);

                for (const auto& paramID : paramIDs)
                {
                    juce::ValueTree paramNode("Param");
                    paramNode.setProperty("id", paramID, nullptr);
                    macroNode.addChild(paramNode, -1, nullptr);
                }

                macroTree.addChild(macroNode, -1, nullptr);
            }

            return macroTree;
        }

        void loadMacroState(const juce::ValueTree& macroTree)
        {
            m_macro_map.clear();

            for (const auto& macroNode : macroTree)
            {
                if (macroNode.hasType("Macro"))
                {
                    const auto macroID = macroNode["id"].toString();
                    std::unordered_set<juce::String> paramIDs;

                    for (const auto& paramNode : macroNode)
                    {
                        if (paramNode.hasType("Param"))
                            paramIDs.insert(paramNode["id"].toString());
                    }

                    m_macro_map[macroID] = std::move(paramIDs);
                }
            }
        }

        juce::String& getCurrentMacro() { return m_current_macro_key; }

        juce::String getMacroForSlider(const juce::String& sliderID) const
        {
            for (const auto& entry : m_macro_map)
            {
                const auto& macroID  = entry.first;
                const auto& sliders  = entry.second; // unordered_set<String>

                if (sliders.find(sliderID) != sliders.end())
                    return macroID; // found
            }

            return {}; // not mapped
        }

    private:
        using MacroState = viator::globals::MacroLearnState;
        std::unordered_map<juce::String, std::unordered_set<juce::String>> m_macro_map;
        MacroState m_macro_learn_state { MacroState::kOff };
        juce::String m_current_macro_key { "" };
    };
}
