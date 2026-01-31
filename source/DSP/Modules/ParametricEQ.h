//
// Created by Landon Viator on 11/12/25.
//

#pragma once

//
// Created by Landon Viator on 11/8/25.
//

#pragma once

#include "juce_dsp/juce_dsp.h"

namespace viator::dsp
{
    template<typename SampleType>
    class ParametricEq {
    public:
        ParametricEq() = default;

        void prepare(juce::dsp::ProcessSpec &spec)
        {
            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
                drive.setCurrentAndTargetValue(1.0f);
            }

            for (auto &filter: m_filters)
            {
                filter.prepare(spec);
            }

            m_sample_rate = static_cast<float>(spec.sampleRate);
        }

        void processBlock(juce::dsp::AudioBlock<float> &block, const int num_samples)
        {
            for (auto &filter: m_filters)
            {
                filter.process(juce::dsp::ProcessContextReplacing<float>(block));
            }
        }

        void setFilterParameters(const float gain, const float cutoff, const float q, const int index) const
        {
            jassert(index < m_filters.size());
            const float gain_scaled = juce::Decibels::decibelsToGain(gain);
            *m_filters[index].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(m_sample_rate, cutoff,
                                                                                           q, gain_scaled);
        }

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        using NotchFilter = juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<float>,
            juce::dsp::IIR::Coefficients<float> >;

        std::array<NotchFilter, 4> m_filters;

        float m_sample_rate{44100.0f};
    };
}
