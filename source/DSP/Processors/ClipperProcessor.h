//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BaseProcessor.h"

namespace ClipperParameters
{
    inline const juce::String driveID = "driveID";
    inline const juce::String driveName = "Drive";

    struct parameters
    {
        explicit parameters(const juce::AudioProcessorValueTreeState &state, int id)
        {
            driveParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    ClipperParameters::driveID + juce::String(id)));

        }

        juce::AudioParameterFloat *driveParam{nullptr};
    };
}

namespace viator::dsp::processors
{
    class ClipperProcessor
            : public viator::dsp::processors::BaseProcessor, public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        //==============================================================================
        ClipperProcessor(int id);

        ~ClipperProcessor() override;

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

    private:
        juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(int id);
        void parameterChanged(const juce::String &parameterID, float newValue) override;

        std::unique_ptr<ClipperParameters::parameters> parameters;
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;

        void updateParameters();

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipperProcessor)
    };
}



