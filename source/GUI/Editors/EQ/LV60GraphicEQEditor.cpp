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
        const auto id = juce::String(processorRef.getProcessorID());

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            setSliderProps(m_main_sliders[i]);
            getSliders().push_back(&m_main_sliders[i]);

            m_main_sliders[i].onValueChange = [this, i]()
            {
                const auto value = m_main_sliders[i].getValue();
                const auto text = value >= 1000.0 ? juce::String(value / 1000.0f, 2) + " kHz" : juce::String(value, 2);
                m_main_labels[i].setText(text, juce::dontSendNotification);
            };
        }

        m_main_sliders[kGain5].setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(220, 60, 40));

        for (auto& label : m_main_labels)
        {
            setLabelProps(label);
        }

        for (int i = 0; i < m_gain_labels.size(); i++)
        {
            setLabelProps(m_gain_labels[i]);
            m_gain_labels[i].setText(gain_labels[i], juce::dontSendNotification);
        }

        for (int i = 0; i < LV60GraphicEQParameters::numBands; ++i)
        {
            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::gainIDs[i] + id, m_main_sliders[i]));
        }

        m_hp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::hpCutoffID + id, m_main_sliders[kHP]);
        m_lp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV60GraphicEQParameters::lpCutoffID + id, m_main_sliders[kLP]);
        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                        (processorRef.getTreeState(), LV60GraphicEQParameters::driveID + id, m_main_sliders[kDrive]);

        for (int i = 0; i < LV60GraphicEQParameters::gainIDs.size(); ++i)
        {
            m_main_sliders[i].setComponentID(LV60GraphicEQParameters::gainIDs[i] + id);
        }
        m_main_sliders[kHP].setComponentID(LV60GraphicEQParameters::hpCutoffID + id);
        m_main_sliders[kLP].setComponentID(LV60GraphicEQParameters::lpCutoffID + id);
        m_main_sliders[kDrive].setComponentID(LV60GraphicEQParameters::driveID + id);

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            const auto value = m_main_sliders[i].getValue();
            const auto text = value >= 1000.0 ? juce::String(value / 1000.0f, 2) + " kHz" : juce::String(value, 2);
            m_main_labels[i].setText(text, juce::dontSendNotification);
        }

        setSize(1000, 600);
    }

    LV60GraphicEQEditor::~LV60GraphicEQEditor()
    {
        for (auto& slider : m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }
    }

//==============================================================================
    void LV60GraphicEQEditor::paint(juce::Graphics &g)
    {
        setBackgroundColor(juce::Colour(41, 43, 45));
        BaseEditor::paint(g);
    }

    void LV60GraphicEQEditor::resized()
    {
        auto x = juce::roundToInt(getWidth() * 0.2);
        auto y = juce::roundToInt(getHeight() * 0.12);
        auto width = juce::roundToInt(getWidth() * 0.6);
        const auto height = getHeight() / (num_sliders + 2);
        auto font_size = static_cast<float>(getWidth()) * 0.05f;

        for (auto& slider : m_main_sliders)
        {
            slider.setBounds(x, y, width, height);
            y += height;
        }

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            const auto _x = m_main_sliders[i].getRight();
            const auto _y = m_main_sliders[i].getY();
            const auto _width = getWidth() - m_main_sliders[i].getRight();
            m_main_labels[i].setBounds(_x, _y, _width, height);
            m_main_labels[i].setFont(viator::gui_utils::Fonts::regular(font_size));

            if (i > m_gain_labels.size() - 1)
                continue;

            m_gain_labels[i].setBounds(0, _y, _width, height);
            m_gain_labels[i].setFont(viator::gui_utils::Fonts::regular(font_size));
        }

        width = getWidth() / 5;
        y = juce::roundToInt(getHeight() * 0.8);
        x = juce::roundToInt(getWidth() * 0.198);
        const auto label_height = height / 2;
        const auto filter_size = juce::roundToInt(width * 0.85);
        font_size = static_cast<float>(getWidth()) * 0.035f;
        m_main_sliders[kHP].setBounds(x, y, filter_size, filter_size);
        m_main_labels[kHP].setBounds(x, m_main_sliders[kHP].getBottom(), width, label_height);
        m_main_labels[kHP].setFont(viator::gui_utils::Fonts::regular(font_size));
        x += width;
        m_main_sliders[kDrive].setBounds(x, y, width, width);
        m_main_labels[kDrive].setBounds(x, m_main_sliders[kDrive].getBottom(), width, label_height);
        m_main_labels[kDrive].setFont(viator::gui_utils::Fonts::regular(font_size));
        x += width + (width - filter_size);
        m_main_sliders[kLP].setBounds(x, y, filter_size, filter_size);
        m_main_labels[kLP].setBounds(x, m_main_sliders[kLP].getBottom(), width, label_height);
        m_main_labels[kLP].setFont(viator::gui_utils::Fonts::regular(font_size));

        BaseEditor::resized();
    }

    void LV60GraphicEQEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        if (&slider != &m_main_sliders[kDrive] &&
            &slider != &m_main_sliders[kHP] &&
            &slider != &m_main_sliders[kLP])
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
            slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
            slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::black);
            slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(240, 220, 200));
            slider.setLookAndFeel(&m_slider_laf);
        } else
        {
            slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
            slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
            slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(5, 120, 190));
            slider.setLookAndFeel(&m_dial_laf);
        }

        addAndMakeVisible(slider);
    }

    void LV60GraphicEQEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
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