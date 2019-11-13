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
            return _items[index] & 0x7fff;
        }

        void setItem(int index, int value, bool doUpdate = true) {
            _items[index] = (_items[index] & 0x8000) | clamp(value, 0, 0x7fff);
            if (doUpdate) {
                update();
            }
        }

        void editItem(int index, int value, int shift) {
            // inverted to improve usability
            setItem(index, item(index) - value * (shift ? 50 : 1));
        }

        void printItem(int index, StringBuilder &str) const {
            // inverted to improve usability
            if (userDefined(index)) {
                str("%d", 0x7fff - item(index));
            } else {
                str("%d (auto)", 0x7fff - item(index));
            }
        }

        bool userDefined(int index) const {
            return _items[index] & 0x8000;
        }

        void setUserDefined(int index, bool value) {
            _items[index] = (_items[index] & 0x7fff) | (value ? 0x8000 : 0);
            update();
        }

        int defaultItemValue(int index) const {
            // In ideal DAC/OpAmp configuration we get:
            // 0     ->  5.17V
            // 32768 -> -5.25V
            static constexpr float volts0 = 5.17f;
            static constexpr float volts1 = -5.25f;

            float volts = itemToVolts(index);

            return clamp(int((volts - volts0) / (volts1 - volts0) * 32768), 0, 0x7fff);
        }

        uint16_t voltsToValue(float volts) const {
            volts = clamp(volts, float(MinVoltage), float(MaxVoltage));
            float fIndex = (volts - MinVoltage) * ItemsPerVolt;
            int index = std::floor(fIndex);
            if (index < ItemCount - 1) {
                float t = fIndex - index;
                return lerp(t, item(index), item(index + 1));
            } else {
                return item(ItemCount - 1);
            }
        }

        void clear();

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

    private:
        void update();

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

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    CvOutputArray _cvOutputs;
};
