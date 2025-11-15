//
// Created by Landon Viator on 11/14/25.
//

#include "BaseEditor.h"

namespace viator::gui::editors
{
    BaseEditor::BaseEditor(viator::dsp::processors::BaseProcessor &p)
            : AudioProcessorEditor(&p), processorRef(p)
    {
        juce::ignoreUnused(processorRef);

        setSize(1000, 600);
    }

    BaseEditor::~BaseEditor()
    {
    }

//==============================================================================
    void BaseEditor::paint(juce::Graphics &g)
    {
    }

    void BaseEditor::resized()
    {
    }
}