#pragma once

#include "PluginProcessor.h"
#include "GUI/Editors/BaseEditor.h"
#include "GUI/Views/EditorRack.h"
#include "GUI/Widgets/MacroSlider.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final
        : public juce::AudioProcessorEditor, public juce::ChangeListener, public juce::ActionListener
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);

    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor &processorRef;

    juce::ComboBox m_oversampling_menu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_oversampling_Attach;

    void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

    viator::gui::views::EditorRack m_rack;


    std::array<viator::gui::widgets::MacroSlider, 10> m_macro_knobs;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> m_macro_attaches;

    void initMacroKnobs();

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void actionListenerCallback(const juce::String &message) override;

    void mouseDown(const juce::MouseEvent &event);

    viator::gui::widgets::MacroSlider *m_macro_slider{nullptr};

    void refreshMacroMappings();

    juce::Viewport m_view_port;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
