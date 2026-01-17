//
// Created by Landon Viator on 11/14/25.
//

#include "ClipperEditor.h"

namespace viator::gui::editors
{
    ClipperEditor::ClipperEditor(viator::dsp::processors::ClipperProcessor &p)
            : viator::gui::editors::BaseEditor(p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        m_drive_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        m_drive_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 32, 64);
        m_drive_slider.addMouseListener(this, true);
        m_drive_slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId,
                                 juce::Colours::transparentBlack);
        m_drive_slider.setComponentID(
                ClipperParameters::driveID + juce::String(processorRef.getProcessorID()));
        getSliders().push_back(&m_drive_slider);
        m_drive_slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::whitesmoke);
        m_drive_slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(190, 49, 68));
        m_drive_slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
        m_drive_slider.setLookAndFeel(&m_dial_laf);
        addAndMakeVisible(m_drive_slider);

        m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processorRef
                        .getTreeState(),
                ClipperParameters::driveID +
                juce::String(
                        processorRef.getProcessorID()),
                m_drive_slider);

        // clip type menu
        juce::StringArray items = {"Soft", "Hard"};
        setComboBoxProps(m_clipper_type_menu, items);
        m_clipper_type_attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                processorRef
                        .getTreeState(),
                ClipperParameters::clipTypeID +
                juce::String(
                        processorRef.getProcessorID()),
                m_clipper_type_menu);

        setSize(1000, 600);
    }

    ClipperEditor::~ClipperEditor()
    {
        m_drive_slider.setLookAndFeel(nullptr);
        m_clipper_type_menu.setLookAndFeel(nullptr);
    }

//==============================================================================
    void ClipperEditor::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::black.brighter(0.15f));
        BaseEditor::paint(g);
    }

    void ClipperEditor::resized()
    {
        m_drive_slider.setBounds(
                getLocalBounds().withSizeKeepingCentre(getWidth() / 1, getWidth() / 1));
        m_drive_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                                       m_drive_slider.getWidth() / 2,
                                       m_drive_slider.getHeight() / 10);

        const auto box_width = m_drive_slider.getWidth() / 4;
        const auto box_x = getWidth() / 2 - box_width / 2;
        m_clipper_type_menu.setBounds(box_x,
                                      m_drive_slider.getBottom(),
                                      box_width,
                                      m_drive_slider.getHeight() / 10);
        BaseEditor::resized();
    }

    void ClipperEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
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