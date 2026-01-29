#include "LV60GraphicEQProcessor.h"
#include "../../../GUI/Editors/EQ/LV60GraphicEQEditor.h"
#include "Globals/Globals.h"

namespace viator::dsp::processors
{
    //==============================================================================
    LV60GraphicEQProcessor::LV60GraphicEQProcessor(int id)
        : BaseProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        BaseProcessor::setProcessorID(id);
        auto layout = createParameterLayout(id);
        initTreeState(this, std::move(layout));
        m_parameters = std::make_unique<LV60GraphicEQParameters::parameters>(getTreeState(), id);
        getTreeState().addParameterListener(LV60GraphicEQParameters::oversamplingChoiceID + juce::String(id), this);
        getTreeState().addParameterListener(LV60GraphicEQParameters::driveID + juce::String(id), this);
    }

    LV60GraphicEQProcessor::~LV60GraphicEQProcessor()
    {
    }

    juce::AudioProcessorValueTreeState::ParameterLayout LV60GraphicEQProcessor::createParameterLayout(const int id)
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;

        juce::NormalisableRange<float> range(20.0f, 20000.0f, 1.0f);
        range.setSkewForCentre(1000.0f);

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LV60GraphicEQParameters::hpCutoffID + juce::String(id), 1},
            LV60GraphicEQParameters::hpCutoffName + juce::String(id),
            range, 20.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LV60GraphicEQParameters::lpCutoffID + juce::String(id), 1},
            LV60GraphicEQParameters::lpCutoffName + juce::String(id),
            range, 20000.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{LV60GraphicEQParameters::driveID + juce::String(id), 1},
            LV60GraphicEQParameters::driveName + juce::String(id),
            0.0f,
            10.0f,
            0.0f));

        const juce::StringArray os_choices = viator::globals::Oversampling::items;
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{LV60GraphicEQParameters::oversamplingChoiceID + juce::String(id), 1},
            LV60GraphicEQParameters::oversamplingChoiceName + juce::String(id),
            os_choices, 0));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{LV60GraphicEQParameters::muteID + juce::String(id), 1},
            LV60GraphicEQParameters::muteName + juce::String(id),
            false));

        for (int i = 0; i < LV60GraphicEQParameters::numBands; ++i)
        {
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{LV60GraphicEQParameters::gainIDs[static_cast<size_t>(i)] + juce::String(id), 1},
                "Gain " + juce::String(i + 1) + juce::String(id),
                -12.0f, 12.0f, 0.0f));
        }

        return {params.begin(), params.end()};
    }

    void LV60GraphicEQProcessor::parameterChanged(const juce::String &parameterID, float newValue)
    {
    }

    //==============================================================================
    const juce::String LV60GraphicEQProcessor::getName() const
    {
        return "LV60GraphicEQ";
    }

    bool LV60GraphicEQProcessor::acceptsMidi() const
    {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool LV60GraphicEQProcessor::producesMidi() const
    {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool LV60GraphicEQProcessor::isMidiEffect() const
    {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double LV60GraphicEQProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int LV60GraphicEQProcessor::getNumPrograms()
    {
        return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
    }

    int LV60GraphicEQProcessor::getCurrentProgram()
    {
        return 0;
    }

    void LV60GraphicEQProcessor::setCurrentProgram(int index)
    {
        juce::ignoreUnused(index);
    }

    const juce::String LV60GraphicEQProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }

    void LV60GraphicEQProcessor::changeProgramName(int index, const juce::String &newName)
    {
        juce::ignoreUnused(index, newName);
    }

    void LV60GraphicEQProcessor::updateParameters()
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
    void LV60GraphicEQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
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

    void LV60GraphicEQProcessor::releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool LV60GraphicEQProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

    void LV60GraphicEQProcessor::processBlock(juce::AudioBuffer<float> &buffer,
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
    }

    //==============================================================================
    bool LV60GraphicEQProcessor::hasEditor() const
    {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor *LV60GraphicEQProcessor::createEditor()
    {
        return new viator::gui::editors::LV60GraphicEQEditor(*this);
    }
}
