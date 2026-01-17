#include "ClipperProcessor.h"
#include "../../../GUI/Editors/ClipperEditor.h"

namespace viator::dsp::processors
{
    //==============================================================================
    ClipperProcessor::ClipperProcessor(int id)
        : BaseProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        BaseProcessor::setProcessorID(id);
        auto layout = createParameterLayout(id);
        initTreeState(this, std::move(layout));
        m_parameters = std::make_unique<ClipperParameters::parameters>(getTreeState(), id);
        getTreeState().addParameterListener(ClipperParameters::oversamplingChoiceID + juce::String(id), this);
        getTreeState().addParameterListener(ClipperParameters::driveID + juce::String(id), this);
        getTreeState().addParameterListener(ClipperParameters::clipTypeID + juce::String(id), this);
    }

    ClipperProcessor::~ClipperProcessor()
    {
    }

    juce::AudioProcessorValueTreeState::ParameterLayout ClipperProcessor::createParameterLayout(const int id)
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ClipperParameters::driveID + juce::String(id), 1},
            ClipperParameters::driveName + juce::String(id),
            0.0f,
            30.0f,
            0.0f));

        const juce::StringArray choices = {"Soft", "Hard"};
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ClipperParameters::clipTypeID + juce::String(id), 1},
            ClipperParameters::clipTypeName + juce::String(id),
            choices, 0));

        const juce::StringArray os_choices = viator::globals::Oversampling::items;
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{ClipperParameters::oversamplingChoiceID + juce::String(id), 1},
            ClipperParameters::oversamplingChoiceName + juce::String(id),
            os_choices, 0));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ClipperParameters::muteID + juce::String(id), 1},
            ClipperParameters::muteName + juce::String(id),
            false));

        return {params.begin(), params.end()};
    }

    void ClipperProcessor::parameterChanged(const juce::String &parameterID, float newValue)
    {
    }

    //==============================================================================
    const juce::String ClipperProcessor::getName() const
    {
        return "Clipper";
    }

    bool ClipperProcessor::acceptsMidi() const
    {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool ClipperProcessor::producesMidi() const
    {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool ClipperProcessor::isMidiEffect() const
    {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double ClipperProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int ClipperProcessor::getNumPrograms()
    {
        return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
    }

    int ClipperProcessor::getCurrentProgram()
    {
        return 0;
    }

    void ClipperProcessor::setCurrentProgram(int index)
    {
        juce::ignoreUnused(index);
    }

    const juce::String ClipperProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }

    void ClipperProcessor::changeProgramName(int index, const juce::String &newName)
    {
        juce::ignoreUnused(index, newName);
    }

    void ClipperProcessor::updateParameters()
    {
        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
        {
            m_process_blocks[static_cast<size_t>(oversampling_choice)].updateParameters(*m_parameters);
        }

        const auto should_mute = m_parameters->muteParam->get();

        for (auto& mute : m_mutes)
        {
            mute.setTargetValue(!static_cast<float>(should_mute));
        }
    }

    //==============================================================================
    void ClipperProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        // Use this method as the place to do any pre-playback
        // initialisation that you need..
        juce::ignoreUnused(sampleRate, samplesPerBlock);

        for (auto& mute : m_mutes)
        {
            mute.reset(sampleRate, 0.02);
        }

        for (int i = 0; i < m_process_blocks.size(); ++i)
        {
            m_process_blocks[i].prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels(), i);
        }

        m_dry_buffer.setSize(getTotalNumOutputChannels(), sampleRate);
    }

    void ClipperProcessor::releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool ClipperProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
    {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused(layouts);
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

    void ClipperProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages)
    {
        juce::ignoreUnused(midiMessages);

        updateParameters();

        m_dry_buffer.clear();
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            m_dry_buffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        }

        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
        {
            m_process_blocks[static_cast<size_t>(oversampling_choice)].process(buffer, buffer.getNumSamples());
        }


        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            const auto* dry_data = m_dry_buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const auto dry_sample = dry_data[sample];
                const auto wet_sample = data[sample];
                const auto mix = m_mutes[channel].getNextValue();
                data[sample] = (1.0f - mix) * dry_sample + wet_sample * mix;
            }
        }
    }

    //==============================================================================
    bool ClipperProcessor::hasEditor() const
    {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor *ClipperProcessor::createEditor()
    {
        return new viator::gui::editors::ClipperEditor(*this);
    }
}
