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
        kReduction,
        kAmplification
    };

    struct ProcessorDefinition
    {
        ProcessorType type;
        juce::String name;
        juce::String category;
        std::function<std::unique_ptr<juce::AudioProcessor>(int)> createProcessor;
        std::function<std::unique_ptr<juce::AudioProcessorEditor>(juce::AudioProcessor &)> createEditor;
    };

    inline const std::vector<ProcessorDefinition> &getProcessorRegistry()
    {
        static const std::vector<ProcessorDefinition> registry = {
                {
                        ProcessorType::kReduction,
                        "Reduction",
                        "Test",
                        [](int id)
                        {
                            return std::make_unique<viator::dsp::processors::ReductionProcessor>(id);
                        },
                        [](juce::AudioProcessor &processor)
                        {
                            auto &type = dynamic_cast<viator::dsp::processors::ReductionProcessor &>(processor);
                            return std::make_unique<viator::gui::editors::ReductionEditor>(type);
                        }
                },
                {
                        ProcessorType::kAmplification,
                        "Amplification",
                        "Test",
                        [](int id)
                        {
                            return std::make_unique<viator::dsp::processors::AmplificationProcessor>(id);
                        },
                        [](juce::AudioProcessor &processor)
                        {
                            auto &typed = dynamic_cast<viator::dsp::processors::AmplificationProcessor &>(processor);
                            return std::make_unique<viator::gui::editors::AmplificationEditor>(typed);
                        }
                },
        };

        return registry;
    }

    inline juce::String toString(ProcessorType type)
    {
        for (const auto& def : getProcessorRegistry())
        {
            if (def.type == type)
            {
                return def.name;
            }

            jassertfalse;
            return {};
        }
    }

    inline ProcessorType processorTypeFromString(const juce::String& name)
    {
        for (const auto& def : getProcessorRegistry())
        {
            if (def.name == name)
            {
                return def.type;
            }
        }

        jassertfalse;
    }

    inline std::unique_ptr<viator::dsp::processors::BaseProcessor> createProcessorByType(ProcessorType type, int index)
    {
        for (const auto& def : getProcessorRegistry())
        {
            if (def.type == type)
            {
                return std::unique_ptr<viator::dsp::processors::BaseProcessor>
                        (dynamic_cast<viator::dsp::processors::BaseProcessor*>(def.createProcessor(index).release()));
                jassertfalse;
            }
        }
    }

    inline std::unique_ptr<juce::AudioProcessorEditor> createEditorForProcessor
    (juce::AudioProcessor* processor)
    {
        for (const auto& def : getProcessorRegistry())
        {
            if (toString(def.type) == processor->getName())
            {
                return def.createEditor(*processor);
            }
        }

        jassertfalse;
    }

    inline std::optional<ProcessorType> getProcessorTypeForProcessor(viator::dsp::processors::BaseProcessor* processor)
    {
        if (!processor)
        {
            return std::nullopt;
        }

        for (const auto& def : getProcessorRegistry())
        {
            if (processor->getName() == def.name)
            {
                return def.type;
            }

            return std::nullopt;
        }
    }
}
