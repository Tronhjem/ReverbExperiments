//
//  AudioBuffer.h
//  Created by Christian Tronhjem on 01.05.21.
//

#ifndef AudioBuffer_h
#define AudioBuffer_h
#include "SonoTronTypes.h"
#include <math.h>
#include "UtilityFunctions.h"
#define DEBUG 1

#if DEBUG
#include <cassert>
#endif

namespace MyDSP
{
///AudioBuffer holds a array of multichannel doubles.
template <class SampleType>
class AudioBuffer
{
public:
    ///Will round size up to nearest power of two.
    AudioBuffer(USHORT numOfChannels, UINT sizeInSamples) :
    mNumOfChannels(numOfChannels)
    {
        mNumOfSamples = NearestPowerOfTwo(sizeInSamples);
        mRingBufferMask = mNumOfSamples - 1;
        mBuffer = new SampleType[mNumOfSamples * numOfChannels] {0};
        mWritePosition = new UINT[numOfChannels] {0};
    }
    
    ~AudioBuffer()
    {
        delete[] mBuffer;
        delete[] mWritePosition;
    }
    
    UINT GetSizeInSamples() const
    {
        return mNumOfSamples;
    }
    
    USHORT GetNumOfChannels()
    {
        return mNumOfChannels;
    }
    
    /// Get Sample at index.
    SampleType GetSample(USHORT channel, int index)
    {
        int realIndex = index & mRingBufferMask;
#if DEBUG
        assert(realIndex >= 0 && realIndex < mNumOfSamples);
        assert(channel < mNumOfChannels);
#endif
        UINT channelIndex = getChannelIndex(channel, realIndex);
        return mBuffer[channelIndex];
    }

    SampleType GetSample(USHORT channel, float index)
    {
        int intIndex = static_cast<int>(floor(index));
        float dif = index - static_cast<float>(intIndex);
        int realIndex = intIndex & mRingBufferMask;

#if DEBUG
        assert(realIndex >= 0 && realIndex < mNumOfSamples);
        assert(dif >= 0.f && dif <= 1.f);
        assert(channel < mNumOfChannels);
#endif
        UINT channelIndex = getChannelIndex(channel, realIndex);
        SampleType sampleA = mBuffer[channelIndex];
        int indexPlusOne = (channelIndex + 1) & mRingBufferMask;
        SampleType sampleB = mBuffer[indexPlusOne];
        return Lerp(sampleA, sampleB, dif);
    }

    /// Set Sample at index.
    /// Be carefull and responsible though, this allows you to be unsafe with the index, to avoid extra checks when not in DEBUG.
    void SetSample(USHORT channel, UINT index, SampleType inSample)
    {
#if DEBUG
        assert(index < mNumOfSamples);
#endif
        UINT channelIndex = getChannelIndex(channel, index);
        mBuffer[channelIndex] = inSample;
    }
    
    /// It allows for writing a block, or get or set individual samples. When using WriteBlock this act as a ring buffer.
    /// Takes a sampleType* and write for the whole size. If end is reached, then it wraps around.
    void WriteBlock(const USHORT channel, const SampleType* const data, const int size)
    {
#if DEBUG
        assert(size < mNumOfSamples * 2);
#endif
        for (int i=0; i< size; ++i)
        {
            WriteNextSample(channel, data[i]);
        }
    }
    
    /// Writes a sample and increments the write position. Wraps around automagically.
    void WriteNextSample(USHORT channel, SampleType sample)
    {
#if DEBUG
        assert(channel < mNumOfChannels);
#endif
        int index = mWritePosition[channel];
        UINT channelIndex = getChannelIndex(channel, index);

#if DEBUG
        assert(channelIndex < (mNumOfSamples * mNumOfChannels));
#endif
        mBuffer[channelIndex] = sample;
        mWritePosition[channel]++;
        
        // Wrap around using the mask.
        mWritePosition[channel] &= mRingBufferMask;
    }
    
    SampleType* GetReadPointer(USHORT channel, UINT index)
    {
#if DEBUG
        assert(index <= mNumOfSamples);
        assert(channel < mNumOfChannels);
#endif
        return mBuffer + getChannelIndex(channel, index);
    }
    
    int GetWriteIndex(USHORT channel)
    {
#if DEBUG
        assert(channel < mNumOfChannels);
#endif
        return mWritePosition[channel];
    }
    
    UINT GetLengthMask()
    {
        return mRingBufferMask;
    }
    
private:
    // to make it explicit we don't have any default constructor.
    AudioBuffer() = delete;
    
    UINT inline getChannelIndex(USHORT channel, UINT index)
    {
        return (channel * mNumOfSamples) + index;
    }
    
    USHORT mNumOfChannels;
    UINT mRingBufferMask;
    UINT mNumOfSamples;
    UINT* mWritePosition;
    SampleType* mBuffer;
};
}
#endif /* AudioBuffer_h */
