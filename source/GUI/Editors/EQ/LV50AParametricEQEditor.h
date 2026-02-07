//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../../DSP/Processors/EQ/LV50AParametricEQProcessor.h"
#include "../BaseEditor.h"
#include "../../Widgets/BaseSlider.h"

namespace viator::gui::editors
{
    class LV50AParametricEQEditor : public viator::gui::editors::BaseEditor {
    public:
        explicit LV50AParametricEQEditor(viator::dsp::processors::LV50AParametricEQProcessor &);

        ~LV50AParametricEQEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        enum Sliders
        {
            kGain1 = 0, kGain2, kGain3, kGain4,
            kQ1, kQ2, kQ3, kQ4,
            kCutoff1, kCutoff2, kCutoff3, kCutoff4,
            kDrive, kLP, kHP,
            num_sliders
        };

    private:
        viator::dsp::processors::LV50AParametricEQProcessor &processorRef;

        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<viator::gui::widgets::BaseSlider, Sliders::num_sliders> m_main_sliders;
        std::array<juce::Label, Sliders::num_sliders> m_main_labels;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> > main_slider_attaches;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_hp_attach, m_lp_attach, m_drive_attach;

        void setSliderProps(juce::Slider &slider);

        void setLabelProps(juce::Label &label);

        void updateLabels();

        viator::laf::DialLAF m_dial_laf;
        viator::gui::laf::MenuLAF m_menu_laf;
    };
}
