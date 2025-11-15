//
// Created by Landon Viator on 11/1/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>
#include "DSP/Units/ConsoleModule.h"
#include "DSP/Units/MasterBus.h"
#include "DSP/Modules/Tube.h"
#include "DSP/Modules/GraphicEq.h"

namespace viator::dsp
{
    class ProcessBlock
    {
    public:
        ProcessBlock() = default;
        ~ProcessBlock() = default;

        void prepare(double sample_rate, int samples_per_block, int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            spec.sampleRate = sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels= num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float>>(spec.numChannels,
                    factor,
                    juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
                    true);
            m_oversampler->initProcessing(spec.maximumBlockSize);

            m_console_module.prepare(spec);
            m_master_bus.prepare(spec);
            m_tube.prepare(spec);
            m_graphic_eq.prepare(spec);
        }

        void process(juce::AudioBuffer<float>& buffer, const int num_samples)
        {
            const int num_oversampled_samples = num_samples * static_cast<int>(m_oversampler->getOversamplingFactor());
            juce::dsp::AudioBlock<float> block (buffer);
            auto up_sampled_block = m_oversampler->processSamplesUp(block);
            m_tube.processBlock(up_sampled_block, num_oversampled_samples);
            m_oversampler->processSamplesDown(block);
        }

        void updateParameters(viator::parameters::parameters& parameters)
        {
            m_console_module.setDrive(parameters.consoleDriveParam->get() * 0.1f);
            m_master_bus.setDrive(parameters.consoleDriveParam->get());
            m_tube.setDrive(parameters.consoleDriveParam->get());

            std::vector<float> gains = {parameters.graphic1Param->get(),
                                          parameters.graphic2Param->get(),
                                          parameters.graphic3Param->get(),
                                          parameters.graphic4Param->get(),
                                          parameters.graphic5Param->get(),
                                          parameters.graphic6Param->get(),
                                          parameters.graphic7Param->get(),
                                          parameters.graphic8Param->get(),
                                          parameters.graphic9Param->get(),
                                          parameters.graphic10Param->get()};

            m_graphic_eq.setFilterParameters(gains);
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float>> m_oversampler;

        viator::dsp::ConsoleModule<float> m_console_module;
        viator::dsp::MasterBus<float> m_master_bus;
        viator::dsp::Tube<float> m_tube;
        viator::dsp::GraphicEq<float> m_graphic_eq;
    };
}
