//
// Created by Landon Viator on 11/17/25.
//

#include "EditorRack.h"
#include "../../PluginProcessor.h"

namespace viator::gui::views
{
    EditorRack::EditorRack(AudioPluginAudioProcessor &p) : processorRef(p)
    {
        processorRef.addActionListener(this);

        m_plugin_selector.setTextWhenNothingSelected("Add Plugin Module");
        buildPopupMenu();
        addAndMakeVisible(m_plugin_selector);

        m_plugin_selector.onChange = [this]()
        {
            const auto selectedId = m_plugin_selector.getSelectedId();
            const auto &registry = viator::dsp::processors::getProcessorRegistry();

            auto it = std::find_if(registry.begin(), registry.end(),
                                   [selectedId, this](const auto &def)
                                   {
                                       return selectedId == processorIdOffset + static_cast<int>(def.type);
                                   });

            if (it != registry.end())
                processorRef.addProcessor(it->type);

            m_plugin_selector.setSelectedId(0); // reset
        };
    }

    EditorRack::~EditorRack()
    {
        processorRef.removeActionListener(this);

        for (const auto& editor : m_editors)
        {
            if (auto *base = dynamic_cast<viator::gui::editors::BaseEditor *>(editor.get()))
            {
                base->removeActionListener(this);
            }
        }
    }

    void EditorRack::paint(juce::Graphics &g)
    {
        g.setColour(juce::Colours::wheat);
        g.drawRect(getLocalBounds());
    }

    void EditorRack::resized()
    {

        auto x = juce::roundToInt(getWidth() * 0);
        auto y = juce::roundToInt(getHeight() * 0);
        auto width = juce::roundToInt(parent_width * 0.25);
        auto height = juce::roundToInt(getHeight());
        for (const auto &m_editor: m_editors)
        {
            m_editor->setBounds(x, y, width, height);
            x += width;
        }

        const auto is_empty = m_editors.empty();
        const auto box_width = juce::roundToInt(getWidth() * 0.2);
        const auto box_height = juce::roundToInt(getHeight() * 0.1);
        const auto box_x = is_empty ? 0
                                    : m_editors[m_editors.size() - 1]->getRight();
        m_plugin_selector.setBounds(getLocalBounds().withSizeKeepingCentre(box_width, box_height).withX(box_x));
    }

    void EditorRack::addEditor()
    {
        const int index = static_cast<int>(processorRef.getProcessors().size()) - 1;

        if (auto *processor = processorRef.getProcessor(index))
        {
            auto editor = viator::dsp::processors::createEditorForProcessor(processor);
            if (editor)
            {
                editor->addMouseListener(this, false);

                if (auto *base = dynamic_cast<viator::gui::editors::BaseEditor *>(editor.get()))
                {
                    base->addActionListener(this);
                }

                m_editors.push_back(std::move(editor));
                sendActionMessage(viator::globals::ActionCommands::editorAdded);
                addAndMakeVisible(*m_editors.back());
            }
        }

        resized();
    }

    void EditorRack::rebuild_editors()
    {
        m_editors.clear();
        removeAllChildren();
        addAndMakeVisible(m_plugin_selector);

        const int numProcessors = static_cast<int>(processorRef.getProcessors().size());

        for (int i = 0; i < numProcessors; ++i)
        {
            if (auto *processor = processorRef.getProcessor(i))
            {
                auto editor = viator::dsp::processors::createEditorForProcessor(processor);
                if (editor)
                {

                    editor->addMouseListener(this, false);

                    if (auto *base = dynamic_cast<viator::gui::editors::BaseEditor *>(editor.get()))
                    {
                        base->addActionListener(this);
                    }

                    m_editors.push_back(std::move(editor));
                    sendActionMessage(viator::globals::ActionCommands::editorAdded);
                    addAndMakeVisible(*m_editors.back());
                }
            }
        }

        resized();
    }

    void EditorRack::mouseDown(const juce::MouseEvent &e)
    {
        // STOP SCROLL ON WHEN OVER CONTROL
        auto *comp = e.eventComponent;

        if (dynamic_cast<juce::Slider *>(comp) != nullptr ||
            comp->findParentComponentOfClass<juce::Slider>() != nullptr)
        {
            return;
        }

        juce::AudioProcessorEditor *editor = nullptr;

        while (comp != nullptr)
        {
            editor = dynamic_cast<juce::AudioProcessorEditor *>(comp);
            if (editor != nullptr)
                break;

            comp = comp->getParentComponent();
        }

        if (editor != nullptr)
        {
            dragging_editor = editor;

            drag_offset_from_top_left = e.getEventRelativeTo(editor).position.toInt();

            for (int i = 0; i < m_editors.size(); ++i)
            {
                if (m_editors[i].get() == editor)
                {
                    drag_original_index = i;
                    break;
                }
            }
        }
    }

    void EditorRack::mouseDrag(const juce::MouseEvent &e)
    {
        if (dragging_editor == nullptr) return;

        const auto drag_position = e.getEventRelativeTo(this).position.toInt() - drag_offset_from_top_left;
        dragging_editor->setTopLeftPosition(drag_position);

        for (int i = 0; i < m_editors.size(); ++i)
        {
            if (m_editors[i].get() == dragging_editor) continue;

            const auto editorCenterX = m_editors[i]->getBounds().getCentreX();
            const auto draggingCenterX = dragging_editor->getBounds().getCentreX();

            if ((draggingCenterX < editorCenterX && drag_original_index > i) ||
                (draggingCenterX > editorCenterX && drag_original_index < i))
            {
                std::iter_swap(m_editors.begin() + drag_original_index, m_editors.begin() + i);
                processorRef.swapProcessors(drag_original_index, i);
                drag_original_index = i;
                resized();
                break;
            }
        }
    }

    void EditorRack::mouseUp(const juce::MouseEvent &)
    {
        dragging_editor = nullptr;
        drag_original_index = -1;
        resized();
    }

    void EditorRack::actionListenerCallback(const juce::String &message)
    {
        if (message == viator::globals::ActionCommands::modulesLoaded)
        {
            rebuild_editors();
        } else if (message == viator::globals::ActionCommands::processorAdded)
        {
            addEditor();
        }

        if (message.startsWith(viator::globals::ActionCommands::editorDeleted))
        {
            const auto hexStr = message.fromFirstOccurrenceOf(viator::globals::ActionCommands::editorDeleted, false, false);
            const auto ptrValue = hexStr.getHexValue64();
            const auto *editorPtr = reinterpret_cast<juce::AudioProcessorEditor *>(static_cast<intptr_t>(ptrValue));

            for (int i = 0; i < m_editors.size(); ++i)
            {
                if (m_editors[i].get() == editorPtr)
                {
                    remove_editor_at_index(i);
                    break;
                }
            }
        }
    }

    void EditorRack::buildPopupMenu()
    {
        std::map<juce::String, juce::PopupMenu> categories;

        for (const auto &def: viator::dsp::processors::getProcessorRegistry())
        {
            categories[def.category].addItem(
                    juce::PopupMenu::Item(def.name)
                            .setID(processorIdOffset + (int) def.type)
            );
        }

        for (auto &[cat, menu]: categories)
            m_plugin_selector.getRootMenu()->addSubMenu(cat, menu);
    }

    void EditorRack::remove_editor_at_index(const int index)
    {
        if (index < 0 || index >= m_editors.size())
            return;

        const auto &editor = m_editors[index];

        if (!editor)
            return;

        removeChildComponent(editor.get());
        m_editors.erase(m_editors.begin() + index);

        processorRef.removeProcessor(index);

        resized();

        sendActionMessage(viator::globals::ActionCommands::editorDeleted);
    }
}
