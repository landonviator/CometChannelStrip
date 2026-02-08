#include "LVPultecEQProcessor.h"
#include "../../../GUI/Editors/EQ/LVPultecEQEditor.h"
#include "Globals/Globals.h"

namespace viator::dsp::processors
{
    //==============================================================================
    LVPultecEQProcessor::LVPultecEQProcessor(int id)
        : BaseProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        BaseProcessor::setProcessorID(id);
        auto layout = createParameterLayout(id);
        initTreeState(this, std::move(layout));
        m_parameters = std::make_unique<LVPultecEQParameters::parameters>(getTreeState(), id);
        getTreeState().addParameterListener(LVPultecEQParameters::oversamplingChoiceID + juce::String(id), this);
    }

    LVPultecEQProcessor::~LVPultecEQProcessor()
    {
    }

    juce::AudioProcessorValueTreeState::ParameterLayout LVPultecEQProcessor::createParameterLayout(const int id)
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;

        juce::NormalisableRange<float> range(20.0f, 20000.0f, 1.0f);
        range.setSkewForCentre(1000.0f);

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::lowBoostID + juce::String(id), 1},
            LVPultecEQParameters::lowBoostName + juce::String(id),
            0.0f, 13.5f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::lowAttenID + juce::String(id), 1},
            LVPultecEQParameters::lowAttenName + juce::String(id),
            0.0f, 17.5f, 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::highAttenSelID + juce::String(id), 1},
            LVPultecEQParameters::highAttenSelName + juce::String(id),
            5000.0f, 20000.0f, 20000.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::lowFreqID + juce::String(id), 1},
            LVPultecEQParameters::lowFreqName + juce::String(id),
            20.0f, 100.0f, 20.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::highFreqID + juce::String(id), 1},
            LVPultecEQParameters::highFreqName + juce::String(id),
            3000.0f, 16000.0f, 3000.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::highBoostID + juce::String(id), 1},
            LVPultecEQParameters::highBoostName + juce::String(id),
            0.0f, 18.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::highAttenID + juce::String(id), 1},
            LVPultecEQParameters::highAttenName + juce::String(id),
            0.0f, 16.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LVPultecEQParameters::bandwidthID + juce::String(id), 1},
            LVPultecEQParameters::bandwidthName + juce::String(id),
            0.4f, 0.8f, 0.4f));


        const juce::StringArray os_choices = viator::globals::Oversampling::items;
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{LVPultecEQParameters::oversamplingChoiceID + juce::String(id), 1},
            LVPultecEQParameters::oversamplingChoiceName + juce::String(id),
            os_choices, 0));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{LVPultecEQParameters::muteID + juce::String(id), 1},
            LVPultecEQParameters::muteName + juce::String(id),
            false));

        return {params.begin(), params.end()};
    }

    void LVPultecEQProcessor::parameterChanged(const juce::String &parameterID, float newValue)
    {
    }

    //==============================================================================
    const juce::String LVPultecEQProcessor::getName() const
    {
        return "LVPultecEQ";
    }

    bool LVPultecEQProcessor::acceptsMidi() const
    {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool LVPultecEQProcessor::producesMidi() const
    {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool LVPultecEQProcessor::isMidiEffect() const
    {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double LVPultecEQProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int LVPultecEQProcessor::getNumPrograms()
    {
        return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
    }

    int LVPultecEQProcessor::getCurrentProgram()
    {
        return 0;
    }

    void LVPultecEQProcessor::setCurrentProgram(int index)
    {
        juce::ignoreUnused(index);
    }

    const juce::String LVPultecEQProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }

    void LVPultecEQProcessor::changeProgramName(int index, const juce::String &newName)
    {
        juce::ignoreUnused(index, newName);
    }

    void LVPultecEQProcessor::updateParameters()
    {
        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
        {
            m_process_blocks[static_cast<size_t>(oversampling_choice)].updateParameters(*m_parameters);
        }

        const auto should_mute = m_parameters->muteParam->get();

        for (auto &mute: m_mutes)
        {
            mute.setTargetValue(!static_cast<float>(should_mute));
        }
    }

    //==============================================================================
    void LVPultecEQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        // Use this method as the place to do any pre-playback
        // initialisation that you need..
        juce::ignoreUnused(sampleRate, samplesPerBlock);

        for (auto &mute: m_mutes)
        {
            mute.reset(sampleRate, 0.02);
        }

        for (int i = 0; i < m_process_blocks.size(); ++i)
        {
            m_process_blocks[i].prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels(), i);
        }

        m_dry_buffer.setSize(getTotalNumOutputChannels(), sampleRate);
    }

    void LVPultecEQProcessor::releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool LVPultecEQProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

    void LVPultecEQProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                           juce::MidiBuffer &midiMessages)
    {
        juce::ignoreUnused(midiMessages);

        updateParameters();

        m_dry_buffer.clear();
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            m_dry_buffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        }

        calculateInputPeakLevel(buffer);

        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
        {
            m_process_blocks[static_cast<size_t>(oversampling_choice)].process(buffer, buffer.getNumSamples());
        }


        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto *data = buffer.getWritePointer(channel);
            const auto *dry_data = m_dry_buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const auto dry_sample = dry_data[sample];
                const auto wet_sample = data[sample];
                const auto mix = m_mutes[channel].getNextValue();
                data[sample] = (1.0f - mix) * dry_sample + wet_sample * mix;
            }
        }

        calculateOutputPeakLevel(buffer);
    }

    //==============================================================================
    bool LVPultecEQProcessor::hasEditor() const
    {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor *LVPultecEQProcessor::createEditor()
    {
        return new viator::gui::editors::LVPultecEQEditor(*this);
    }
}
