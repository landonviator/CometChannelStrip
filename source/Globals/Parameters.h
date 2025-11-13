//
// Created by Landon Viator on 11/1/25.
//

#pragma once

namespace viator::parameters
{
    inline const juce::String oversamplingChoiceID = "oversamplingChoiceID";
    inline const juce::String oversamplingChoiceName = "oversamplingChoiceName";

    inline const juce::String consoleDriveID = "consoleDriveID";
    inline const juce::String consoleDriveName = "Console Drive";

    inline const juce::String graphic1ID = "graphic1ID";
    inline const juce::String graphic1Name = "Console Drive";
    inline const juce::String graphic2ID = "graphic2ID";
    inline const juce::String graphic2Name = "Console Drive";
    inline const juce::String graphic3ID = "graphic3ID";
    inline const juce::String graphic3Name = "Console Drive";
    inline const juce::String graphic4ID = "graphic4ID";
    inline const juce::String graphic4Name = "Console Drive";
    inline const juce::String graphic5ID = "graphic5ID";
    inline const juce::String graphic5Name = "Console Drive";
    inline const juce::String graphic6ID = "graphic6ID";
    inline const juce::String graphic6Name = "Console Drive";
    inline const juce::String graphic7ID = "graphic7ID";
    inline const juce::String graphic7Name = "Console Drive";
    inline const juce::String graphic8ID = "graphic8ID";
    inline const juce::String graphic8Name = "Console Drive";
    inline const juce::String graphic9ID = "graphic9ID";
    inline const juce::String graphic9Name = "Console Drive";
    inline const juce::String graphic10ID = "graphic1oID";
    inline const juce::String graphic10Name = "Console Drive";

    struct parameters
    {
        explicit parameters(const juce::AudioProcessorValueTreeState &state)
        {
            oversamplingParam = dynamic_cast<juce::AudioParameterChoice *>(state.getParameter(
                    viator::parameters::oversamplingChoiceID));
            consoleDriveParam = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::consoleDriveID));

            graphic1Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic1ID));
            graphic2Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic2ID));
            graphic3Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic3ID));
            graphic4Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic4ID));
            graphic5Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic5ID));
            graphic6Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic6ID));
            graphic7Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic7ID));
            graphic8Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic8ID));
            graphic9Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic9ID));
            graphic10Param = dynamic_cast<juce::AudioParameterFloat *>(state.getParameter(
                    viator::parameters::graphic10ID));

        }

        juce::AudioParameterChoice *oversamplingParam{nullptr};
        juce::AudioParameterFloat *consoleDriveParam{nullptr};

        juce::AudioParameterFloat *graphic1Param{nullptr};
        juce::AudioParameterFloat *graphic2Param{nullptr};
        juce::AudioParameterFloat *graphic3Param{nullptr};
        juce::AudioParameterFloat *graphic4Param{nullptr};
        juce::AudioParameterFloat *graphic5Param{nullptr};
        juce::AudioParameterFloat *graphic6Param{nullptr};
        juce::AudioParameterFloat *graphic7Param{nullptr};
        juce::AudioParameterFloat *graphic8Param{nullptr};
        juce::AudioParameterFloat *graphic9Param{nullptr};
        juce::AudioParameterFloat *graphic10Param{nullptr};
    };
}
