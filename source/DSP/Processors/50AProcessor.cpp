#include "50AProcessor.h"
#include "../../GUI/Editors/50AEditor.h"

namespace viator::dsp::processors
{

//==============================================================================
    AmplificationProcessor::AmplificationProcessor(int id)
            : BaseProcessor(BusesProperties()
                                    .withInput("Input", juce::AudioChannelSet::stereo(), true)
                                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
    }

    AmplificationProcessor::~AmplificationProcessor()
    {
    }

//==============================================================================
    const juce::String AmplificationProcessor::getName() const
    {
        return "50A";
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

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate <= 0 ? 44100.0f : sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = getTotalNumOutputChannels();

        for (auto& filter : m_lp_filter)
        {
            filter.prepare(spec);
            filter.setCutoffFrequency(800.0f);
            filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        }
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

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto *data = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const auto xn = data[sample];
                const auto yn = m_lp_filter[channel].processSample(channel, xn);
                data[sample] = yn;
            }
        }
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
}