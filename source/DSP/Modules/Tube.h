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
    class Tube
    {
    public:
        Tube() = default;

        void prepare(juce::dsp::ProcessSpec &spec)
        {
            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
                drive.setCurrentAndTargetValue(1.0f);
            }

            for (auto &drive: m_drive_comp_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
                drive.setCurrentAndTargetValue(1.0f);
            }

            for (auto &drive: m_mix_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
                drive.setCurrentAndTargetValue(1.0f);
            }

            for (auto &filter: m_dc_filters)
            {
                filter.prepare(spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(5.0f);
            }

            for (auto &filter: m_miller_cap_filter)
            {
                filter.prepare(spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
                filter.setCutoffFrequency(10000.0f);
            }
        }

        void processBlock(juce::dsp::AudioBlock<float> &block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    float xn = data[sample];
                    const float drive = m_drive_smoothers[channel].getNextValue();
                    const float drive_comp = m_drive_comp_smoothers[channel].getNextValue();
                    float yn = xn * drive;
                    yn = processConduction(yn, 1.5f);
                    yn = processTube(yn, 1.0f, 1.5f, 1.0f, 4.0f, -1.5f);
                    yn = m_dc_filters[channel].processSample(static_cast<int>(channel), yn);
                    yn = m_miller_cap_filter[channel].processSample(static_cast<int>(channel), yn);
                    yn *= 0.8f * drive_comp;
                    data[sample] = yn;
                }
            }
        }

        inline float processConduction(const float xn, const float thresh)
        {
            const auto mask = static_cast<float>(xn >= 0.0f);

            float clip_delta = xn - thresh;
            clip_delta = std::fmax(clip_delta, 0.0f);
            const float compressionFactor = 0.447f + 0.545f * std::exp(-0.3241584f * clip_delta);

            return (1.0f - mask) * xn + compressionFactor * xn * mask;
        }

        inline float processTube(float xn, const float k, const float thresh, const float offset, const float
        clip_pos, const float clip_neg)
        {
            xn += offset;
            float yn = 0.0;

            if (xn > thresh)
            {
                if (xn > clip_pos)
                {
                    yn = clip_pos;
                } else
                {
                    xn -= thresh;

                    if (clip_pos > 1.0f)
                    {
                        xn /= (clip_pos - thresh);
                    }

                    yn = xn * (3.0f / 2.0f) * (1.0f - (xn * xn) / 3.0f);
                    yn *= (clip_pos - thresh);
                    yn += thresh;
                }
            } else if (xn > 0.0f)
            {
                yn = xn;
            } else
            {
                if (xn < clip_neg)
                {
                    yn = clip_neg;
                } else
                {
                    if (clip_neg < -1.0)
                    {
                        xn /= fabs(clip_neg);
                    }

                    yn = tanh(k * xn) / tanh(k);
                    yn *= std::fabs(clip_neg);
                }
            }

            return yn;
        }

        void setDrive(const float value)
        {
            for (auto &drive: m_drive_smoothers)
            {
                drive.setTargetValue(juce::Decibels::decibelsToGain(value * 0.3f));
            }

            for (auto &drive: m_drive_comp_smoothers)
            {
                const auto raw_comp = value * -0.3f;
                const auto comp_scaled = juce::jlimit(-15.0f, 0.0f, raw_comp);
                drive.setTargetValue(juce::Decibels::decibelsToGain(comp_scaled));
            }

            for (auto &drive: m_mix_smoothers)
            {
                drive.setTargetValue(value * 0.01f);
            }
        }

    private:
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers, m_drive_comp_smoothers, m_mix_smoothers;
        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_dc_filters, m_miller_cap_filter;
    };
}

