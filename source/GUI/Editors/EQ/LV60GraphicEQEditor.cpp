//
// Created by Landon Viator on 11/14/25.
//

#include "LV60GraphicEQEditor.h"

namespace viator::gui::editors
{
    LV60GraphicEQEditor::LV60GraphicEQEditor(viator::dsp::processors::LV60GraphicEQProcessor &p)
            : viator::gui::editors::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        for (auto& slider : m_main_sliders)
        {
            setSliderProps(slider);
        }

        for (int i = 0; i < LV60GraphicEQParameters::numBands; ++i)
        {
            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::gainIDs[i] + juce::String(processorRef.getProcessorID()), m_main_sliders[i]));
        }

        m_hp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::hpCutoffID + juce::String(processorRef.getProcessorID()), m_main_sliders[kHP]);
        m_lp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::lpCutoffID + juce::String(processorRef.getProcessorID()), m_main_sliders[kLP]);
        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                        (processorRef.getTreeState(), LV60GraphicEQParameters::driveID + juce::String(processorRef.getProcessorID()), m_main_sliders[kDrive]);

        setSize(1000, 600);
    }

    LV60GraphicEQEditor::~LV60GraphicEQEditor()
    {
    }

//==============================================================================
    void LV60GraphicEQEditor::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::black.brighter(0.15f));
        BaseEditor::paint(g);
    }

    void LV60GraphicEQEditor::resized()
    {
        auto x = juce::roundToInt(getWidth() * 0.2);
        auto y = juce::roundToInt(getHeight() * 0.12);
        auto width = juce::roundToInt(getWidth() * 0.6);
        const auto height = getHeight() / (num_sliders + 2);

        for (auto& slider : m_main_sliders)
        {
            slider.setBounds(x, y, width, height);
            y += height;
        }

        width = getWidth() / 5;
        y = juce::roundToInt(getHeight() * 0.76);
        m_main_sliders[kHP].setBounds(x, y, width, width);
        x += width;
        m_main_sliders[kDrive].setBounds(x, y, width, width);
        x += width;
        m_main_sliders[kLP].setBounds(x, y, width, width);

        BaseEditor::resized();
    }

    void LV60GraphicEQEditor::setSliderProps(juce::Slider &slider)
    {
        if (&slider != &m_main_sliders[kDrive] &&
            &slider != &m_main_sliders[kHP] &&
            &slider != &m_main_sliders[kLP])
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        } else
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        }

        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);
    }

    void LV60GraphicEQEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setSelectedId(1, juce::dontSendNotification);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, viator::gui_utils::Colors::editor_minor_bg_color());
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
                                       viator::gui_utils::Colors::editor_minor_bg_color());
        addAndMakeVisible(box);
    }
}