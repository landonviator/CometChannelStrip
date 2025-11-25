//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "ProcessorIncludes.h"
#include "../../GUI/Editors/EditorIncludes.h"

namespace viator::dsp::processors
{
    enum class ProcessorType
    {
        kClipper,
        k50A,
        kTest
    };

    struct ProcessorDefinition
    {
        ProcessorType type;
        juce::String name;
        juce::String category;
        std::function<std::unique_ptr<juce::AudioProcessor>(int)> createProcessor;
        std::function<std::unique_ptr<juce::AudioProcessorEditor>(juce::AudioProcessor &)> createEditor;
    };

    inline const std::vector<ProcessorDefinition>& getProcessorRegistry()
    {
        static const std::vector<ProcessorDefinition> registry = {
                {
                        ProcessorType::kClipper,
                        "Clipper",
                        "Test",
                        [](int id)
                        {
                            return std::make_unique<viator::dsp::processors::ClipperProcessor>(id);
                        },
                        [](juce::AudioProcessor& processor)
                        {
                            auto& type = dynamic_cast<viator::dsp::processors::ClipperProcessor&>(processor);
                            return std::make_unique<viator::gui::editors::ClipperEditor>(type);
                        }
                },
                {
                        ProcessorType::k50A,
                        "50A",
                        "Test",
                        [](int id)
                        {
                            return std::make_unique<viator::dsp::processors::AmplificationProcessor>(id);
                        },
                        [](juce::AudioProcessor& processor)
                        {
                            auto& typed = dynamic_cast<viator::dsp::processors::AmplificationProcessor&>(processor);
                            return std::make_unique<viator::gui::editors::AmplificationEditor>(typed);
                        }
                },
                {
                        ProcessorType::kTest,
                        "Test",
                        "Test",
                        [](int id)
                        {
                            return std::make_unique<viator::dsp::processors::TestProcessor>(id);
                        },
                        [](juce::AudioProcessor& processor)
                        {
                            auto& typed = dynamic_cast<viator::dsp::processors::TestProcessor&>(processor);
                            return std::make_unique<viator::gui::editors::TestEditor>(typed);
                        }
                }
        };

        return registry;
    }

    inline juce::String toString(ProcessorType type)
    {
        for (const auto &def: getProcessorRegistry())
            if (def.type == type)
                return def.name;

        jassertfalse;
        return {};
    }

    inline ProcessorType processorTypeFromString(const juce::String &name)
    {
        for (const auto &def: getProcessorRegistry())
            if (def.name == name)
                return def.type;

        jassertfalse;
        return ProcessorType::kClipper;
    }

    inline std::unique_ptr<BaseProcessor> createProcessorByType(ProcessorType type, int index)
    {
        for (const auto& def : getProcessorRegistry())
            if (def.type == type)
                return std::unique_ptr<BaseProcessor>(static_cast<BaseProcessor*>(def.createProcessor(index).release()));

        jassertfalse;
        return nullptr;
    }

    inline std::unique_ptr<juce::AudioProcessorEditor> createEditorForProcessor(juce::AudioProcessor *processor)
    {
        DBG("Looking up editor for processor name: " + processor->getName());

        for (const auto &def: getProcessorRegistry())
        {
            DBG("  Checking registry name: " + def.name);
            if (toString(def.type) == processor->getName())
            {
                DBG("  Match found! Creating editor for: " + def.name);
                return def.createEditor(*processor);
            }
        }

        DBG("!! No match found. Falling back or asserting.");
        // fallback
        if (auto *bc = dynamic_cast<viator::dsp::processors::ClipperProcessor *>(processor))
            return std::make_unique<viator::gui::editors::ClipperEditor>(*bc);

        jassertfalse;
        return nullptr;
    }


    inline std::optional<ProcessorType> getProcessorTypeForProcessor(juce::AudioProcessor *processor)
    {
        if (processor == nullptr)
            return std::nullopt;

        for (const auto &def: getProcessorRegistry())
        {
            // We use the processor's name (set in the constructor) as an identifier
            if (processor->getName() == def.name)
                return def.type;
        }

        return std::nullopt;
    }
}