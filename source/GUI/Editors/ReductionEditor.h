//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../DSP/Processors/ReductionProcessor.h"

namespace viator::gui::editors
{
    class ReductionEditor : public juce::AudioProcessorEditor
    {
    public:
        explicit ReductionEditor(viator::dsp::processors::ReductionProcessor &);

        ~ReductionEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

    private:
        viator::dsp::processors::ReductionProcessor &processorRef;
    };
}