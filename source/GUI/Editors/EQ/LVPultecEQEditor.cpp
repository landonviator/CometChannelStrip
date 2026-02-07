//
// Created by Landon Viator on 11/14/25.
//

#include "LVPultecEQEditor.h"

namespace viator::gui::editors
{
    LVPultecEQEditor::LVPultecEQEditor(viator::dsp::processors::LVPultecEQProcessor &p)
        : viator::gui::editors::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);
        const auto id = juce::String(processorRef.getProcessorID());

        const juce::StringArray ids = {
            LVPultecEQParameters::lowBoostID, LVPultecEQParameters::lowAttenID, LVPultecEQParameters::highBoostID, LVPultecEQParameters::highAttenID,
            LVPultecEQParameters::lowFreqID, LVPultecEQParameters::bandwidthID, LVPultecEQParameters::highFreqID, LVPultecEQParameters::highAttenSelID
        };

        const juce::StringArray names = {
            LVPultecEQParameters::lowBoostName, LVPultecEQParameters::lowAttenName, LVPultecEQParameters::highBoostName,
            LVPultecEQParameters::highAttenName,
            LVPultecEQParameters::lowFreqName, LVPultecEQParameters::bandwidthName, LVPultecEQParameters::highFreqName,
            LVPultecEQParameters::highAttenSelName
        };

        for (int i = 0; i < num_sliders; ++i)
        {
            setSliderProps(m_main_sliders[i]);
            getSliders().push_back(&m_main_sliders[i]);
            m_main_sliders[i].setComponentID(ids[i]);
            m_main_sliders[i].setName(names[i]);

            main_slider_attaches.emplace_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getTreeState(), ids[i] + id, m_main_sliders[i]));
        }

        for (auto &label: m_main_labels)
        {
            setLabelProps(label);
        }

        updateLabels();

        setSize(1000, 600);
    }

    LVPultecEQEditor::~LVPultecEQEditor()
    {
        for (auto &slider: m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }
    }

    //==============================================================================
    void LVPultecEQEditor::paint(juce::Graphics &g)
    {
        BaseEditor::paint(g);
    }

    void LVPultecEQEditor::resized()
    {
        const auto width = juce::roundToInt(getWidth() * 0.34);
        auto x = juce::roundToInt(getWidth() * 0.05);
        auto y = getHeight() - juce::roundToInt(width * 1.5);

        for (int i = 0; i < 4; ++i)
        {
            m_main_sliders[i].setBounds(x, y, width, width);
            y -= width;
        }

        y = getHeight() - juce::roundToInt(width * 1.5);
        x = getWidth() - x - width;
        for (int i = 4; i < 8; ++i)
        {
            m_main_sliders[i].setBounds(x, y, width, width);
            y -= width;
        }

        BaseEditor::resized();
    }

    void LVPultecEQEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
        slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(211, 218, 217));
        slider.setColour(juce::Slider::ColourIds::backgroundColourId, gui_utils::Colors::dial_bg());
        slider.setLookAndFeel(&m_dial_laf);

        addAndMakeVisible(slider);
    }

    void LVPultecEQEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }

    void LVPultecEQEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
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

    void LVPultecEQEditor::updateLabels()
    {
        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            const auto value = m_main_sliders[i].getValue();
            const auto text_value = value >= 1000.0 ? juce::String(value / 1000.0f, 2) + " kHz" : juce::String(value, 2);
            const auto text = m_main_sliders[i].isMouseOverOrDragging() ? text_value : m_main_sliders[i].getName();
            m_main_labels[i].setText(text, juce::dontSendNotification);
        }
    }
}
