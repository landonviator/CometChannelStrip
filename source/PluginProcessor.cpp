#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), m_tree_state(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    m_parameters = std::make_unique<viator::parameters::parameters>(m_tree_state);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;

    const auto items = viator::globals::Oversampling::items;
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{viator::parameters::oversamplingChoiceID, 1},
                                                                  viator::parameters::oversamplingChoiceName, items, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::consoleDriveID, 1},
                                                                  viator::parameters::consoleDriveName, 0.0f, 100.0f,
                                                                  0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic1ID, 1},
                                                                 viator::parameters::graphic1Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic2ID, 1},
                                                                 viator::parameters::graphic2Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic3ID, 1},
                                                                 viator::parameters::graphic3Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic4ID, 1},
                                                                 viator::parameters::graphic4Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic5ID, 1},
                                                                 viator::parameters::graphic5Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic6ID, 1},
                                                                 viator::parameters::graphic6Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic7ID, 1},
                                                                 viator::parameters::graphic7Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic8ID, 1},
                                                                 viator::parameters::graphic8Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic9ID, 1},
                                                                 viator::parameters::graphic9Name, -15.0f, 15.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::graphic10ID, 1},
                                                                 viator::parameters::graphic10Name, -15.0f, 15.0f,
                                                                 0.0f));


    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{

}

void AudioPluginAudioProcessor::updateParameters()
{
    const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
    if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
    {
        m_process_blocks[static_cast<size_t>(oversampling_choice)].updateParameters(*m_parameters);
    }
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    for (int i = 0; i < m_process_blocks.size(); ++i)
    {
        m_process_blocks[i].prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels(), i);
    }
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    updateParameters();

    juce::ScopedNoDenormals noDenormals;

    const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
    if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
    {
        m_process_blocks[static_cast<size_t>(oversampling_choice)].process(buffer, buffer.getNumSamples());
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    //return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
