//
// Created by Landon Viator on 11/8/25.
//

#pragma once
#include "juce_dsp/juce_dsp.h"

namespace viator::dsp
{
    template <typename SampleType>
    class ConsoleModule
    {
    public:
        ConsoleModule() = default;

        void prepare(juce::dsp::ProcessSpec& spec)
        {
            for (auto& drive : m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
            }
        }

        void processBlock(juce::dsp::AudioBlock<float>& block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < num_samples; ++sample) {
                    const float xn = data[sample];
                    const float k = m_drive_smoothers[channel].getNextValue();
                    const float yn = xn + k / two_pi * std::sin(xn * two_pi);
                    data[sample] = yn;
                }
            }
        }

        void setDrive(const float value)
        {
            for (auto& drive : m_drive_smoothers)
            {
                drive.setTargetValue(value);
            }
        }

    private:

        static constexpr float two_pi = juce::MathConstants<float>::pi * 2.0f;

        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;
    };
}
