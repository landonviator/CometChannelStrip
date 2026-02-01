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
        const auto bounds = getLocalBounds();
        const auto main_colour = m_comp_bg;
        constexpr auto contrast = 0.1;
        const auto center_x = static_cast<float>(bounds.getCentreX());
        const auto center_y = static_cast<float>(bounds.getCentreY());
        const auto bottom = static_cast<float>(bounds.getBottom());

        const juce::ColourGradient gradient(
            main_colour.brighter(contrast),center_x, center_y, main_colour.darker(contrast), center_x, bottom, true
        );

        g.setGradientFill(gradient);
        g.fillRect(bounds);

        g.setColour(juce::Colour(73, 73, 73));
        g.drawRect(0, 0, getWidth(), getHeight(), 2);
    }

    void BaseEditor::resized()
    {
        // SLIDERS
        const auto font_size = static_cast<float>(getWidth()) * 0.04f;

        auto width = juce::roundToInt(getHeight() * 0.1);
        auto height = width;
        auto x = 0;
        auto y = getHeight() - height;
        m_io_sliders[kInput].setBounds(x, y, width, height);
        m_io_sliders[kInput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        // LABELS
        x = getWidth() - width;
        m_io_sliders[kOutput].setBounds(x, y, width, height);
        m_io_sliders[kOutput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        // MENUS
        x = juce::roundToInt(getWidth() * 0.025);
        width = juce::roundToInt(getWidth() * 0.3);
        height = juce::roundToInt(getHeight() * 0.05);
        y = juce::roundToInt(getHeight() * 0.025);
        const auto padding = juce::roundToInt(getWidth() * 0.021);
        m_preset_browser.setBounds(x, y, width, height);
        x += width + padding;
        width = juce::roundToInt(width * 0.75);
        m_oversampling_menu.setBounds(x, y, width, height);
        width = juce::roundToInt(width * 0.5);
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
        slider.setLookAndFeel(&m_io_laf);
        addAndMakeVisible(slider);
    }

    void BaseEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, m_widget_bg);
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
                         m_widget_bg);
        button.setColour(juce::TextButton::ColourIds::buttonOnColourId,
                         m_widget_bg);
        addAndMakeVisible(button);
    }
}
