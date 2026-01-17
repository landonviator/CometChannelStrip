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

    m_processors.clear();
    //addProcessor(viator::dsp::processors::ProcessorType::kClipper);
    //addProcessor(viator::dsp::processors::ProcessorType::kClipper);

    for (int i = 0; i < 10; ++i)
    {
        m_tree_state.addParameterListener("macro" + juce::String(i) + "ID", this);
    }
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

    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro1ID, 1},
                                                                 viator::parameters::macro1Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro2ID, 1},
                                                                 viator::parameters::macro2Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro3ID, 1},
                                                                 viator::parameters::macro3Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro4ID, 1},
                                                                 viator::parameters::macro4Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro5ID, 1},
                                                                 viator::parameters::macro5Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro6ID, 1},
                                                                 viator::parameters::macro6Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro7ID, 1},
                                                                 viator::parameters::macro7Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro8ID, 1},
                                                                 viator::parameters::macro8Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro9ID, 1},
                                                                 viator::parameters::macro9Name, 0.0f, 1.0f,
                                                                 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{viator::parameters::macro10ID, 1},
                                                                 viator::parameters::macro10Name, 0.0f, 1.0f,
                                                                 0.0f));
    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    for (const auto &processor: m_processors)
    {
        if (const auto *module_processor = dynamic_cast<viator::dsp::processors::BaseProcessor *>(processor.get()))
        {
            auto &tree = module_processor->getTreeState();
            if (auto *param = m_tree_state.getParameter(parameterID))
            {
                m_macro_map.update(tree, parameterID, newValue);
            }
        }
    }
}

void AudioPluginAudioProcessor::updateParameters()
{
//    const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
//    if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_process_blocks.size())
//    {
//        m_process_blocks[static_cast<size_t>(oversampling_choice)].updateParameters(*m_parameters);
//    }
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    for (const auto& processor : m_processors)
    {
        if (processor)
        {
            processor->prepareToPlay(sampleRate, samplesPerBlock);
        }
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
    const juce::ScopedTryLock tryLock(m_processor_lock);
    if (!tryLock.isLocked())
        return;

    juce::ignoreUnused (midiMessages);

    updateParameters();

    juce::ScopedNoDenormals noDenormals;

    for (int i = 0; i < m_processors.size(); ++ i)
    {
        if (m_processors[i])
        {
            m_processors[i]->processBlock(buffer, midiMessages);
        }
    }
}

void AudioPluginAudioProcessor::addProcessor(viator::dsp::processors::ProcessorType type)
{
    const juce::ScopedLock lock (m_processor_lock);

    const int index = static_cast<int>(m_processors.size());
    auto processor = viator::dsp::processors::createProcessorByType(type, index);

    if (processor)
    {
        processor->prepareToPlay(getSampleRate(), getBlockSize());
        m_processors.emplace_back(std::move(processor));
    } else
    {
        jassertfalse;
    }

    sendActionMessage(viator::globals::ActionCommands::processorAdded);
}

void AudioPluginAudioProcessor::swapProcessors(const int a, const int b)
{
    const juce::ScopedLock lock (m_processor_lock);

    if (m_processors[a] && m_processors[b])
    {
        std::swap(m_processors[a], m_processors[b]);
    }
}

void AudioPluginAudioProcessor::removeProcessor(const int index)
{
    const juce::ScopedLock lock (m_processor_lock);

    if (m_processors[index])
    {
        m_processors.erase(m_processors.begin() + index);
    }
}

viator::dsp::processors::BaseProcessor* AudioPluginAudioProcessor::getProcessor(int index)
{
    const juce::ScopedLock lock (m_processor_lock);

    if (m_processors[index])
    {
        return m_processors[index].get();
    }

    jassertfalse;
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    DBG("📦 Starting getStateInformation()");

    juce::ValueTree parentTree("PluginState");

    DBG("📦 Processors to save: " << m_processors.size());

    for (int i = 0; i < m_processors.size(); ++i)
    {
        auto *processor = m_processors[i].get();
        juce::MemoryBlock subState;
        processor->getStateInformation(subState);

        if (auto processorTree = juce::ValueTree::readFromData(subState.getData(), subState.getSize()); processorTree.isValid())
        {
            juce::ValueTree wrapper("Processor");

            const auto &registry = viator::dsp::processors::getProcessorRegistry();
            for (const auto &def: registry)
            {
                const auto processor_name = processor->getName();
                const auto name = def.name;
                if (processor->getName() == def.name)
                {
                    wrapper.setProperty("type", def.name, nullptr);
                    wrapper.setProperty("index", i, nullptr);
                    break;
                }
            }

            wrapper.addChild(processorTree, -1, nullptr);
            parentTree.addChild(wrapper, -1, nullptr);

            DBG("✅ Saved processor: " << processor->getName());
        } else
        {
            DBG("❌ Invalid processor state tree at index: " << i);
        }
    }

    // Store parameter state
    auto parameterStateCopy = m_tree_state.copyState();
    parameterStateCopy.removeChild(parameterStateCopy.getChildWithName("Macros"), nullptr);
    parameterStateCopy.appendChild(m_macro_map.saveMacroState(), nullptr);

    parentTree.addChild(parameterStateCopy, -1, nullptr);
    DBG("✅ Parameters added to state");

    DBG("💾 Saving state. parentTree type: " << parentTree.getType().toString()
                                            << ", numChildren: " << parentTree.getNumChildren());
    for (int i = 0; i < parentTree.getNumChildren(); ++i)
    {
        auto child = parentTree.getChild(i);
        DBG("   save child[" << i << "] type = " << child.getType().toString());
    }

    // Serialize to memory
    juce::MemoryOutputStream stream(destData, false);
    parentTree.writeToStream(stream);

    DBG("📦 Finished getStateInformation(), total size: " << destData.getSize() << " bytes");
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto parentTree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (!parentTree.isValid())
    {
        DBG("❌ Invalid parentTree in setStateInformation");
        return;
    }

    DBG("🔍 parentTree type: " << parentTree.getType().toString()
                              << ", numChildren: " << parentTree.getNumChildren());

    m_processors.clear();

    for (int i = 0; i < parentTree.getNumChildren(); ++i)
    {
        auto wrapper = parentTree.getChild(i);

        if (wrapper.hasType(m_tree_state.state.getType()))
        {
            m_tree_state.state = wrapper;
            continue;
        }

        if (!wrapper.hasType("Processor")) continue;

        const auto typeStr = wrapper.getProperty("type").toString();
        const int index = static_cast<int>(wrapper.getProperty("index", 0));

        if (wrapper.getNumChildren() == 0)
            continue;

        auto processorTree = wrapper.getChild(0);
        auto processorType = viator::dsp::processors::processorTypeFromString(typeStr);

        auto processor = createProcessorByType(processorType, index);
        if (processor != nullptr)
        {
            juce::MemoryOutputStream stream;
            processorTree.writeToStream(stream);

            processor->prepareToPlay(getSampleRate(), getBlockSize());
            processor->setStateInformation(stream.getData(), static_cast<int>(stream.getDataSize()));
            m_processors.push_back(std::move(processor));
            sendActionMessage("Loaded");
        } else
        {
            DBG("Failed to create processor of type: " << typeStr);
        }
    }

    const auto macros = m_tree_state.state.getChildWithName("Macros");
    if (macros.isValid())
        m_macro_map.loadMacroState(macros);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
