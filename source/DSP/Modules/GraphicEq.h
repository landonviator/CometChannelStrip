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
    class GraphicEq {
    public:
        GraphicEq() = default;

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

        void setFilterParameters(const std::vector<float> &gain_values)
        {
            jassert(gain_values.size() == m_cutoffs.size());
            jassert(gain_values.size() == m_filters.size());
            for (int i = 0; i < gain_values.size(); ++i)
            {
                const float gain = juce::Decibels::decibelsToGain(std::abs(gain_values[i] * 1.35f));
                const int end_item_index = gain_values.size() - 1;
                const float q = i == end_item_index ? gain * 0.1f + m_top_band_q : gain - m_band_q;
                *m_filters[i].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(m_sample_rate, m_cutoffs[i],
                                                                                           q,
                                                                                           juce::Decibels::decibelsToGain(
                                                                                               gain_values[i]));
            }
        }

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        using NotchFilter = juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<float>,
            juce::dsp::IIR::Coefficients<float> >;

        std::array<NotchFilter, 10> m_filters;

        static constexpr float m_top_band_q{0.5f};
        static constexpr float m_band_q{0.293f};

        float m_sample_rate{44100.0f};
        std::vector<float> m_cutoffs = {
            16000.0f, 8000.0f, 4000.0f, 2000.0f, 1000.0f, 500.0f, 250.0f, 125.0f, 63.0f, 31.0f
        };
    };
}
