#pragma once

#include "model/Curve.h"

#include <array>

#include <cstdint>

class CurveRecorder {
public:
    void reset() {
        _writeIndex = 0;
        _writeCount = 0;
    }

    bool write(uint32_t relativeTick, uint32_t divisor, float value) {
        bool result = false;

        // record step at the beginning of a step
        // this allows capturing the cv value both at the end of the current step and the start of the next step
        if (relativeTick % divisor == 0) {
            // fill up buffer with current value
            while (_writeIndex <= RecordBufferLength) {
                _buffer[_writeIndex++] = value;
            }

            // record step if recording has been enabled for the duration of the step
            if (_writeCount == divisor) {
                result = true;
            }

            reset();
        }

        uint32_t writeTo = ((relativeTick % divisor) * RecordBufferLength) / divisor;

        while (writeTo >= _writeIndex) {
            _buffer[_writeIndex] = value;
            ++_writeIndex;
        }

        ++_writeCount;

        return result;
    }

    struct Match {
        float min;
        float max;
        Curve::Type type;
    };

    Match matchCurve() const {
        // determine range
        float curveMin = 1.f;
        float curveMax = 0.f;
        for (auto &value : _buffer) {
            curveMin = std::min(curveMin, value);
            curveMax = std::max(curveMax, value);
        }

        // compute squared error between a curve type and the recorded data
        auto curveError = [this, curveMin, curveMax] (Curve::Type type) {
            float error = 0.f;
            float x = 0.f;
            for (uint32_t i = 0; i < RecordBufferLength; ++i) {
                float yn = (_buffer[i] - curveMin) / (curveMax - curveMin);
                float y = Curve::eval(type, x);
                error += (yn - y) * (yn - y);
                x += (1.f / RecordBufferLength);
            }
            return error;
        };

        // determine best curve shape (least error)
        Curve::Type curveType = Curve::Type(0);
        float minError = curveError(curveType);
        for (Curve::Type type = Curve::Type(1); type < Curve::Last; type = Curve::Type(type + 1)) {
            float error = curveError(type);
            if (error < minError) {
                minError = error;
                curveType = type;
            }
        }

        return { curveMin, curveMax, curveType };
    }

private:
    static constexpr uint32_t RecordBufferLength = 16;

    std::array<float, RecordBufferLength + 1> _buffer;
    uint32_t _writeIndex;
    uint32_t _writeCount;
};
