//
// Created by Landon Viator on 11/14/25.
//

#include "LV50AParametricEQEditor.h"

namespace viator::gui::editors
{
    LV50AParametricEQEditor::LV50AParametricEQEditor(viator::dsp::processors::LV50AParametricEQProcessor &p)
        : viator::gui::editors::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);
        const auto id = juce::String(processorRef.getProcessorID());

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            setSliderProps(m_main_sliders[i]);
            getSliders().push_back(&m_main_sliders[i]);
        }

        for (auto &label: m_main_labels)
        {
            setLabelProps(label);
        }

        for (int i = 0; i < LV50AParametricEQParameters::numBands; ++i)
        {
            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::gainIDs[i] + id, m_main_sliders[i]));

            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::qIDs[i] + id, m_main_sliders[i + 4]));

            main_slider_attaches.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::cutoffIDs[i] + id, m_main_sliders[i + 8]));

            m_main_sliders[i].setName("Gain " + juce::String(i+1));
            m_main_sliders[i + 4].setName("Q " + juce::String(i+1));
            m_main_sliders[i + 8].setName("Cutoff " + juce::String(i+1));

            m_main_sliders[i].setComponentID(LV50AParametricEQParameters::gainIDs[i] + id);
            m_main_sliders[i + 4].setComponentID(LV50AParametricEQParameters::qIDs[i] + id);
            m_main_sliders[i + 8].setComponentID(LV50AParametricEQParameters::cutoffIDs[i] + id);

            const auto red = juce::Colour(191, 9, 47);
            m_main_sliders[i].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, red);
            m_main_sliders[i].setColour(juce::Slider::ColourIds::thumbColourId, red.brighter(0.5f));

            const auto green = juce::Colour(0, 106, 103);
            m_main_sliders[i + 4].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, green);
            m_main_sliders[i + 4].setColour(juce::Slider::ColourIds::thumbColourId, green.brighter(0.5f));

            const auto blue  = juce::Colour(69, 104, 130);
            m_main_sliders[i + 8].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, blue);
            m_main_sliders[i + 8].setColour(juce::Slider::ColourIds::thumbColourId, blue.brighter(0.5f));
        }

        m_hp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::hpCutoffID + id, m_main_sliders[kHP]);
        m_lp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::lpCutoffID + id, m_main_sliders[kLP]);
        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
                (processorRef.getTreeState(), LV50AParametricEQParameters::driveID + id, m_main_sliders[kDrive]);

        for (int i = 0; i < LV50AParametricEQParameters::gainIDs.size(); ++i)
        {
            m_main_sliders[i].setComponentID(LV50AParametricEQParameters::gainIDs[i] + id);
        }
        m_main_sliders[kHP].setComponentID(LV50AParametricEQParameters::hpCutoffID + id);
        m_main_sliders[kHP].setName("HP");
        m_main_sliders[kHP].setComponentID(LV50AParametricEQParameters::hpCutoffID + id);

        m_main_sliders[kLP].setComponentID(LV50AParametricEQParameters::lpCutoffID + id);
        m_main_sliders[kLP].setName("LP");
        m_main_sliders[kLP].setComponentID(LV50AParametricEQParameters::lpCutoffID + id);

        m_main_sliders[kDrive].setComponentID(LV50AParametricEQParameters::driveID + id);
        m_main_sliders[kDrive].setName("Drive");
        m_main_sliders[kDrive].setComponentID(LV50AParametricEQParameters::driveID + id);


        updateLabels();

        setSize(1000, 600);
    }

    LV50AParametricEQEditor::~LV50AParametricEQEditor()
    {
        for (auto &slider: m_main_sliders)
        {
            slider.setLookAndFeel(nullptr);
        }
    }

    //==============================================================================
    void LV50AParametricEQEditor::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colour(0, 0, 0)); //.withAlpha(0.85f));
        BaseEditor::paint(g);

        updateLabels();
    }

    void LV50AParametricEQEditor::resized()
    {
        auto x = juce::roundToInt(getWidth() * 0.05);
        auto y = juce::roundToInt(getHeight() * 0.12);
        auto width = getWidth() / 5;
        const auto padding_y = juce::roundToInt(width * 0.65);
        const auto padding_x = juce::roundToInt(width * 0.16);
        const auto height = getHeight() / (num_sliders + 2);
        const auto font_size = static_cast<float>(getWidth()) * 0.035f;

        for (int i = 0; i < 4; ++i)
        {
            y = juce::roundToInt(getHeight() * 0.12);
            m_main_sliders[i].setBounds(x, y, width, width);
            y += width + padding_y;
            m_main_sliders[i + 8].setBounds(x, y, width, width);
            y += width + padding_y;
            m_main_sliders[i + 4].setBounds(x, y, width, width);
            x += width + padding_x;
        }

        width = getWidth() / 5;
        y = juce::roundToInt(getHeight() * 0.8);
        x = juce::roundToInt(getWidth() * 0.198);
        const auto label_height = height / 2;
        const auto filter_size = juce::roundToInt(width * 0.85);
        m_main_sliders[kHP].setBounds(x, y, filter_size, filter_size);
        x += width;
        m_main_sliders[kDrive].setBounds(x, y, width, width);
        x += width + (width - filter_size);
        m_main_sliders[kLP].setBounds(x, y, filter_size, filter_size);

        for (int i = 0; i < m_main_sliders.size(); ++i)
        {
            m_main_labels[i].setBounds(m_main_sliders[i].getX(), m_main_sliders[i].getBottom(), width, label_height);
            m_main_labels[i].setFont(viator::gui_utils::Fonts::regular(font_size));
        }

        BaseEditor::resized();
    }

    void LV50AParametricEQEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(215, 215, 215).withAlpha(0.85f));
        slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(5, 120, 190));
        slider.setLookAndFeel(&m_dial_laf);

        addAndMakeVisible(slider);
    }

    void LV50AParametricEQEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }

    void LV50AParametricEQEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
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

    void LV50AParametricEQEditor::updateLabels()
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
