//
// Created by Landon Viator on 11/8/25.
//

#pragma once

#include "juce_dsp/juce_dsp.h"

namespace viator::dsp
{
    template<typename SampleType>
    class MasterBus
    {
    public:
        MasterBus() = default;

        void prepare(juce::dsp::ProcessSpec &spec)
        {
            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
            }

            for (int i = 0; i < m_negative_dc_filters.size(); ++i)
            {
                m_positive_dc_filters[i].prepare(spec);
                m_positive_dc_filters[i].setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                m_positive_dc_filters[i].setCutoffFrequency(5.0f);
                m_negative_dc_filters[i].prepare(spec);
                m_negative_dc_filters[i].setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                m_negative_dc_filters[i].setCutoffFrequency(5.0f);
            }
        }

        void processBlock(juce::dsp::AudioBlock<float> &block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    const float xn = data[sample];
                    const float k = m_drive_smoothers[channel].getNextValue();
                    const float yn = processPoletti(xn, k, channel);
                    data[sample] = yn;
                }
            }
        }

        void setDrive(const float value)
        {
            for (auto &drive: m_drive_smoothers)
            {
                drive.setTargetValue(juce::Decibels::decibelsToGain(value));
            }
        }

        inline float processWaveshaper(const float xn, const float k, const float lp, const float ln)
        {
            const float numerator = k * xn;

            const float x_positive = numerator / (1.0f + numerator / lp);
            const float x_negative = numerator / (1.0f - numerator / ln);

            const auto mask = static_cast<float>(xn >= 0.0f);

            return x_negative + (x_positive - x_negative) * mask;
        }

        inline float processPoletti(const float xn, const float k, const int channel)
        {
            const float mix = juce::jmap(k, 1.0f, 3.1f, 0.0f, 1.0f);
            float xn_positive = processWaveshaper(xn, k, 6.6f, 0.6f);
            float xn_negative = processWaveshaper(xn, k, 0.6f, 6.6f);

            xn_positive = m_positive_dc_filters[channel].processSample(channel, xn_positive);
            xn_negative = m_negative_dc_filters[channel].processSample(channel, xn_negative);

            xn_positive = processWaveshaper(xn_positive, k, 1.6f, 1.6f);
            xn_negative = processWaveshaper(xn_negative, k, 1.6f, 1.6f);

            const float yn = xn_positive + xn_negative;
            return (1.0f - mix) * xn + yn * mix;
        }

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_positive_dc_filters, m_negative_dc_filters;
    };
}
