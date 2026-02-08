//
// Created by Landon Viator on 11/14/25.
//

#pragma once

#include "../../DSP/Processors/BaseProcessor.h"
#include "../Widgets/BaseSlider.h"
#include "../Style/DialLAF.h"
#include "../Style/MacroLAF.h"
#include "../Style/ButtonLAF.h"
#include "../Style/MenuLAF.h"
#include "../Style/Colors.h"
#include "../../Globals/Globals.h"
#include "../Style/Images.h"
#include "../Widgets/LevelMeter.h"

namespace viator::gui::editors
{
    class BaseEditor : public juce::AudioProcessorEditor, public juce::ActionBroadcaster, public juce::Timer {
    public:
        explicit BaseEditor(dsp::processors::BaseProcessor &);

        ~BaseEditor() override;

        //==============================================================================
        void paint(juce::Graphics &) override;

        void resized() override;

        std::vector<widgets::BaseSlider *> &getSliders()
        {
            return m_sliders;
        }

        enum SliderType {
            kInput = 0,
            kOutput,
            kNumSliders
        };

        enum ButtonType {
            kMute = 0,
            kSolo,
            kDelete
        };

        void setBackgroundColor(const juce::Colour colour) { m_comp_bg = colour; repaint(); };

    private:
        dsp::processors::BaseProcessor &processorRef;
        std::vector<widgets::BaseSlider *> m_sliders;

        std::array<juce::Slider, 2> m_io_sliders;
        viator::laf::MacroLAF m_io_laf {1};

        virtual void setSliderProps(juce::Slider &slider);

        juce::ComboBox m_preset_browser, m_oversampling_menu;
        laf::MenuLAF m_menu_laf;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_oversampling_menu_attach;

        virtual void setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items);

        std::array<juce::TextButton, 3> m_buttons;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_mute_attach;
        void setButtonProps(juce::TextButton &button, const juce::String &name);

        juce::Colour m_comp_bg = gui_utils::Colors::light_bg();
        juce::Colour m_widget_bg = juce::Colours::black.withAlpha(0.3f);

        std::unique_ptr<juce::DropShadower> m_drop_shadow;

        viator::laf::ButtonLAF m_button_laf;

        std::array<LevelMeter, kNumSliders> m_input_meters, m_output_meters;

        void timerCallback() override;
    };
}
