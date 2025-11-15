//
// Created by Landon Viator on 11/14/25.
//

#include "ReductionEditor.h"

namespace viator::gui::editors
{
    ReductionEditor::ReductionEditor(viator::dsp::processors::ReductionProcessor &p)
            : AudioProcessorEditor(&p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        setSize(1000, 600);
    }

    ReductionEditor::~ReductionEditor()
    {
    }

//==============================================================================
    void ReductionEditor::paint(juce::Graphics &g)
    {
    }

    void ReductionEditor::resized()
    {
    }
}