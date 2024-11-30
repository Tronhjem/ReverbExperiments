//
//  SingeDelay.h
//  Created by Christian Tronhjem on 08.10.21.
//

#ifndef SingleDelay_h
#define SingleDelay_h

#include "AudioBuffer.h"

namespace MyDSP
{
    template <class SampleType>
    class MultiChannelDelayRead
    {
    public:
        MultiChannelDelayRead(AudioBuffer<SampleType>& buffer) : mBuffer(buffer)
        {
            int channels = buffer.GetNumOfChannels();
            mChannelReadIndex = new int[channels];
        }

        ~MultiChannelDelayRead()
        {
            delete[] mChannelReadIndex;
        }
        
        void SetDelayInMs(int sampleRate, int channel, float milliSeconds)
        {
            float delay = sampleRate * 0.001f * milliSeconds;
            SetDelayInSamples(channel, delay);
        }
        
        void SetDelayInSamples(int channel, float delay)
        {
            int flooredDelayTime = static_cast<int>(floor(delay));
            mDelayFraction = delay - flooredDelayTime;
            int position = mBuffer.GetWriteIndex(channel);
            mChannelReadIndex[channel] = position - flooredDelayTime;
            mChannelReadIndex[channel] &= mBuffer.GetLengthMask();
        }

        SampleType GetNextDelayedSample(USHORT channel)
        {
            float fractionIndex = static_cast<float>(mChannelReadIndex[channel]) + mDelayFraction;
            SampleType sample = mBuffer.GetSample(channel, fractionIndex);
            mChannelReadIndex[channel]++;
            mChannelReadIndex[channel] &= mBuffer.GetLengthMask();
            return sample;
        }
        
    private:
        float mDelayFraction = 0;
        int* mChannelReadIndex;
        AudioBuffer<SampleType>& mBuffer;
    };
}
#endif
