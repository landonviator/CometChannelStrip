//
// Created by Landon Viator on 11/14/25.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace viator::dsp::processors
{
class BaseProcessor : public juce::AudioProcessor
    {
    public:
    explicit BaseProcessor(const BusesProperties& ioConfig) : juce::AudioProcessor(ioConfig) {}
    ~BaseProcessor() override = default;

    juce::AudioProcessorValueTreeState& getTreeState() const { return *m_tree_state; }

    void initTreeState(BaseProcessor* owner, juce::AudioProcessorValueTreeState::ParameterLayout layout)
    {
        m_tree_state = std::make_unique<juce::AudioProcessorValueTreeState>(*owner, nullptr, "PARAMETERS", std::move(layout));
    }

    int getProcessorID() { return m_processor_id; }

    void setProcessorID(const int id) { m_processor_id = id; }

    void getStateInformation(juce::MemoryBlock &destData) override
    {
        juce::MemoryOutputStream stream(destData, false);
        getTreeState().state.writeToStream(stream);
    }

    void setStateInformation(const void *data, int sizeInBytes) override
    {
        if (auto state = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes)); state.isValid()) {
            getTreeState().state = state;
        }
    }

    std::pair<float, float> getInputLevelsStereo() const
    {
        return { input_levels[0].getCurrentValue(), input_levels[1].getCurrentValue() };
    }

    std::pair<float, float> getOutputLevelsStereo() const
    {
        return { output_levels[0].getCurrentValue(), output_levels[1].getCurrentValue() };
    }

    void calculateInputPeakLevel(const juce::AudioBuffer<float> &buffer)
    {
        const int numInputChannels = getTotalNumInputChannels();
        const int numSamples = buffer.getNumSamples();

        for (int ch = 0; ch < numInputChannels; ++ch)
        {
            input_levels[ch].skip(numSamples);
            input_peaks[ch] = buffer.getMagnitude(ch, 0, numSamples);

            if (input_peaks[ch] < input_levels[ch].getCurrentValue())
                input_levels[ch].setTargetValue(input_peaks[ch]);
            else
                input_levels[ch].setCurrentAndTargetValue(input_peaks[ch]);
        }

        for (int ch = numInputChannels; ch < 2; ++ch)
        {
            input_levels[ch].skip(numSamples);
            input_peaks[ch] = 0.0f;
            input_levels[ch].setTargetValue(0.0f);
        }
    }

    void calculateOutputPeakLevel(const juce::AudioBuffer<float> &buffer)
    {
        const int numInputChannels = getTotalNumInputChannels();
        const int numSamples = buffer.getNumSamples();

        for (int ch = 0; ch < numInputChannels; ++ch)
        {
            output_levels[ch].skip(numSamples);
            output_peaks[ch] = buffer.getMagnitude(ch, 0, numSamples);

            if (output_peaks[ch] < output_levels[ch].getCurrentValue())
                output_levels[ch].setTargetValue(output_peaks[ch]);
            else
                output_levels[ch].setCurrentAndTargetValue(output_peaks[ch]);
        }

        for (int ch = numInputChannels; ch < 2; ++ch)
        {
            output_levels[ch].skip(numSamples);
            output_peaks[ch] = 0.0f;
            output_levels[ch].setTargetValue(0.0f);
        }
    }

private:

    std::unique_ptr<juce::AudioProcessorValueTreeState> m_tree_state;

    int m_processor_id { -1 };

    std::array<juce::SmoothedValue<float>, 2> input_levels;
    std::array<float, 2> input_peaks;
    std::array<juce::SmoothedValue<float>, 2> output_levels;
    std::array<float, 2> output_peaks;

    //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseProcessor)
    };
}



