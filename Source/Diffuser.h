//
//  Diffuser.h
//  ReverbFun
//
//  Created by Christian Tronhjem on 26.11.24.
//

#ifndef Diffuser_h
#define Diffuser_h
#include "MultiChannelDelayRead.h"
#include "AudioBuffer.h"
#include "Matrices.h"
#include <time.h>

template<int numChannels = 8, int maxDelay=100>
class Diffuser
{
public:
    int shouldInvert[numChannels] {};
    float matrixMix[numChannels] {};
    
    MyDSP::AudioBuffer<float> delayBuffers {numChannels, 44100 * 3};
    MyDSP::MultiChannelDelayRead<float> delays {delayBuffers};
    
    Diffuser()
    {
        srand (static_cast<unsigned int>(time(NULL)));
        
        for (int i = 0; i < numChannels; ++i)
        {
            bool invert = static_cast<bool>(rand() % 2);
            shouldInvert[i] = invert ? -1 : 1;
            matrixMix[i] = 0.f;
        }
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float delay = (static_cast<float>(maxDelay) / static_cast<float>(numChannels)) * channel;
            delays.SetDelayInMs(44100, channel, delay);
        }
    }
    
    void Process(juce::AudioBuffer<float>& buffer, bool shouldClear = false)
    {
        const auto numSamples = buffer.getNumSamples();
        
        for (int i = 0; i < numSamples; ++i)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                delayBuffers.WriteNextSample(channel, channelData[i]);

                matrixMix[channel] = delays.GetNextDelayedSample(channel);
            }
            
            //TODO: change this so etiher matrix doesnt need the pointer or whatever.
            Hadamard<float, numChannels>::inPlace(&matrixMix[0]);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                channelData[i] = matrixMix[channel] * static_cast<float>(shouldInvert[channel]);
            }
        }
    }
};

#endif /* Diffuser_h */
