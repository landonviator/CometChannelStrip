#include "PluginProcessor.h"
#include "PluginEditor.h"

//test

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    const auto items = viator::globals::Oversampling::items;
    setComboBoxProps(m_oversampling_menu, items);
    m_oversampling_Attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getTreeState(),
                                                                                                     viator::parameters::oversamplingChoiceID,
                                                                                                     m_oversampling_menu);

    setSize (1000, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(34, 40, 49));
}

void AudioPluginAudioProcessorEditor::resized()
{
    const auto padding = juce::roundToInt(getWidth() * 0.03);
    const auto width = juce::roundToInt(getWidth() * 0.1);
    const auto height = juce::roundToInt(getHeight() * 0.05);
    const auto x = getWidth() - width - padding;
    const auto y = padding;
    m_oversampling_menu.setBounds(x, y, width, height);
}

void AudioPluginAudioProcessorEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
{
    box.addItemList(items, 1);
    box.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(box);
}
