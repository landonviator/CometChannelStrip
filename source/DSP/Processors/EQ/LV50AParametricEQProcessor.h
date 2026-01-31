//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../BaseProcessor.h"
#include "LV50AParametricEQProcessBlock.h"

namespace viator::dsp::processors
{
    class LV50AParametricEQProcessor
            : public viator::dsp::processors::BaseProcessor, public juce::AudioProcessorValueTreeState::Listener {
    public:
        //==============================================================================
        explicit LV50AParametricEQProcessor(int id);

        ~LV50AParametricEQProcessor() override;

        //==============================================================================
        void prepareToPlay(double sampleRate, int samplesPerBlock) override;

        void releaseResources() override;

        bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

        void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

        //==============================================================================
        juce::AudioProcessorEditor *createEditor() override;

        bool hasEditor() const override;

        //==============================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;

        bool producesMidi() const override;

        bool isMidiEffect() const override;

        double getTailLengthSeconds() const override;

        //==============================================================================
        int getNumPrograms() override;

        int getCurrentProgram() override;

        void setCurrentProgram(int index) override;

        const juce::String getProgramName(int index) override;

        void changeProgramName(int index, const juce::String &newName) override;

        enum class MuteState {
            kNormal,
            kMuted
        };

        MuteState m_mute_state { MuteState::kNormal };
    private:
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(int id);

        void parameterChanged(const juce::String &parameterID, float newValue) override;

        std::unique_ptr<LV50AParametricEQParameters::parameters> m_parameters;

        void updateParameters();

        std::array<viator::dsp::LV50AParametricEQProcessBlock, 5> m_process_blocks;

        std::array<juce::SmoothedValue<float>, 2> m_mutes;
        juce::AudioBuffer<float> m_dry_buffer;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LV50AParametricEQProcessor)
    };
}
