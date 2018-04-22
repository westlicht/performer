#pragma once

#include "Config.h"

#include "Serialize.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <array>

class Calibration {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    class CvOutput {
    public:
        static constexpr int MinVoltage = -5;
        static constexpr int MaxVoltage = 5;
        static constexpr int ItemsPerVolt = 1;
        static constexpr int ItemCount = (MaxVoltage - MinVoltage) * ItemsPerVolt + 1;

        typedef std::array<uint16_t, ItemCount> ItemArray;

        // items

        static float itemToVolts(int index) {
            return float(index) / ItemsPerVolt + MinVoltage;
        }

        static void itemName(StringBuilder &str, int index) {
            str("%+.1fV", itemToVolts(index));
        }

        const ItemArray &items() const { return _items; }
              ItemArray &items()       { return _items; }

        int item(int index) const {
            return _items[index];
        }

        void setItem(int index, int value) {
            _items[index] = clamp(value, 0, 0x8000);
        }

        void editItem(int index, int value, int shift) {
            // inverted to improve usability
            setItem(index, item(index) - value * (shift ? 50 : 1));
        }

        void printItem(int index, StringBuilder &str) const {
            // inverted to improve usability
            str("%d%s", 0x8000 - item(index), item(index) == defaultItemValue(index) ? " (default)" : "");
        }

        int defaultItemValue(int index) const {
            // In ideal DAC/OpAmp configuration we get:
            // 0     ->  5.17V
            // 32768 -> -5.25V
            static constexpr float volts0 = 5.17f;
            static constexpr float volts1 = -5.25f;

            float volts = itemToVolts(index);

            return (volts - volts0) / (volts1 - volts0) * 32768;
        }

        uint16_t voltsToValue(float volts) const {
            volts = clamp(volts, float(MinVoltage), float(MaxVoltage));
            float fIndex = (volts - MinVoltage) * ItemsPerVolt;
            int index = std::floor(fIndex);
            if (index < ItemCount - 1) {
                float t = fIndex - index;
                return lerp(t, _items[index], _items[index + 1]);
            } else {
                return _items[ItemCount - 1];
            }
        }

        void autoFill();

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        ItemArray _items;
    };

    typedef std::array<CvOutput, CONFIG_CV_OUTPUT_CHANNELS> CvOutputArray;

    //----------------------------------------
    // cvOutputs
    //----------------------------------------

    const CvOutputArray &cvOutputs() const { return _cvOutputs; }
          CvOutputArray &cvOutputs()       { return _cvOutputs; }

    const CvOutput &cvOutput(int index) const { return _cvOutputs[index]; }
          CvOutput &cvOutput(int index)       { return _cvOutputs[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    CvOutputArray _cvOutputs;
};
