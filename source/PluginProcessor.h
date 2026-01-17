#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Globals/Parameters.h"
#include "Globals/Globals.h"
#include "DSP/Processors/BaseProcessor.h"
#include "DSP/Processors/ProcessorUtils.h"
#include "Engine/MacroMap.h"
//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor,
        public juce::AudioProcessorValueTreeState::Listener,
public juce::ActionBroadcaster
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getTreeState() { return m_tree_state; }

    void addProcessor(viator::dsp::processors::ProcessorType type);
    void swapProcessors(const int a, const int b);
    void removeProcessor(const int index);
    viator::dsp::processors::BaseProcessor* getProcessor(int index);

    std::vector<std::unique_ptr<viator::dsp::processors::BaseProcessor>>& getProcessors() { return m_processors; }

    std::atomic<bool> m_adding_processor {false};

    viator::engine::MacroMap& getMacroMap() { return m_macro_map; }
private:

    juce::AudioProcessorValueTreeState m_tree_state;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String &parameterID, float newValue);

    void updateParameters();

    std::unique_ptr<viator::parameters::parameters> m_parameters;

    std::vector<std::unique_ptr<viator::dsp::processors::BaseProcessor>> m_processors;

    juce::CriticalSection m_processor_lock;

    viator::engine::MacroMap m_macro_map;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
