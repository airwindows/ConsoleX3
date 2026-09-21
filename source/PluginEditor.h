#pragma once

#include "AirwindowsUI.h"
#include "PluginProcessor.h"
#include "BinaryData.h"



//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, juce::Slider::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider *) override;
    void sliderDragStarted(juce::Slider *) override;
    void sliderDragEnded(juce::Slider *) override;
    void sliderDragInternal(juce::Slider *, bool started);
    void updateTrackProperties();
    void updatePluginSize();
    void idle();
    
    struct IdleTimer : juce::Timer
        {
            IdleTimer(PluginEditor *ed) : ed(ed) {}
            ~IdleTimer() override = default;
            void timerCallback() override { ed->idle(); }
            PluginEditor *ed;
        };


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef; //quick ref
    
    std::unique_ptr<IdleTimer> idleTimer;
    AirwindowsLookAndFeel airwindowsLookAndFeel;
    AirwindowsMeter meter;
    float aspectRatio = 1.0;
 
    juce::Slider HIGKnob { "High" };
    juce::Slider HMGKnob { "HMid" };
    juce::Slider LMGKnob { "LMid" };
    juce::Slider BSGKnob { "Bass" };
    juce::Slider HIFKnob { "Freq" };
    juce::Slider HMFKnob { "Freq" };
    juce::Slider LMFKnob { "Freq" };
    juce::Slider BSFKnob { "Freq" };
    
    juce::Slider THRKnob { "Threshold" };
    juce::Slider ATKKnob { "Attack" };
    juce::Slider RLSKnob { "Release" };
    juce::Slider RATKnob { "Inv/Wet" };
    
    juce::Slider TRMKnob { "Input" };
    juce::Slider MDEKnob { "Channel | Buss" };
    juce::Slider DISKnob { "Meter Mode" };
    juce::Slider SMOKnob { "Smoothing" };
    juce::Slider MORKnob { "More" };
    
    juce::Slider LOPKnob { "Lowpass" };
    juce::Slider LPQKnob { "Resonance" };
    juce::Slider HIPKnob { "Hipass" };
    juce::Slider HPQKnob { "Resonance" };
    
    juce::Slider PANKnob { "Pan" };
    juce::Slider FADKnob { "Fader" };

    juce::Colour hostTrackColour = juce::Colours::lightgrey;
    juce::String hostTrackName = juce::String();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
