//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../DSP/Processors/BaseProcessor.h"

namespace viator::gui::editors
{
    class BaseEditor : public juce::AudioProcessorEditor
    {
    public:
        explicit BaseEditor(viator::dsp::processors::BaseProcessor &);

        ~BaseEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

    private:
        viator::dsp::processors::BaseProcessor &processorRef;
    };
}