#ifndef Matrices_h
#define Matrices_h

// taken from Signalsmith-Audio example code
// https://github.com/Signalsmith-Audio/reverb-example-code/blob/main/mix-matrix.h

#include <cmath>

template<typename Sample, int size>
class Householder {
    static constexpr Sample multiplier{-2.0/size};
public:
    static void inPlace(Sample *arr) {
        double sum = 0;
        for (int i = 0; i < size; ++i) {
            sum += arr[i];
        }

        sum *= multiplier;
        
        for (int i = 0; i < size; ++i) {
            arr[i] += sum;
        }
    };
};

template<typename Sample, int size>
class Hadamard {
public:
    static inline void recursiveUnscaled(Sample * data) {
        if (size <= 1) return;
        constexpr int hSize = size/2;

        // Two (unscaled) Hadamards of half the size
        Hadamard<Sample, hSize>::recursiveUnscaled(data);
        Hadamard<Sample, hSize>::recursiveUnscaled(data + hSize);

        // Combine the two halves using sum/difference
        for (int i = 0; i < hSize; ++i) {
            double a = data[i];
            double b = data[i + hSize];
            data[i] = (a + b);
            data[i + hSize] = (a - b);
        }
    }

    static inline void inPlace(Sample * data) {
        recursiveUnscaled(data);
        
        Sample scalingFactor = std::sqrt(1.0/size);
        for (int c = 0; c < size; ++c) {
            data[c] *= scalingFactor;
        }
    }
};

#endif /* Matrices_h */
