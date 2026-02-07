//
// Created by Landon Viator on 11/14/25.
//

#include "BaseEditor.h"
#include "GUI/Style/Images.h"

namespace viator::gui::editors
{
    BaseEditor::BaseEditor(viator::dsp::processors::BaseProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        const auto shadow_color = juce::Colours::black;
        auto shadow = juce::DropShadow(shadow_color, 10, {0, 4});

        //m_drop_shadow = std::make_unique<juce::DropShadower>(shadow);
       // m_drop_shadow->setOwner(this);

        // SLIDERS
        for (auto &slider: m_io_sliders) {
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

        for (auto &button: m_buttons) {
            button.setLookAndFeel(nullptr);
        }
    }

    //==============================================================================
    void BaseEditor::paint(juce::Graphics &g)
    {
        const auto bounds = getLocalBounds();

        // juce::ColourGradient faceGrad(
        //     gui_utils::Colors::medium_bg().brighter(0.05f),
        //     bounds.getX() + bounds.getWidth() * 0.25f, bounds.getY() + bounds.getHeight() * 0.20f,
        //     gui_utils::Colors::medium_bg().darker(0.05f),
        //     bounds.getRight() - bounds.getWidth() * 0.15f, bounds.getBottom() - bounds.getHeight() * 0.10f,
        //     true
        // );
        //
        // g.setGradientFill(faceGrad);
        g.setColour(juce::Colour(44, 49, 60));
        g.fillRect(bounds);

        constexpr auto padding = 1;

        //g.setColour(gui_utils::Colors::header_bg());
        g.fillRect(padding, 0, getWidth() - padding, juce::roundToInt(getHeight() * 0.05) + 12);
        g.fillRect(padding, juce::roundToInt(getHeight() * 0.91), getWidth() - padding, juce::roundToInt(getHeight() * 0.09));

        g.setColour(gui_utils::Colors::light_bg());
        g.drawRect(0, 0, getWidth(), getHeight(), padding);

        auto y = static_cast<float>(getHeight()) * 0.05f + 12.0f;
        g.drawLine(padding, y, static_cast<float>(getWidth()) - padding, y, padding);

        y = static_cast<float>(getHeight()) * 0.91f;
        g.drawLine(padding, y, static_cast<float>(getWidth()) - padding, y, padding);
    }

    void BaseEditor::resized()
    {
        // SLIDERS
        auto width = juce::roundToInt(getHeight() * 0.1);
        auto height = width;
        auto x = 0;
        auto y = juce::roundToInt(getHeight() * 0.91);
        m_io_sliders[kInput].setBounds(x, y, width, height);
        m_io_sliders[kInput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        // LABELS
        x = getWidth() - width;
        m_io_sliders[kOutput].setBounds(x, y, width, height);
        m_io_sliders[kOutput].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        constexpr auto padding = 2;

        // MENUS
        x = 4;
        width = juce::roundToInt(getWidth() * 0.335);
        height = juce::roundToInt(getHeight() * 0.05);
        y = juce::roundToInt(getHeight() * 0.012);
        m_preset_browser.setBounds(x, y, width, height);
        x += width + padding;
        width = juce::roundToInt(width * 0.75);
        m_oversampling_menu.setBounds(x, y, width, height);
        width = juce::roundToInt(width * 0.5);
        x = m_oversampling_menu.getRight() + padding;
        for (auto &button: m_buttons) {
            button.setBounds(x, y, width, height);
            x += width + padding;
        }
    }

    void BaseEditor::setSliderProps(juce::Slider &slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, gui_utils::Colors::dial_bg());
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
        slider.setLookAndFeel(&m_io_laf);
        addAndMakeVisible(slider);
    }

    void BaseEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId, gui_utils::Colors::light_bg());
        addAndMakeVisible(box);
        box.setSelectedId(1);
    }

    void BaseEditor::setButtonProps(juce::TextButton &button, const juce::String &name)
    {
        button.setButtonText(name);
        button.setColour(juce::ComboBox::ColourIds::outlineColourId,
                         juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::ColourIds::buttonOnColourId, gui_utils::Colors::widget_on());
        button.setColour(juce::TextButton::ColourIds::textColourOffId, gui_utils::Colors::text());
        button.setColour(juce::TextButton::ColourIds::textColourOnId, gui_utils::Colors::text());
        button.setLookAndFeel(&m_button_laf);
        addAndMakeVisible(button);
    }
}
