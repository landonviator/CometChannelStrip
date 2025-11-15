//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../DSP/Processors/AmplificationProcessor.h"

namespace viator::gui::editors
{
    class AmplificationEditor : public juce::AudioProcessorEditor
    {
    public:
        explicit AmplificationEditor(viator::dsp::processors::AmplificationProcessor &);

        ~AmplificationEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

    private:
        viator::dsp::processors::AmplificationProcessor &processorRef;
    };
}