/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define INTERVAL_BETWEEN_CLICKS 44100 * 2

//==============================================================================
ReverbFunAudioProcessor::ReverbFunAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

ReverbFunAudioProcessor::~ReverbFunAudioProcessor()
{
}

//==============================================================================
const juce::String ReverbFunAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbFunAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbFunAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbFunAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbFunAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbFunAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverbFunAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbFunAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverbFunAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbFunAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverbFunAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ReverbFunAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbFunAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void ReverbFunAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = buffer.getNumChannels();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto numSamples = buffer.getNumSamples();
        const float* sample = buffer.getReadPointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            float sampleToWrite = sample[i] * 0.5f;

            // ==========================================
//            // CLICK SAMPLE FOR TESTING, REMOVE LATER
//            ++sampleCounter[channel];
//            if(sampleCounter[channel] > INTERVAL_BETWEEN_CLICKS)
//            {
//                sampleCounter[channel] = 0;
//                sampleToWrite += 0.25f;
//            }
//            // END CLICK SAMPLE FOR TESTING, REMOVE LATER
            // ==========================================

            for (int j = 0; j < DIFFUSER_CHANNELS; ++j)
            {
                auto* channelData = interrimBuffer.getWritePointer (j);
                channelData[i] = sampleToWrite;
            }
        }
    }
    
    diffuser.Process(interrimBuffer);
    diffuser2.Process(interrimBuffer);
    diffuser3.Process(interrimBuffer);
    diffuser4.Process(interrimBuffer);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* outBuffer = buffer.getWritePointer (channel);
        const auto numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            float sampleToWrite = 0;
            auto* channelData = interrimBuffer.getWritePointer(channel);
            sampleToWrite = channelData[i];

//            for (int j = 0; j < DIFFUSER_CHANNELS; ++j)
//            {
//                auto* channelData = interrimBuffer.getWritePointer(j);
//                sampleToWrite += channelData[i];
//            }

            outBuffer[i] = sampleToWrite;
        }
    }
}

//==============================================================================
bool ReverbFunAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbFunAudioProcessor::createEditor()
{
    return new ReverbFunAudioProcessorEditor (*this);
}

//==============================================================================
void ReverbFunAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReverbFunAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbFunAudioProcessor();
}
