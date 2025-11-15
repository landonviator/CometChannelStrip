#include "AmplificationProcessor.h"
#include "../../GUI/Editors/AmplificationEditor.h"

namespace viator::dsp::processors
{

//==============================================================================
    AmplificationProcessor::AmplificationProcessor(int id)
            : viator::dsp::processors::BaseProcessor(id)
    {
    }

    AmplificationProcessor::~AmplificationProcessor()
    {
    }

//==============================================================================
    const juce::String AmplificationProcessor::getName() const
    {
        return "Amplification";
    }

    bool AmplificationProcessor::acceptsMidi() const
    {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool AmplificationProcessor::producesMidi() const
    {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool AmplificationProcessor::isMidiEffect() const
    {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double AmplificationProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int AmplificationProcessor::getNumPrograms()
    {
        return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
    }

    int AmplificationProcessor::getCurrentProgram()
    {
        return 0;
    }

    void AmplificationProcessor::setCurrentProgram(int index)
    {
        juce::ignoreUnused(index);
    }

    const juce::String AmplificationProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }

    void AmplificationProcessor::changeProgramName(int index, const juce::String &newName)
    {
        juce::ignoreUnused(index, newName);
    }


//==============================================================================
    void AmplificationProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        // Use this method as the place to do any pre-playback
        // initialisation that you need..
        juce::ignoreUnused(sampleRate, samplesPerBlock);

    }

    void AmplificationProcessor::releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool AmplificationProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
    {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused (layouts);
        return true;
#else
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        // Some plugin hosts, such as certain GarageBand versions, will only
        // load plugins that support stereo bus layouts.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
#endif

        return true;
#endif
    }

    void AmplificationProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                     juce::MidiBuffer &midiMessages)
    {
        juce::ignoreUnused(midiMessages);

        buffer.applyGain(1.5f);
    }

//==============================================================================
    bool AmplificationProcessor::hasEditor() const
    {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor *AmplificationProcessor::createEditor()
    {
        return new viator::gui::editors::AmplificationEditor (*this);
    }

//==============================================================================
    void AmplificationProcessor::getStateInformation(juce::MemoryBlock &destData)
    {
        // You should use this method to store your parameters in the memory block.
        // You could do that either as raw data, or use the XML or ValueTree classes
        // as intermediaries to make it easy to save and load complex data.
        juce::ignoreUnused(destData);
    }

    void AmplificationProcessor::setStateInformation(const void *data, int sizeInBytes)
    {
        // You should use this method to restore your parameters from this memory block,
        // whose contents will have been created by the getStateInformation() call.
        juce::ignoreUnused(data, sizeInBytes);
    }
}