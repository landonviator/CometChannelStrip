//
// Created by Landon Viator on 11/1/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>
#include "../../modules/GraphicEq.h"

namespace LV60GraphicEQParameters
{
    inline const juce::String oversamplingChoiceID   = "oversamplingChoiceID";
    inline const juce::String oversamplingChoiceName = "oversamplingChoiceName";

    inline const juce::String muteID   = "muteID";
    inline const juce::String muteName = "Mute";

    inline const juce::String hpCutoffID   = "hpCutoffID";
    inline const juce::String hpCutoffName = "HP Hz";

    inline const juce::String lpCutoffID   = "lpCutoffID";
    inline const juce::String lpCutoffName = "LP Hz";

    inline const juce::String driveID   = "driveID";
    inline const juce::String driveName = "Drive";

    constexpr int numBands = 10;

    inline const std::array<juce::String, numBands> gainIDs = {
        "gain1ID", "gain2ID", "gain3ID", "gain4ID", "gain5ID",
        "gain6ID", "gain7ID", "gain8ID", "gain9ID", "gain10ID"
    };

    struct parameters
    {
        explicit parameters(const juce::AudioProcessorValueTreeState& state, const int id)
        {
            oversamplingParam = dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(
                oversamplingChoiceID + juce::String(id)));

            hpCutoffParam = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(
                hpCutoffID + juce::String(id)));

            lpCutoffParam = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(
                lpCutoffID + juce::String(id)));

            driveParam = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(
                driveID + juce::String(id)));

            muteParam = dynamic_cast<juce::AudioParameterBool*>(state.getParameter(
                muteID + juce::String(id)));

            for (int i = 0; i < numBands; ++i)
                gainParams[i] = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(
                    gainIDs[static_cast<size_t>(i)] + juce::String(id)));
        }

        juce::AudioParameterChoice* oversamplingParam{ nullptr };
        juce::AudioParameterFloat*  hpCutoffParam{ nullptr };
        juce::AudioParameterFloat*  lpCutoffParam{ nullptr };
        juce::AudioParameterFloat*  driveParam{ nullptr };
        juce::AudioParameterBool*   muteParam{ nullptr };

        std::array<juce::AudioParameterFloat*, numBands> gainParams{};
    };
}

namespace viator::dsp
{
    class LV60GraphicEQProcessBlock {
    public:
        LV60GraphicEQProcessBlock() = default;

        ~LV60GraphicEQProcessBlock() = default;

        enum { kNumBands = LV60GraphicEQParameters::numBands };
        enum {kHP = 0, kLP, kNum_filters };

        void prepare(const double sample_rate, const int samples_per_block, const int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            spec.sampleRate = sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels = num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float> >(spec.numChannels,
                                                                              factor,
                                                                              juce::dsp::Oversampling<
                                                                                  float>::FilterType::filterHalfBandPolyphaseIIR,
                                                                              true);
            m_oversampler->initProcessing(spec.maximumBlockSize);

            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate <= 0 ? 44100.0f : spec.sampleRate, 0.02);
            }

            m_graphic_eq.prepare(spec);

            for (auto& filter: m_filters)
            {
                filter.prepare(spec);
            }

            m_filters[kHP].setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            m_filters[kLP].setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        }

        void process(juce::AudioBuffer<float> &buffer, const int num_samples)
        {

            juce::dsp::AudioBlock<float> block(buffer);

            const auto up_sampled_block = m_oversampler->processSamplesUp(block);
            processSaturation(up_sampled_block, buffer.getNumSamples());
            m_oversampler->processSamplesDown(block);

            m_graphic_eq.processBlock(block, num_samples);
            for (auto& filter: m_filters)
            {
                filter.process(juce::dsp::ProcessContextReplacing<float>(block));
            }
        }

        void updateParameters(LV60GraphicEQParameters::parameters &parameters)
        {
            for (auto &drive: m_drive_smoothers)
            {
                if (parameters.driveParam)
                {
                    const auto raw_drive = parameters.driveParam->get();
                    const auto safe_drive = juce::jlimit(0.0f, 50.0f, raw_drive);
                    const auto db_drive = juce::Decibels::decibelsToGain(safe_drive);
                    drive.setTargetValue(db_drive);
                }
            }

            std::vector<float> gains{};
            gains.reserve(LV60GraphicEQParameters::numBands);

            for (size_t i = 0; i < LV60GraphicEQParameters::numBands; ++i)
                gains.push_back(parameters.gainParams[i] ? parameters.gainParams[i]->get() : 0.0f);

            m_graphic_eq.setFilterParameters(gains);

            const auto hp_cutoff = parameters.hpCutoffParam->get();
            const auto lp_cutoff = parameters.lpCutoffParam->get();
            m_filters[kHP].setCutoffFrequency(hp_cutoff);
            m_filters[kLP].setCutoffFrequency(lp_cutoff);
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float> > m_oversampler;
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;
        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_filters;

        viator::dsp::GraphicEq<float> m_graphic_eq;

        void processSaturation(const juce::dsp::AudioBlock<float>& block, const int num_samples)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const float xn = data[sample];
                    const float drive = m_drive_smoothers[channel].getNextValue();
                    const float yn = std::tanh(xn * drive);
                    data[sample] = yn;
                }
            }
        }
    };
}
