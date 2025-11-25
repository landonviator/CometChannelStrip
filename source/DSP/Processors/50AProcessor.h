//
// Created by Landon Viator on 11/14/25.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "BaseProcessor.h"



namespace viator::dsp::processors
{
class AmplificationProcessor : public viator::dsp::processors::BaseProcessor
    {
    public:
        //==============================================================================
        AmplificationProcessor(int id);
        ~AmplificationProcessor() override;

        //==============================================================================
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        //==============================================================================
        juce::AudioProcessorEditor* createEditor() override;
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
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;

    private:

        std::array<juce::dsp::LinkwitzRileyFilter<float>, 2> m_lp_filter;
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmplificationProcessor)
    };
}



