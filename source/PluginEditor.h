#pragma once

#include "PluginProcessor.h"
#include "GUI/Editors/BaseEditor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::ComboBox m_oversampling_menu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_oversampling_Attach;
    void setComboBoxProps(juce::ComboBox& box, const juce::StringArray& items);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
