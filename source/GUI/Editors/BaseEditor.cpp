//
// Created by Landon Viator on 11/14/25.
//

#include "BaseEditor.h"

namespace viator::gui::editors
{
    BaseEditor::BaseEditor(viator::dsp::processors::BaseProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        // SLIDERS
        for (auto &slider: m_io_sliders)
        {
            setSliderProps(slider);
        }

        m_io_sliders[kInput].setName("In");
        m_io_sliders[kOutput].setName("Out");

        // LABELS
        for (auto &label: m_io_labels)
        {
            setLabelProps(label);
        }

        // MENUS
        juce::StringArray items = {"Presets"};
        setComboBoxProps(m_preset_browser, items);

        items.clear();
        items = {"Off", "X2", "X4", "X8", "X16"};
        setComboBoxProps(m_oversampling_menu, items);
        m_oversampling_menu_attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processorRef
            .getTreeState(),
            "oversamplingChoiceID" +
            juce::String(
                processorRef.getProcessorID()),
            m_oversampling_menu);

        // BUTTONS
        setButtonProps(m_buttons[kMute], "M");
        setButtonProps(m_buttons[kSolo], "S");
        setButtonProps(m_buttons[kDelete], "X");
        m_buttons[kMute].setClickingTogglesState(true);
        m_buttons[kSolo].setClickingTogglesState(true);
        m_buttons[kDelete].setClickingTogglesState(false);

        m_buttons[kDelete].onClick = [this]()
        {
            const auto ptrValue = reinterpret_cast<std::uintptr_t>(this);
            sendActionMessage(
                viator::globals::ActionCommands::editorDeleted + juce::String::toHexString(static_cast<juce::int64>(ptrValue)));
        };

        m_mute_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef
            .getTreeState(), "muteID" +
                             juce::String(
                                 processorRef.getProcessorID()),
            m_buttons[kMute]);

        setSize(1000, 600);
    }

    BaseEditor::~BaseEditor()
    {
        m_io_sliders[kInput].setLookAndFeel(nullptr);
        m_io_sliders[kOutput].setLookAndFeel(nullptr);
        m_preset_browser.setLookAndFeel(nullptr);
        m_oversampling_menu.setLookAndFeel(nullptr);
    }

    //==============================================================================
    void BaseEditor::paint(juce::Graphics &g)
    {
        drawHeaderAndFooter(g);
        showLabelHover();
    }

    void BaseEditor::resized()
    {
        // SLIDERS
        const auto font_size = static_cast<float>(getWidth()) * 0.04f;

        auto width = juce::roundToInt(getHeight() * 0.09);
        auto height = width;
        auto x = 0;
        auto y = juce::roundToInt(getHeight() * 0.905);
        m_io_sliders[kInput].setBounds(x, y, width, height);
        m_io_sliders[kInput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        m_io_labels[kInput].setBounds(m_io_sliders[kInput].getRight(), y, width, height);
        m_io_labels[kInput].setFont(viator::gui_utils::Fonts::regular(font_size));

        // LABELS
        x = getWidth() - width;
        m_io_sliders[kOutput].setBounds(x, y, width, height);
        m_io_sliders[kOutput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        m_io_labels[kOutput].setBounds(m_io_sliders[kOutput].getX() - width, y, width, height);
        m_io_labels[kOutput].setFont(viator::gui_utils::Fonts::regular(font_size));

        // MENUS
        const auto padding = 2;
        const auto available = getWidth() - 12;
        width = juce::roundToInt(available * 0.4);
        height = juce::roundToInt(getHeight() * 0.05);
        x = padding;
        y = juce::roundToInt(getHeight() * 0.025);
        m_preset_browser.setBounds(x, y, width, height);
        x += width + padding;
        width = juce::roundToInt(available * 0.25);
        m_oversampling_menu.setBounds(x, y, width, height);

        width = juce::roundToInt(available * 0.116);
        x = m_oversampling_menu.getRight() + padding;
        for (auto &button: m_buttons)
        {
            button.setBounds(x, y, width, height);
            x += width + padding;
        }
    }

    void BaseEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::whitesmoke);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::dimgrey);
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
        slider.setLookAndFeel(&m_dial_laf);
        addAndMakeVisible(slider);
    }

    void BaseEditor::setLabelProps(juce::Label &label)
    {
        label.setJustificationType(juce::Justification::centred);
        label.setText("Label", juce::dontSendNotification);
        addAndMakeVisible(label);
    }

    void BaseEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, viator::gui_utils::Colors::editor_minor_bg_color());
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
                                       viator::gui_utils::Colors::editor_minor_bg_color());
        addAndMakeVisible(box);
        box.setSelectedId(1);
    }

    void BaseEditor::setButtonProps(juce::TextButton &button, const juce::String &name)
    {
        button.setButtonText(name);
        button.setColour(juce::ComboBox::ColourIds::outlineColourId,
                         juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::ColourIds::buttonColourId,
                         viator::gui_utils::Colors::editor_minor_bg_color());
        button.setColour(juce::TextButton::ColourIds::buttonOnColourId,
                         viator::gui_utils::Colors::editor_minor_bg_color());
        addAndMakeVisible(button);
    }

    void BaseEditor::showLabelHover()
    {
        std::for_each(m_io_sliders.begin(), m_io_sliders.end(),
                      [this, idx = 0](auto &slider) mutable
                      {
                          auto &label = m_io_labels[idx++];

                          const auto isOver = slider.isMouseOverOrDragging();
                          const auto value = slider.getValue();
                          const auto name = slider.getName();

                          label.setText(isOver ? juce::String(value, 2) : name,
                                        juce::dontSendNotification);
                      });
    }

    void BaseEditor::drawHeaderAndFooter(juce::Graphics &g)
    {
        g.setColour(viator::gui_utils::Colors::editor_bg_color());
        g.fillRect(getLocalBounds().withHeight(getHeight() / 10));
        g.fillRect(getLocalBounds().withHeight(getHeight() / 10).withY(
            juce::roundToInt(getHeight() * 0.9)));
    }
}
