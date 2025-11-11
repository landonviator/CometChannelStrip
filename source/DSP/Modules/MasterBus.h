//
// Created by Landon Viator on 11/8/25.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

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

            for (auto &filter: m_dc_filters)
            {
                filter.prepare(spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(20.0f);
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
                    const float yn = getPoletti(xn, k, channel);
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

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_dc_filters;

        static inline float getWaveShaped(const float xn, const float k, const float ln, const float lp)
        {
            const float ax = std::fabs(xn);
            float yn_positive = 0.5f * (ax + xn);
            float yn_negative = 0.5f * (xn - ax);

            const float numerator_positive = yn_positive * k;
            const float numerator_negative = yn_negative * k;

            float denominator_positive = 1.0f + yn_positive * k / lp;
            float denominator_negative = 1.0f - yn_negative * k / ln;

            yn_positive = numerator_positive / denominator_positive;
            yn_negative = numerator_negative / denominator_negative;

            return yn_positive + yn_negative;
        }

        inline float getPoletti(const float xn, const float k, const int channel)
        {
            const float ax = std::fabs(xn);
            float yn_positive = 0.5f * (ax + xn);
            float yn_negative = 0.5f * (xn - ax);
            float mix = juce::jlimit(0.0f, 10.0f, k) * 0.1f;

            const float gain = 10.0f;
            yn_positive = getWaveShaped(yn_positive, gain, 0.6f, 6.6f);
            yn_negative = getWaveShaped(yn_negative, gain, 6.6f, 0.6f);

            yn_positive = m_dc_filters[channel].processSample(channel, yn_positive);
            yn_negative = m_dc_filters[channel].processSample(channel, yn_negative);

            const float symmetrical = 1.6f;
            yn_positive = getWaveShaped(yn_positive, gain, symmetrical, symmetrical);
            yn_negative = getWaveShaped(yn_negative, gain, symmetrical, symmetrical);

            const float yn = yn_positive + yn_negative;
            return (1.0f - mix) * xn + yn * mix;
        }
    };
}
