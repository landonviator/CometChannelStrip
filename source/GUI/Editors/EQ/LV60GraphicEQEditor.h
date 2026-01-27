//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/EQ/LV60GraphicEQProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"

namespace viator::gui::editors
{
    class LV60GraphicEQEditor : public viator::gui::editors::BaseEditor
    {
    public:
        explicit LV60GraphicEQEditor(viator::dsp::processors::LV60GraphicEQProcessor &);

        ~LV60GraphicEQEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum Sliders {
            kGain1 = 0, kGain2, kGain3, kGain4, kGain5,
            kGain6, kGain7, kGain8, kGain9, kGain10,
            kDrive, kLP, kHP, num_sliders
        };

    private:
        viator::dsp::processors::LV60GraphicEQProcessor &processorRef;

        void setComboBoxProps(juce::ComboBox& box, const juce::StringArray& items);

        std::array<juce::Slider, Sliders::num_sliders> m_main_sliders;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> main_slider_attaches;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_hp_attach, m_lp_attach, m_drive_attach;
        void setSliderProps(juce::Slider& slider);

        viator::gui::laf::DialLAF m_dial_laf;
        viator::gui::laf::MenuLAF m_menu_laf;
    };
}