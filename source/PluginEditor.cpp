#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p), m_rack(processorRef)
{
    juce::ignoreUnused(processorRef);

    const auto items = viator::globals::Size::items;
    setComboBoxProps(m_size_menu, items);

    m_size_menu.onChange = [this]()
    {
        constexpr auto base_width = 1400;

        std::unordered_map<int, double> size_map =
        {
            {0, 0.5},  {1, 0.75}, {2, 1.0}, {3, 1.25}, {4, 1.5}, {5, 1.75}, {6, 2.0}
        };

        const auto choice = m_size_menu.getSelectedItemIndex();
        const auto width = juce::roundToInt(base_width * size_map[choice]);
        const auto height = width / 2;
        setSize(width, height);
    };

    addAndMakeVisible(m_rack);
    m_rack.addActionListener(this);
    m_rack.rebuild_editors();

    //addAndMakeVisible(m_macro_bg);

    initMacroKnobs();

    m_view_port.setViewedComponent(&m_rack, false);
    m_view_port.setScrollBarsShown(false, true);
    addAndMakeVisible(m_view_port);

    refreshMacroMappings();

    setResizable(true, false);
    getConstrainer()->setFixedAspectRatio(2.0);
    setResizeLimits(700, 350, 2800, 1400);
    setSize(1400, 700);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    for (auto &macro: m_macro_knobs)
    {
        macro.removeMouseListener(this);
        macro.setLookAndFeel(nullptr);
    }

    m_rack.removeActionListener(this);

    //m_macro_bg.setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds();

    juce::ColourGradient faceGrad(
        viator::gui_utils::Colors::main_bg().brighter(0.05f),
        bounds.getX() + bounds.getWidth() * 0.25f, bounds.getY() + bounds.getHeight() * 0.20f,
        viator::gui_utils::Colors::main_bg().darker(0.05f),
        bounds.getRight() - bounds.getWidth() * 0.15f, bounds.getBottom() - bounds.getHeight() * 0.10f,
        true
    );

    g.setGradientFill(faceGrad);
    g.fillRect(bounds);

    // header
    auto line = static_cast<float>(getHeight()) * 0.08f;
    g.setColour(viator::gui_utils::Colors::light_bg());
    g.drawLine(0, line, static_cast<float>(getWidth()), line, 1.0f);

    // footer
    line = static_cast<float>(getHeight()) * 0.94f;
    g.drawLine(0, line, static_cast<float>(getWidth()), line, 1.0f);

    g.drawRect(bounds);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // SIZE MENU
    auto padding = juce::roundToInt(getHeight() * 0.01);
    const auto width = juce::roundToInt(getWidth() * 0.1);
    const auto x = getWidth() - width - padding;
    const auto y = padding;
    const auto height = juce::roundToInt(getHeight() * 0.05);
    m_size_menu.setBounds(x, y, width, height);

    const int rackX = juce::roundToInt(getWidth() * 0.05);
    const int rackY = getHeight() / 10;
    const int rackWidth = juce::roundToInt(getWidth() * 0.9);
    const int rackHeight = juce::roundToInt(getHeight() * 0.76);
    const int numEditors = static_cast<int>(m_rack.getEditors().size());
    const auto rack_extra = juce::roundToInt((rackWidth * 0.25)) * numEditors;

    m_rack.setParentWidth(rackWidth);
    m_rack.setBounds(
            rackX,
            rackY,
            numEditors < 4 ? rackWidth : rackWidth + rack_extra,
            rackHeight);

    m_view_port.setBounds(rackX, rackY, rackWidth, rackHeight);

    // Macro Dials
    auto compX = juce::roundToInt(getWidth() * 0.1);
    const auto compY = juce::roundToInt(getHeight() * 0.84);
    const auto compWidth = juce::roundToInt(getWidth() * 0.05);
    const auto compHeight = juce::roundToInt(compWidth * 1.089);
    padding = juce::roundToInt(getWidth() * 0.033);
    for (auto &dial: m_macro_knobs)
    {
        dial.setBounds(compX, compY, compWidth, compHeight);
        compX += compWidth + padding;
    }
}

void AudioPluginAudioProcessorEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
{
    box.addItemList(items, 1);
    box.setSelectedId(3, juce::dontSendNotification);
    addAndMakeVisible(box);
}

void AudioPluginAudioProcessorEditor::initMacroKnobs()
{
    for (int i = 0; i < m_macro_knobs.size(); ++i)
    {
        m_macro_knobs[i].setSliderStyle(juce::Slider::RotaryVerticalDrag);
        m_macro_knobs[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        m_macro_knobs[i].setComponentID("macro" + juce::String(i + 1) + "ID");
        m_macro_knobs[i].setName("Macro\n" + juce::String(i+1));
        m_macro_knobs[i].addMouseListener(this, true);
        m_macro_knobs[i].setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentWhite);
        m_macro_knobs[i].setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(234, 234, 234));
        m_macro_knobs[i].setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, viator::gui_utils::Colors::dial_bg());
        m_macro_knobs[i].setLookAndFeel(&m_macro_laf);
        m_macro_attaches.emplace_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
                                                                                               .getTreeState(),
                                                                                       "macro" + juce::String(i + 1) +
                                                                                       "ID", m_macro_knobs[i]));
        addAndMakeVisible(m_macro_knobs[i]);
    }
}

void AudioPluginAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (auto slider = dynamic_cast<viator::gui::widgets::BaseSlider *>(source))
    {
        const auto slider_id_to_map = slider->getComponentID();
        const auto is_mapped = slider->getIsMapped();

        if (is_mapped)
        {
            processorRef.getMacroMap().removeMacroAssignment(slider_id_to_map);
        } else
        {
            processorRef.getMacroMap().addMacroAssignment(slider_id_to_map);
            slider->getProperties().set(viator::globals::WidgetProperties::macroKey, processorRef.getMacroMap()
                    .getCurrentMacro());
        }

        slider->setIsMapped(!is_mapped);
        slider->showMapping(!is_mapped);
    }
}

void AudioPluginAudioProcessorEditor::actionListenerCallback(const juce::String &message)
{
    if (message == viator::globals::ActionCommands::editorAdded)
    {
        for (auto &editor: m_rack.getEditors())
        {
            if (auto *base_editor = dynamic_cast<viator::gui::editors::BaseEditor *>(editor.get()))
            {
                for (auto &slider: base_editor->getSliders())
                {
                    slider->removeChangeListener(this);
                    slider->addChangeListener(this);
                }
            }
        }

        resized();
    }

    if (message == viator::globals::ActionCommands::editorDeleted)
    {
        resized();
    }
}

void AudioPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        if (auto *macro_slider = dynamic_cast<viator::gui::widgets::MacroSlider *>(event.eventComponent))
        {
            for (auto &macro: m_macro_knobs)
            {
                if (&macro != macro_slider)
                    macro.enableMacroState(false);
            }

            const auto selected_macro = macro_slider->getComponentID();
            macro_slider->toggleMacroState();
            const auto macro_state = macro_slider->getMacroState();
            processorRef.getMacroMap().setMacroLearnState(macro_state);
            processorRef.getMacroMap().macroStateChanged(selected_macro);

            for (auto &editor: m_rack.getEditors())
            {
                if (auto *base_editor = dynamic_cast<viator::gui::editors::BaseEditor *>(editor.get()))
                {
                    for (auto &slider: base_editor->getSliders())
                    {
                        const auto state = static_cast<bool>(static_cast<int>(macro_state));
                        const auto is_macro = selected_macro == slider->getProperties().getWithDefault(
                                viator::globals::WidgetProperties::macroKey, "").toString();
                        slider->showMapping(state && is_macro);
                    }
                }
            }
        }
    }
}

void AudioPluginAudioProcessorEditor::refreshMacroMappings()
{
    for (auto& editor : m_rack.getEditors())
    {
        if (auto* base_editor = dynamic_cast<viator::gui::editors::BaseEditor*>(editor.get()))
        {
            for (auto* slider : base_editor->getSliders())
            {
                const auto sliderID = slider->getComponentID();
                const auto macroID  = processorRef.getMacroMap().getMacroForSlider(sliderID);
                const bool mapped   = macroID.isNotEmpty();

                // restore component property (GUI-only)
                if (mapped)
                    slider->getProperties().set(viator::globals::WidgetProperties::macroKey, macroID);
                else
                    slider->getProperties().remove(viator::globals::WidgetProperties::macroKey);

                // restore internal slider state
                slider->setIsMapped(mapped);

                // default hide until a macro knob is active
                slider->showMapping(false);
            }
        }
    }
}