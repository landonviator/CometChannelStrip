//
// Created by Landon Viator on 11/17/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Editors/BaseEditor.h"
#include "../../DSP/Processors/ProcessorUtils.h"

class AudioPluginAudioProcessor;

namespace viator::gui::views
{
    class EditorRack
            : public juce::Component, public juce::ActionBroadcaster, public juce::ActionListener
    {
    public:
        EditorRack(AudioPluginAudioProcessor &);

        ~EditorRack() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void rebuild_editors();

        std::vector<std::unique_ptr<juce::AudioProcessorEditor>> &getEditors()
        { return m_editors; }

        void setParentWidth(const int newWidth) { parent_width = newWidth; resized(); }


    private:
        AudioPluginAudioProcessor &processorRef;
        std::vector<std::unique_ptr<juce::AudioProcessorEditor>> m_editors;

        void addEditor();

        juce::ComboBox m_plugin_selector;
        const int processorIdOffset = 1000;
        void buildPopupMenu();

        void mouseDown(const juce::MouseEvent &e) override;

        void mouseDrag(const juce::MouseEvent &e) override;

        void mouseUp(const juce::MouseEvent &e) override;

        juce::AudioProcessorEditor *dragging_editor = nullptr;
        juce::Point<int> drag_offset_from_top_left;
        int drag_original_index = -1;

        int parent_width { 1 };

        void actionListenerCallback(const juce::String &message) override;

        void remove_editor_at_index(const int index);
    };
}