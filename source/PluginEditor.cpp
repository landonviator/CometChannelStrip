#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p), m_rack(processorRef)
{
    juce::ignoreUnused(processorRef);

    const auto items = viator::globals::Oversampling::items;
    setComboBoxProps(m_oversampling_menu, items);

    addAndMakeVisible(m_rack);
    m_rack.addActionListener(this);
    m_rack.rebuild_editors();
    initMacroKnobs();

    m_view_port.setViewedComponent(&m_rack, false);
    m_view_port.setScrollBarsShown(false, true);
    addAndMakeVisible(m_view_port);

    refreshMacroMappings();

    setSize(1500, 700);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    for (auto &macro: m_macro_knobs)
    {
        macro.removeMouseListener(this);
    }

    m_rack.removeActionListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black.brighter(0.12f));

    g.setColour(juce::Colours::black);
    g.drawRect(0, 0, getWidth(), getHeight(), 3);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // OS MENU
    const auto padding = juce::roundToInt(getWidth() * 0.03);
    auto width = juce::roundToInt(getWidth() * 0.1);
    auto height = juce::roundToInt(getHeight() * 0.05);
    auto x = getWidth() - width - padding;
    auto y = padding;
    m_oversampling_menu.setBounds(x, y, width, height);

    const int rackX = juce::roundToInt(getWidth() * 0.0);
    const int rackY = getHeight() / 10;
    const int rackWidth = juce::roundToInt(getWidth());
    const int rackHeight = juce::roundToInt(getHeight() * 0.8);
    const int numEditors = static_cast<int>(m_rack.getEditors().size());
    const auto rack_extra = juce::roundToInt((rackWidth * 0.25)) * numEditors;

    m_rack.setParentWidth(rackWidth);
    m_rack.setBounds(
            rackX,
            rackY,
            numEditors < 4 ? rackWidth : rackWidth + rack_extra,
            rackHeight);

    m_view_port.setBounds(rackX, rackY, rackWidth, rackHeight);

    // MACRO DIALS
    x = juce::roundToInt(getWidth() * 0.026);
    y = juce::roundToInt(getHeight() * 0.9);
    width = juce::roundToInt(getWidth() * 0.05);
    height = width;
    for (auto &knob: m_macro_knobs)
    {
        knob.setBounds(x, y, width, height);
        x += width * 2;
    }
}

void AudioPluginAudioProcessorEditor::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
{
    box.addItemList(items, 1);
    box.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(box);
}

void AudioPluginAudioProcessorEditor::initMacroKnobs()
{
    for (int i = 0; i < m_macro_knobs.size(); ++i)
    {
        m_macro_knobs[i].setSliderStyle(juce::Slider::RotaryVerticalDrag);
        m_macro_knobs[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        m_macro_knobs[i].setComponentID("macro" + juce::String(i + 1) + "ID");
        m_macro_knobs[i].addMouseListener(this, true);
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