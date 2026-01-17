//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../DSP/Processors/BaseProcessor.h"
#include "../Widgets/BaseSlider.h"
#include "../Style/DialLAF.h"
#include "../Style/MenuLAF.h"
#include "../Style/Colors.h"
#include "../../Globals/Globals.h"

namespace viator::gui::editors
{
    class BaseEditor : public juce::AudioProcessorEditor, public juce::ActionBroadcaster {
    public:
        explicit BaseEditor(viator::dsp::processors::BaseProcessor &);

        ~BaseEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        std::vector<viator::gui::widgets::BaseSlider *> &getSliders()
        {
            return m_sliders;
        }

        enum SliderType {
            kInput = 0,
            kOutput
        };

        enum ButtonType {
            kMute = 0,
            kSolo,
            kDelete
        };

    private:
        viator::dsp::processors::BaseProcessor &processorRef;
        std::vector<viator::gui::widgets::BaseSlider *> m_sliders;

        std::array<juce::Slider, 2> m_io_sliders;

        void setSliderProps(juce::Slider &slider);

        std::array<juce::Label, 2> m_io_labels;

        void setLabelProps(juce::Label &label);

        juce::ComboBox m_preset_browser, m_oversampling_menu;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_oversampling_menu_attach;

        void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<juce::TextButton, 3> m_buttons;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_mute_attach;

        void setButtonProps(juce::TextButton &button, const juce::String &name);

        void showLabelHover();

        void drawHeaderAndFooter(juce::Graphics &g);

        viator::gui::laf::DialLAF m_dial_laf;
        viator::gui::laf::MenuLAF m_menu_laf;
    };
}
