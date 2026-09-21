#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
: AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    setResizable(true, true);
    setLookAndFeel(&airwindowsLookAndFeel);
    if (hostTrackColour != juce::Colour()) {
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, hostTrackColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, hostTrackColour);
    } else {
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour);
    }
    updateTrackProperties();
    updatePluginSize();
    
    idleTimer = std::make_unique<IdleTimer>(this);
    idleTimer->startTimer(1000/30); //space between UI screen updates. Larger is slower updates to screen
    
    meter.setOpaque(true);
    meter.resetArrays();
    meter.displayTrackName = hostTrackName;
    meter.displayTrackLetter = hostTrackName.substring(0,1);
    meter.addMouseListener(this, false);
    addAndMakeVisible(meter);
    meter.outputR = 10.0;
    meter.outputG = 10.0;
    meter.outputB = 10.0;
    
    if (airwindowsLookAndFeel.knobMode == 0) {
        HIGKnob.setSliderStyle(juce::Slider::Rotary);
        HMGKnob.setSliderStyle(juce::Slider::Rotary);
        LMGKnob.setSliderStyle(juce::Slider::Rotary);
        BSGKnob.setSliderStyle(juce::Slider::Rotary);
        HIFKnob.setSliderStyle(juce::Slider::Rotary);
        HMFKnob.setSliderStyle(juce::Slider::Rotary);
        LMFKnob.setSliderStyle(juce::Slider::Rotary);
        BSFKnob.setSliderStyle(juce::Slider::Rotary);
        THRKnob.setSliderStyle(juce::Slider::Rotary);
        ATKKnob.setSliderStyle(juce::Slider::Rotary);
        RLSKnob.setSliderStyle(juce::Slider::Rotary);
        RATKnob.setSliderStyle(juce::Slider::Rotary);
        TRMKnob.setSliderStyle(juce::Slider::Rotary);
        MDEKnob.setSliderStyle(juce::Slider::Rotary);
        DISKnob.setSliderStyle(juce::Slider::Rotary);
        SMOKnob.setSliderStyle(juce::Slider::Rotary);
        MORKnob.setSliderStyle(juce::Slider::Rotary);
        LOPKnob.setSliderStyle(juce::Slider::Rotary);
        LPQKnob.setSliderStyle(juce::Slider::Rotary);
        HIPKnob.setSliderStyle(juce::Slider::Rotary);
        HPQKnob.setSliderStyle(juce::Slider::Rotary);
        PANKnob.setSliderStyle(juce::Slider::Rotary);
        FADKnob.setSliderStyle(juce::Slider::Rotary);
    }
    
    if (airwindowsLookAndFeel.knobMode == 1) {
        HIGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        RATKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        TRMKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        MDEKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        DISKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        SMOKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LPQKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HPQKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    }
    
    if (airwindowsLookAndFeel.knobMode == 2) {
        HIGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        RATKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        TRMKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        MDEKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        DISKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        SMOKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LPQKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HPQKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    }
    
    if (airwindowsLookAndFeel.knobMode == 3) {
        HIGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        RATKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        TRMKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        MDEKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        DISKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        SMOKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LPQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HPQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    }
    
    HIGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIGKnob.setRange(0.0f, 1.0f);
    HIGKnob.setValue(processorRef.params[PluginProcessor::KNOBHIG]->get(), juce::NotificationType::dontSendNotification);
    HIGKnob.setDoubleClickReturnValue(true, 0.5);
    HIGKnob.addListener(this);
    addAndMakeVisible(HIGKnob);
    HIGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.8f, 0.152f, 0.164f, 1.0f)); //fixed thumb color red
    
    HMGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HMGKnob.setRange(0.0f, 1.0f);
    HMGKnob.setValue(processorRef.params[PluginProcessor::KNOBHMG]->get(), juce::NotificationType::dontSendNotification);
    HMGKnob.setDoubleClickReturnValue(true, 0.5);
    HMGKnob.addListener(this);
    addAndMakeVisible(HMGKnob);
    HMGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.1f, 0.63f, 0.1f, 1.0f)); //fixed thumb color green
    
    LMGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LMGKnob.setRange(0.0f, 1.0f);
    LMGKnob.setValue(processorRef.params[PluginProcessor::KNOBLMG]->get(), juce::NotificationType::dontSendNotification);
    LMGKnob.setDoubleClickReturnValue(true, 0.5);
    LMGKnob.addListener(this);
    addAndMakeVisible(LMGKnob);
    LMGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.078f, 0.466f, 0.7f, 1.0f)); //fixed thumb color blue
    
    BSGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    BSGKnob.setRange(0.0f, 1.0f);
    BSGKnob.setValue(processorRef.params[PluginProcessor::KNOBBSG]->get(), juce::NotificationType::dontSendNotification);
    BSGKnob.setDoubleClickReturnValue(true, 0.5);
    BSGKnob.addListener(this);
    addAndMakeVisible(BSGKnob);
    BSGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.513f, 0.27f, 0.153f, 1.0f)); //fixed thumb color brown
    
    HIFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIFKnob.setRange(0.0f, 1.0f);
    HIFKnob.setValue(processorRef.params[PluginProcessor::KNOBHIF]->get(), juce::NotificationType::dontSendNotification);
    HIFKnob.setDoubleClickReturnValue(true, 0.5);
    HIFKnob.addListener(this);
    addAndMakeVisible(HIFKnob);
    HIFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.8f, 0.152f, 0.164f, 1.0f)); //fixed thumb color red
    
    HMFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HMFKnob.setRange(0.0f, 1.0f);
    HMFKnob.setValue(processorRef.params[PluginProcessor::KNOBHMF]->get(), juce::NotificationType::dontSendNotification);
    HMFKnob.setDoubleClickReturnValue(true, 0.5);
    HMFKnob.addListener(this);
    addAndMakeVisible(HMFKnob);
    HMFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.1f, 0.63f, 0.1f, 1.0f)); //fixed thumb color green
    
    LMFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LMFKnob.setRange(0.0f, 1.0f);
    LMFKnob.setValue(processorRef.params[PluginProcessor::KNOBLMF]->get(), juce::NotificationType::dontSendNotification);
    LMFKnob.setDoubleClickReturnValue(true, 0.5);
    LMFKnob.addListener(this);
    addAndMakeVisible(LMFKnob);
    LMFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.078f, 0.466f, 0.7f, 1.0f)); //fixed thumb color blue
    
    BSFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    BSFKnob.setRange(0.0f, 1.0f);
    BSFKnob.setValue(processorRef.params[PluginProcessor::KNOBBSF]->get(), juce::NotificationType::dontSendNotification);
    BSFKnob.setDoubleClickReturnValue(true, 0.5);
    BSFKnob.addListener(this);
    addAndMakeVisible(BSFKnob);
    BSFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.513f, 0.27f, 0.153f, 1.0f)); //fixed thumb color brown
    
    THRKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    THRKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    THRKnob.setRange(0.0f, 1.0f);
    THRKnob.setValue(processorRef.params[PluginProcessor::KNOBTHR]->get(), juce::NotificationType::dontSendNotification);
    THRKnob.setDoubleClickReturnValue(true, 1.0);
    THRKnob.addListener(this);
    addAndMakeVisible(THRKnob);
    THRKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //fixed thumb color black for blinkenlight
    
    ATKKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    ATKKnob.setRange(0.0f, 1.0f);
    ATKKnob.setValue(processorRef.params[PluginProcessor::KNOBATK]->get(), juce::NotificationType::dontSendNotification);
    ATKKnob.setDoubleClickReturnValue(true, 0.384);
    ATKKnob.addListener(this);
    addAndMakeVisible(ATKKnob);
    ATKKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.72f, 0.72f, 1.0f)); //fixed thumb color cyan
    
    RLSKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    RLSKnob.setRange(0.0f, 1.0f);
    RLSKnob.setValue(processorRef.params[PluginProcessor::KNOBRLS]->get(), juce::NotificationType::dontSendNotification);
    RLSKnob.setDoubleClickReturnValue(true, 0.618);
    RLSKnob.addListener(this);
    addAndMakeVisible(RLSKnob);
    RLSKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.72f, 0.72f, 1.0f)); //fixed thumb color cyan
    
    RATKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    RATKnob.setRange(0.0f, 1.0f);
    RATKnob.setValue(processorRef.params[PluginProcessor::KNOBRAT]->get(), juce::NotificationType::dontSendNotification);
    RATKnob.setDoubleClickReturnValue(true, 1.0);
    RATKnob.addListener(this);
    addAndMakeVisible(RATKnob);
    RATKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.1f, 0.63f, 0.1f, 1.0f)); //fixed thumb color green
    
    TRMKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    TRMKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    TRMKnob.setRange(0.0f, 1.0f);
    TRMKnob.setValue(processorRef.params[PluginProcessor::KNOBTRM]->get(), juce::NotificationType::dontSendNotification);
    TRMKnob.setDoubleClickReturnValue(true, 0.5);
    TRMKnob.addListener(this);
    addAndMakeVisible(TRMKnob);
    TRMKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //fixed thumb color black for blinkenlight
    
    MDEKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    MDEKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    MDEKnob.setRange(0.0f, 1.0f);
    MDEKnob.setValue(processorRef.params[PluginProcessor::KNOBMDE]->get(), juce::NotificationType::dontSendNotification);
    MDEKnob.setDoubleClickReturnValue(true, 0.5);
    MDEKnob.addListener(this);
    addAndMakeVisible(MDEKnob);
    MDEKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.35f, 0.0f, 0.5f, 1.0f)); //fixed thumb color deep purple
    
    DISKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    DISKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    DISKnob.setRange(0.0f, 1.0f);
    DISKnob.setValue(processorRef.params[PluginProcessor::KNOBDIS]->get(), juce::NotificationType::dontSendNotification);
    DISKnob.setDoubleClickReturnValue(true, 0.25); //this is showing equal parts peak and slew, for preference
    DISKnob.addListener(this);
    addAndMakeVisible(DISKnob);
    DISKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.35f, 0.0f, 0.5f, 1.0f)); //fixed thumb color deep purple
    
    SMOKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    SMOKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    SMOKnob.setRange(0.0f, 1.0f);
    SMOKnob.setValue(processorRef.params[PluginProcessor::KNOBSMO]->get(), juce::NotificationType::dontSendNotification);
    SMOKnob.setDoubleClickReturnValue(true, 0.5);
    SMOKnob.addListener(this);
    addAndMakeVisible(SMOKnob);
    SMOKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.35f, 0.0f, 0.5f, 1.0f)); //fixed thumb color deep purple
    
    MORKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    MORKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    MORKnob.setRange(0.0f, 1.0f);
    MORKnob.setValue(processorRef.params[PluginProcessor::KNOBMOR]->get(), juce::NotificationType::dontSendNotification);
    MORKnob.setDoubleClickReturnValue(true, 0.01);
    MORKnob.addListener(this);
    addAndMakeVisible(MORKnob);
    MORKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //fixed thumb color black for blinkenlight
    
    PANKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    PANKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    PANKnob.setRange(0.0f, 1.0f);
    PANKnob.setValue(processorRef.params[PluginProcessor::KNOBPAN]->get(), juce::NotificationType::dontSendNotification);
    PANKnob.setDoubleClickReturnValue(true, 0.5);
    PANKnob.addListener(this);
    addAndMakeVisible(PANKnob);
    PANKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.35f, 0.0f, 0.5f, 1.0f)); //fixed thumb color deep purple
    
    LOPKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LOPKnob.setRange(0.0f, 1.0f);
    LOPKnob.setValue(processorRef.params[PluginProcessor::KNOBLOP]->get(), juce::NotificationType::dontSendNotification);
    LOPKnob.setDoubleClickReturnValue(true, 1.0);
    LOPKnob.addListener(this);
    addAndMakeVisible(LOPKnob);
    LOPKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.91f, 0.93f, 0.0f, 1.0f)); //fixed thumb color yellow
    
    LPQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LPQKnob.setRange(0.0f, 1.0f);
    LPQKnob.setValue(processorRef.params[PluginProcessor::KNOBLPQ]->get(), juce::NotificationType::dontSendNotification);
    LPQKnob.setDoubleClickReturnValue(true, 0.0);
    LPQKnob.addListener(this);
    addAndMakeVisible(LPQKnob);
    LPQKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.91f, 0.93f, 0.0f, 1.0f)); //fixed thumb color yellow
    
    HIPKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIPKnob.setRange(0.0f, 1.0f);
    HIPKnob.setValue(processorRef.params[PluginProcessor::KNOBHIP]->get(), juce::NotificationType::dontSendNotification);
    HIPKnob.setDoubleClickReturnValue(true, 0.0);
    HIPKnob.addListener(this);
    addAndMakeVisible(HIPKnob);
    HIPKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.91f, 0.93f, 0.0f, 1.0f)); //fixed thumb color yellow
    
    HPQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HPQKnob.setRange(0.0f, 1.0f);
    HPQKnob.setValue(processorRef.params[PluginProcessor::KNOBHPQ]->get(), juce::NotificationType::dontSendNotification);
    HPQKnob.setDoubleClickReturnValue(true, 0.0);
    HPQKnob.addListener(this);
    addAndMakeVisible(HPQKnob);
    HPQKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.91f, 0.93f, 0.0f, 1.0f)); //fixed thumb color yellow
    
    FADKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    FADKnob.setRange(0.0f, 1.0f);
    FADKnob.setValue(processorRef.params[PluginProcessor::KNOBFAD]->get(), juce::NotificationType::dontSendNotification);
    FADKnob.setDoubleClickReturnValue(true, 1.0);
    FADKnob.addListener(this);
    addAndMakeVisible(FADKnob);
    FADKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //fixed thumb color black for blinkenlight
    
    setSize (airwindowsLookAndFeel.userWidth, airwindowsLookAndFeel.userHeight);
    // Make sure that before the constructor has finished, you've set the editor's size to whatever you need it to be.
}

PluginEditor::~PluginEditor(){
    setLookAndFeel(nullptr); //we do need to set the look and feel to null pointer
}

void PluginEditor::paint (juce::Graphics& g)
{
    if (airwindowsLookAndFeel.alfInterpolation == 0) g.setImageResamplingQuality(g.lowResamplingQuality);
    if (airwindowsLookAndFeel.alfInterpolation == 1) g.setImageResamplingQuality(g.mediumResamplingQuality);
    if (airwindowsLookAndFeel.alfInterpolation == 2) g.setImageResamplingQuality(g.highResamplingQuality); //copy this section to other stretched-bitmap meters
    if (airwindowsLookAndFeel.blurImage == juce::Image()) { // == means image is BLANK so we do a color
        if (hostTrackColour != juce::Colour()) {
            g.setFillType(juce::FillType(hostTrackColour)); g.setOpacity(airwindowsLookAndFeel.applyTrackColour);
        }
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour.interpolatedWith (hostTrackColour, airwindowsLookAndFeel.applyTrackColour));
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour.interpolatedWith (hostTrackColour, airwindowsLookAndFeel.applyTrackColour));
        if (airwindowsLookAndFeel.useToneColor) {
            g.fillAll (meter.backdropColour);
            airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, meter.backdropColour);
            airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, meter.backdropColour);
        } else {
            g.fillAll (airwindowsLookAndFeel.defaultColour);
        }
    } else { //we have an image that might be named and actual size, or might be the overall tiled texture you can use
        if (airwindowsLookAndFeel.usingNamedImage) {
            g.drawImageWithin(airwindowsLookAndFeel.backgroundImage, 0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight(), 0);
        } else {
            g.setTiledImageFill(airwindowsLookAndFeel.backgroundImage, 0, 0, 1.0f); g.fillAll();
        }
        
        if (hostTrackColour != juce::Colour()) {
            g.setFillType(juce::FillType(hostTrackColour)); g.setOpacity(airwindowsLookAndFeel.applyTrackColour); g.fillAll();
        }
        airwindowsLookAndFeel.defaultColour = juce::Colour::fromRGBA(airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getRed(),airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getGreen(),airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getBlue(),1.0);
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour);
    } //find the color of the background tile or image, if there is one. Please use low-contrast stuff, but I'm not your mom :)
    
    g.setFont(juce::FontOptions(airwindowsLookAndFeel.newFont, g.getCurrentFont().getHeight(), 0));
    if ((hostTrackName == juce::String()) || (hostTrackName.length() < 1.0f)) hostTrackName = juce::String("ConsoleX3");
    meter.displayTrackName = hostTrackName; //if not track name, then name of plugin. To be displayed on the actual peakmeter
    meter.displayTrackLetter = hostTrackName.substring(0,1);
    meter.displayFont = airwindowsLookAndFeel.newFont; //in the custom font, if we're using one
    
    float whiteLevel = (findColour(juce::ResizableWindow::backgroundColourId).getLightness()+0.37f)*0.618f; //apply about half of this
    float blackLevel = 1.0f-whiteLevel;
    whiteLevel *= whiteLevel;
    blackLevel *= blackLevel;
    
    auto area = getLocalBounds(); // this is a huge huge routine, but not all of it runs at all times!
    auto linewidth = (int)fmin(area.getHeight(),area.getWidth());
    linewidth = (int)cbrt(linewidth/2)-1;
    area.reduce((int)(linewidth*1.618f), (int)(linewidth*1.618f));
    if (area.getWidth() < 161.8f || area.getHeight() < 161.8f) {
        meter.drawInfo = false;
        g.setColour(juce::Colours::black); g.setOpacity(1.0f);
        g.drawRect(meter.getX(), meter.getY(), area.getWidth(), area.getHeight());
        //do not draw embossed areas if it's meter-only
    } else {
        meter.drawInfo = true;
        auto meterBorder = area;
        meterBorder.removeFromBottom(area.getHeight()-(meter.displayHeight+(linewidth*2)));
        g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
        g.fillRect(meterBorder.getTopLeft().x+linewidth, meterBorder.getTopLeft().y, meterBorder.getWidth()-(linewidth*3), linewidth);
        g.fillRect(meterBorder.getTopLeft().x, meterBorder.getTopLeft().y, linewidth, meterBorder.getHeight()-linewidth);
        g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
        g.fillRect(meterBorder.getTopRight().x-(linewidth*2), meterBorder.getTopRight().y+linewidth, linewidth, meterBorder.getHeight()-linewidth);
        g.fillRect(meterBorder.getBottomLeft().x+linewidth, meterBorder.getBottomLeft().y-linewidth, meterBorder.getWidth()-(linewidth*3), linewidth);
        area.removeFromTop(meter.displayHeight+(linewidth*3)); //remaining area is for controls. getProportion sets first start X and Y placement, then size X and Y placement
        area.removeFromBottom(linewidth);
        float skew = airwindowsLookAndFeel.applyTilt; //this is the amount of tilt the knobs experience at top and bottom. MAX 0.5 becomes full tilt
        if (aspectRatio >= 0.0f && aspectRatio < 0.25f+(0.25f*skew)) { //8h-1w
            auto eqPane = area;
            eqPane.removeFromBottom((area.getHeight()/2)-linewidth);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(eqPane.getTopLeft().x+linewidth, eqPane.getTopLeft().y, eqPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(eqPane.getTopLeft().x, eqPane.getTopLeft().y, linewidth, eqPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(eqPane.getTopRight().x-(linewidth*2), eqPane.getTopRight().y+linewidth, linewidth, eqPane.getHeight()-linewidth);
            g.fillRect(eqPane.getBottomLeft().x+linewidth, eqPane.getBottomLeft().y-linewidth, eqPane.getWidth()-(linewidth*3), linewidth);
            auto fadPane = area;
            fadPane.removeFromTop((area.getHeight()/2)+(linewidth*2));
            auto compPane = fadPane;
            compPane.removeFromBottom(((fadPane.getHeight()*3)/4)+(linewidth*3));
            fadPane.removeFromTop((fadPane.getHeight()/4)-(linewidth*2));
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(compPane.getTopLeft().x+linewidth, compPane.getTopLeft().y, compPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(compPane.getTopLeft().x, compPane.getTopLeft().y, linewidth, compPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(compPane.getTopRight().x-(linewidth*2), compPane.getTopRight().y+linewidth, linewidth, compPane.getHeight()-linewidth);
            g.fillRect(compPane.getBottomLeft().x+linewidth, compPane.getBottomLeft().y-linewidth, compPane.getWidth()-(linewidth*3), linewidth);
            
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(fadPane.getTopLeft().x+linewidth, fadPane.getTopLeft().y, fadPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(fadPane.getTopLeft().x, fadPane.getTopLeft().y, linewidth, fadPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(fadPane.getTopRight().x-(linewidth*2), fadPane.getTopRight().y+linewidth, linewidth, fadPane.getHeight()-linewidth);
            g.fillRect(fadPane.getBottomLeft().x+linewidth, fadPane.getBottomLeft().y-linewidth, fadPane.getWidth()-(linewidth*3), linewidth);
        }
        if (aspectRatio >= 0.25f+(0.25f*skew) && aspectRatio < 1.0f+(1.0f*skew)) { //4h-2w
            auto eqPane = area;
            eqPane.removeFromRight((area.getWidth()/2)+(linewidth*2));
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(eqPane.getTopLeft().x+linewidth, eqPane.getTopLeft().y, eqPane.getWidth()-(linewidth*2), linewidth);
            g.fillRect(eqPane.getTopLeft().x, eqPane.getTopLeft().y, linewidth, eqPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(eqPane.getTopRight().x-linewidth, eqPane.getTopRight().y+linewidth, linewidth, eqPane.getHeight()-linewidth);
            g.fillRect(eqPane.getBottomLeft().x+linewidth, eqPane.getBottomLeft().y-linewidth, eqPane.getWidth()-(linewidth*2),linewidth);
            auto fadPane = area;
            fadPane.removeFromLeft((area.getWidth()/2)-linewidth);
            auto compPane = fadPane;
            compPane.removeFromBottom(((fadPane.getHeight()*3)/4));
            fadPane.removeFromTop((fadPane.getHeight()/4)+linewidth);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(compPane.getTopLeft().x+linewidth, compPane.getTopLeft().y, compPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(compPane.getTopLeft().x, compPane.getTopLeft().y, linewidth, compPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(compPane.getTopRight().x-(linewidth*2), compPane.getTopRight().y+linewidth, linewidth, compPane.getHeight()-linewidth);
            g.fillRect(compPane.getBottomLeft().x+linewidth, compPane.getBottomLeft().y-linewidth, compPane.getWidth()-(linewidth*3), linewidth);
            
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(fadPane.getTopLeft().x+linewidth, fadPane.getTopLeft().y, fadPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(fadPane.getTopLeft().x, fadPane.getTopLeft().y, linewidth, fadPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(fadPane.getTopRight().x-(linewidth*2), fadPane.getTopRight().y+linewidth, linewidth, fadPane.getHeight()-linewidth);
            g.fillRect(fadPane.getBottomLeft().x+linewidth, fadPane.getBottomLeft().y-linewidth, fadPane.getWidth()-(linewidth*3), linewidth);
        }
        if (aspectRatio >= 1.0f+(1.0f*skew) && aspectRatio < 4.0f+(4.0f*skew)) { //2h-4w
            auto eqPane = area;
            eqPane.removeFromBottom((area.getHeight()/2)+linewidth);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(eqPane.getTopLeft().x+linewidth, eqPane.getTopLeft().y, eqPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(eqPane.getTopLeft().x, eqPane.getTopLeft().y, linewidth, eqPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(eqPane.getTopRight().x-(linewidth*2), eqPane.getTopRight().y+linewidth, linewidth, eqPane.getHeight()-linewidth);
            g.fillRect(eqPane.getBottomLeft().x+linewidth, eqPane.getBottomLeft().y-linewidth, eqPane.getWidth()-(linewidth*3), linewidth);
            auto fadPane = area;
            fadPane.removeFromTop(area.getHeight()/2);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(fadPane.getTopLeft().x+linewidth, fadPane.getTopLeft().y, fadPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(fadPane.getTopLeft().x, fadPane.getTopLeft().y, linewidth, fadPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(fadPane.getTopRight().x-(linewidth*2), fadPane.getTopRight().y+linewidth, linewidth, fadPane.getHeight()-linewidth);
            g.fillRect(fadPane.getBottomLeft().x+linewidth, fadPane.getBottomLeft().y-linewidth, fadPane.getWidth()-(linewidth*3), linewidth);
        }
        if (aspectRatio >= 4.0f+(4.0f*skew) && aspectRatio < 999999.0f) { //1h-8w
            auto eqPane = area;
            eqPane.removeFromRight((area.getWidth()/2)+linewidth);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(eqPane.getTopLeft().x+linewidth, eqPane.getTopLeft().y, eqPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(eqPane.getTopLeft().x, eqPane.getTopLeft().y, linewidth, eqPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(eqPane.getTopRight().x-(linewidth*2), eqPane.getTopRight().y+linewidth, linewidth, eqPane.getHeight()-linewidth);
            g.fillRect(eqPane.getBottomLeft().x+linewidth, eqPane.getBottomLeft().y-linewidth, eqPane.getWidth()-(linewidth*3), linewidth);
            auto fadPane = area;
            fadPane.removeFromLeft((area.getWidth()/2)-linewidth);
            g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
            g.fillRect(fadPane.getTopLeft().x+linewidth, fadPane.getTopLeft().y, fadPane.getWidth()-(linewidth*3), linewidth);
            g.fillRect(fadPane.getTopLeft().x, fadPane.getTopLeft().y, linewidth, fadPane.getHeight()-linewidth);
            g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
            g.fillRect(fadPane.getTopRight().x-(linewidth*2), fadPane.getTopRight().y+linewidth, linewidth, fadPane.getHeight()-linewidth);
            g.fillRect(fadPane.getBottomLeft().x+linewidth, fadPane.getBottomLeft().y-linewidth, fadPane.getWidth()-(linewidth*3), linewidth);
        }
    }
    g.setColour(juce::Colours::white); g.setOpacity(whiteLevel);
    g.fillRect(0, 0, getLocalBounds().getWidth()-linewidth, linewidth);
    g.fillRect(0, linewidth, linewidth, getLocalBounds().getHeight()-(linewidth*2));
    g.setColour(juce::Colours::black); g.setOpacity(blackLevel);
    g.fillRect(linewidth, getLocalBounds().getHeight()-linewidth, getLocalBounds().getWidth(), linewidth);
    g.fillRect(getLocalBounds().getWidth()-linewidth, linewidth, linewidth, getLocalBounds().getHeight()-linewidth);
    
    g.setColour (juce::Colours::black);
    g.setOpacity(0.384f); g.drawRect(2, 2, getLocalBounds().getWidth()-4, getLocalBounds().getHeight()-4);
    g.setOpacity(0.618f); g.drawRect(1, 1, getLocalBounds().getWidth()-2, getLocalBounds().getHeight()-2);
    g.setOpacity(1.000f); g.drawRect(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
    //draw global bevel effect, either from the color or from the color of the blurred texture, and a black border
}

void PluginEditor::resized()
{
    auto area = getLocalBounds(); // this is a huge huge routine, but not all of it runs at all times!
    processorRef.pluginWidth = airwindowsLookAndFeel.userWidth = area.getWidth();
    processorRef.pluginHeight = airwindowsLookAndFeel.userHeight = area.getHeight();
    auto linewidth = (int)fmin(area.getHeight(),area.getWidth());
    linewidth = (int)cbrt(linewidth/2)-1;
    area.reduce((int)(linewidth*1.618f), (int)(linewidth*1.618f));
    if (area.getWidth() < 161.8f || area.getHeight() < 161.8f) {
        meter.displayWidth = (int)area.getWidth();
        meter.displayHeight = (int)area.getHeight(); //meter-only display for tiny windows
        meter.setBounds(area);
        int step = area.getWidth()+1;
        area.translate(step, area.getHeight()+1);         //move all controls offscreen diagonally
        HIFKnob.setBounds(area); area.translate(step, 0); //and make them not overlap even while offscreen
        HIGKnob.setBounds(area); area.translate(step, 0);
        HMFKnob.setBounds(area); area.translate(step, 0);
        HMGKnob.setBounds(area); area.translate(step, 0);
        LMFKnob.setBounds(area); area.translate(step, 0);
        LMGKnob.setBounds(area); area.translate(step, 0);
        BSFKnob.setBounds(area); area.translate(step, 0);
        BSGKnob.setBounds(area); area.translate(step, 0);
        THRKnob.setBounds(area); area.translate(step, 0);
        ATKKnob.setBounds(area); area.translate(step, 0);
        RLSKnob.setBounds(area); area.translate(step, 0);
        RATKnob.setBounds(area); area.translate(step, 0);
        HIPKnob.setBounds(area); area.translate(step, 0);
        LOPKnob.setBounds(area); area.translate(step, 0);
        HPQKnob.setBounds(area); area.translate(step, 0);
        LPQKnob.setBounds(area); area.translate(step, 0);
        TRMKnob.setBounds(area); area.translate(step, 0);
        MDEKnob.setBounds(area); area.translate(step, 0);
        DISKnob.setBounds(area); area.translate(step, 0);
        SMOKnob.setBounds(area); area.translate(step, 0);
        MORKnob.setBounds(area); area.translate(step, 0);
        PANKnob.setBounds(area); area.translate(step, 0);
        FADKnob.setBounds(area); area.translate(step, 0);
    } else {
        meter.displayWidth = (int)area.getWidth();
        meter.displayHeight = (int)(area.getHeight()*0.3819661f); //meter is inverse, controls are golden ratio
        if (meter.displayHeight > (float)meter.displayWidth*0.618f) meter.displayHeight = (int)(meter.displayWidth*0.618f);
        meter.displayWidth -= (linewidth*3);
        meter.displayHeight -= (linewidth*3);
        meter.setBounds(area.getX()+linewidth,area.getY()+linewidth,meter.displayWidth,meter.displayHeight);
        area.removeFromTop(meter.displayHeight+(linewidth*3)); //remaining area is for controls. getProportion sets first start X and Y placement, then size X and Y placement
        area.removeFromBottom(linewidth);
        aspectRatio = (float)area.getWidth() / (float)area.getHeight(); //Larger than 1: horisontal. Smaller than 1: vertical
        float skew = airwindowsLookAndFeel.applyTilt; //this is the amount of tilt the knobs experience at top and bottom. MAX 0.5 becomes full tilt
        
        // 12h-1w = 0.11  (0.26)  6h-2w = 0.41   4h-3w = 1.0    3h-4w = 1.8    2h-6w = 3.85     1h-12w = 15.42
        float lilKnob = 0.34f; float bigKnob = 0.66f; float tripleKnob = 0.30f; float medKnob = 0.42f;  float hugeKnob = 0.88f;
        float sliderW = 0.9f; float sliderN = 0.64f; float sliderH = 0.23f; float sliderS = 0.18f; //these are knob sizes scaled to the size of the block
        //these are always the same for all aspect ratios, but panels and offsets are unique to the aspect ratios, as are the sequence of control blocks
        
        float A1x = 0.08f; float A2x = 0.55f; float A1y = 0.18f; float A2y = 0.62f; //A is four medium knobs in a box
        float B1x = 0.04f; float B1y = 0.3f; float B2x = 0.54f; float B2y = 0.05f; //B is big and small knob above right
        float C1x = 0.06f; float C2x = 0.2f; float C1y = 0.03f; float C2y = 0.2f; float C3y = 0.35f; float C4y = 0.51f; float C5y = 0.64f;  float C6y = 0.8f; //C is the six slider bank, evenly spaced
        float D1x = 0.06f; float D1y = 0.22f; float D2x = 0.05f; float D3x = 0.36f; float D4x = 0.67f; float D234y = 0.52f; //D is one slider with three knobs under it:
        float S1x = 0.04f; float S1y = 0.08f; //S is single knob
        
        if (aspectRatio >= 0.0f && aspectRatio < 0.25f+(0.25f*skew)) { //8h-1w
            float pX = (float)area.getWidth()/1.0f;
            float offsetX = 0.0f;
            float pY = (float)area.getHeight()/8.0f; //size of each movable block
            float offsetY = 0.0f;
            float panelMax = skew*sqrt(fmaxf((pX/pY)-0.8f,0.0f));
            //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
            HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-1.0f), 0.0f, 0.0f, 1.0f));
            HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
            
            HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.9f), 0.0f, 0.0f, 1.0f));
            HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY = pY; //put between vertical SECTIONS (not knobs)
            
            HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
            
            HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.7f), 0.0f, 0.0f, 1.0f));
            HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY = pY*2.0f; //put between vertical SECTIONS (not knobs)
            
            LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.6f), 0.0f, 0.0f, 1.0f));
            LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
            
            LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.5f), 0.0f, 0.0f, 1.0f));
            LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY = pY*3.0f; //put between vertical SECTIONS (not knobs)
            
            BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
            
            BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.3f), 0.0f, 0.0f, 1.0f));
            BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY = pY*4.0f; //put between vertical SECTIONS (not knobs)
            
            THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            
            ATKKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.4f), 0.0f, 0.0f, 1.0f));
            ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RLSKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.4f), 0.0f, 0.0f, 1.0f));
            RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D3x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RATKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.4f), 0.0f, 0.0f, 1.0f));
            RATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D4x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Dynamics D block
            offsetY = pY*5.0f; //put between vertical SECTIONS (not knobs)
            
            HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            
            HPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.6f), 0.0f, 0.0f, 1.0f));
            HPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()}));
            LPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.6f), 0.0f, 0.0f, 1.0f));
            LPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()})); //LowpassHighpass A block
            offsetY = pY*6.0f; //put between vertical SECTIONS (not knobs)
            
            TRMKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            MDEKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            DISKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            SMOKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            MORKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C5y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C6y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //TrimEtc C block
            offsetY = pY*7.0f; //put between vertical SECTIONS (not knobs)
            
            FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*S1x))/area.getWidth(), (offsetY+(pY*S1y))/area.getHeight(), (pX*hugeKnob)/area.getWidth(), (pY*hugeKnob)/area.getHeight()}));
        } //8h-1w
        
        if (aspectRatio >= 0.25f+(0.25f*skew) && aspectRatio < 1.0f+(1.0f*skew)) { //4h-2w
            float pX = (float)area.getWidth()*0.5f;
            float offsetX = 0.0f;
            float pY = (float)area.getHeight()/4.0f;
            float offsetY = 0.0f;
            float panelMax = skew*sqrt(fmaxf((pX/pY)-0.8f,0.0f));
            //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
            HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.9f), 0.0f, 0.0f, 1.0f));
            HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
            
            HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.7f), 0.0f, 0.0f, 1.0f));
            HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
            
            HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.1f), 0.0f, 0.0f, 1.0f));
            HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.1f), 0.0f, 0.0f, 1.0f));
            LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
            
            LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.4f), 0.0f, 0.0f, 1.0f));
            LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.7f), 0.0f, 0.0f, 1.0f));
            BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
            
            BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.9f), 0.0f, 0.0f, 1.0f));
            BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            
            offsetX += pX; //put between vertical SECTIONS (not knobs)
            offsetY = 0.0f; //put between horizontal SECTIONS (not knobs)
            
            THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            
            ATKKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.7f), 0.0f, 0.0f, 1.0f));
            ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RLSKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.7f), 0.0f, 0.0f, 1.0f));
            RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D3x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RATKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.7f), 0.0f, 0.0f, 1.0f));
            RATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D4x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Dynamics D block
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            
            HPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.1f), 0.0f, 0.0f, 1.0f));
            HPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()}));
            LPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.1f), 0.0f, 0.0f, 1.0f));
            LPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()})); //LowpassHighpass A block
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            TRMKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            MDEKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            DISKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            SMOKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            MORKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C5y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C6y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //TrimEtc C block
            offsetY += pY; //put between vertical SECTIONS (not knobs)
            
            FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*S1x))/area.getWidth(), (offsetY+(pY*S1y))/area.getHeight(), (pX*hugeKnob)/area.getWidth(), (pY*hugeKnob)/area.getHeight()}));
        } //4h-2w
        
        if (aspectRatio >= 1.0f+(1.0f*skew) && aspectRatio < 4.0f+(4.0f*skew)) { //2h-4w
            float pX = (float)area.getWidth()/4.0f; //top bank is 4 knobs wide
            float offsetX = 0.0f;
            float pY = (float)area.getHeight()*0.5f;
            float offsetY = 0.0f;
            float panelMax = skew*sqrt(fmaxf((pX/pY)-0.8f,0.0f));
            //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
            BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
            
            BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
            
            LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
            
            HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
            
            HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.4f), 0.0f, 0.0f, 1.0f));
            HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            
            offsetY += pY; //begin where top section stops
            offsetX = 0.0f; //put between vertical SECTIONS (not knobs)
            
            THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            
            ATKKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RLSKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D3x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RATKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            RATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D4x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Dynamics D block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.3f), 0.0f, 0.0f, 1.0f));
            HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.3f), 0.0f, 0.0f, 1.0f));
            LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            
            HPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            HPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()}));
            LPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.8f), 0.0f, 0.0f, 1.0f));
            LPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()})); //LowpassHighpass A block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            TRMKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            MDEKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            DISKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            SMOKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            MORKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C5y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C6y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //TrimEtc C block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*S1x))/area.getWidth(), (offsetY+(pY*S1y))/area.getHeight(), (pX*hugeKnob)/area.getWidth(), (pY*hugeKnob)/area.getHeight()}));
        } //2h-4w
        
        if (aspectRatio >= 4.0f+(4.0f*skew) && aspectRatio < 999999.0f) { //1h-8w
            float pX = (float)area.getWidth()/8.0f;
            float offsetX = 0.0f;
            float pY = (float)area.getHeight()/1.0f; //size of each movable block
            float offsetY = 0.0f;
            float panelMax = skew*sqrt(fmaxf((pX/pY)-0.8f,0.0f));
            //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
            BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
            
            BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
            
            LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
            
            HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.8f), 0.0f, 0.0f, 1.0f));
            HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
            
            HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            
            ATKKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RLSKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D3x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()}));
            RATKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.5f), 0.0f, 0.0f, 1.0f));
            RATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D4x))/area.getWidth(), (offsetY+(pY*D234y))/area.getHeight(), (pX*tripleKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Dynamics D block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.5f), 0.0f, 0.0f, 1.0f));
            HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*-0.5f), 0.0f, 0.0f, 1.0f));
            LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A1y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*medKnob)/area.getHeight()}));
            
            HPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.6f), 0.0f, 0.0f, 1.0f));
            HPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A1x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()}));
            LPQKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.6f), 0.0f, 0.0f, 1.0f));
            LPQKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*A2x))/area.getWidth(), (offsetY+(pY*A2y))/area.getHeight(), (pX*medKnob)/area.getWidth(), (pY*tripleKnob)/area.getHeight()})); //LowpassHighpass A block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            TRMKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            MDEKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            DISKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            SMOKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C2x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderN)/area.getWidth(), (pY*sliderS)/area.getHeight()}));
            MORKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C5y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
            PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1x))/area.getWidth(), (offsetY+(pY*C6y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //TrimEtc C block
            offsetX += pX; //put between horizontal SECTIONS (not knobs)
            
            FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(0.5f+(panelMax*0.3f), 0.0f, 0.0f, 1.0f));
            FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*S1x))/area.getWidth(), (offsetY+(pY*S1y))/area.getHeight(), (pX*hugeKnob)/area.getWidth(), (pY*hugeKnob)/area.getHeight()}));
        } //1h-8w
    } //above only draws when size is not meter-only size
}

void PluginEditor::sliderValueChanged(juce::Slider *s)
{
    if (s == &HIGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &THRKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTHR;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &ATKKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBATK;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RLSKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRLS;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RATKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRAT;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &TRMKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTRM;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MDEKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMDE;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &DISKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBDIS;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &SMOKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBSMO;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MORKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMOR;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LOPKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLOP;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LPQKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLPQ;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIPKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIP;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HPQKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHPQ;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &PANKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBPAN;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &FADKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBFAD;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
}

void PluginEditor::sliderDragStarted(juce::Slider *s) {sliderDragInternal(s, true);}
void PluginEditor::sliderDragEnded(juce::Slider *s) {sliderDragInternal(s, false);}
void PluginEditor::sliderDragInternal(juce::Slider *s, bool bv)
{
    if (s == &HIGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &THRKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTHR;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &ATKKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBATK;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RLSKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRLS;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RATKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRAT;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &TRMKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTRM;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MDEKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMDE;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &DISKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBDIS;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &SMOKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBSMO;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MORKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMOR;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LOPKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLOP;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LPQKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLPQ;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIPKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIP;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HPQKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHPQ;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &PANKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBPAN;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &FADKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBFAD;
        processorRef.uiToAudio.push(msg);
    }
}

void PluginEditor::updateTrackProperties() {
    auto opt = processorRef.trackProperties.colourARGB;
    if (opt.has_value()) hostTrackColour = juce::Colour(*opt);
    auto optB = processorRef.trackProperties.name;
    if (optB.has_value()) hostTrackName = *optB;
    repaint();
}

void PluginEditor::updatePluginSize() {
    airwindowsLookAndFeel.userWidth = processorRef.pluginWidth;
    airwindowsLookAndFeel.userHeight = processorRef.pluginHeight;
    if (airwindowsLookAndFeel.userWidth < 8 || airwindowsLookAndFeel.userWidth > 16386) airwindowsLookAndFeel.userWidth = 618;
    if (airwindowsLookAndFeel.userHeight < 8 || airwindowsLookAndFeel.userHeight > 16386) airwindowsLookAndFeel.userHeight = 375;
    repaint();
}



void PluginEditor::idle()
{
    PluginProcessor::AudioToUIMessage msg;
    bool repaintTS{false}; //we don't redraw interface just for getting data into the GUI section
    while (processorRef.audioToUI.pop(msg)) {
        switch (msg.what) {
            case PluginProcessor::AudioToUIMessage::NEW_VALUE:
                if (msg.which == PluginProcessor::KNOBHIG) {HIGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHMG) {HMGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLMG) {LMGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBBSG) {BSGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHIF) {HIFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHMF) {HMFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLMF) {LMFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBBSF) {BSFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBTHR) {THRKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBATK) {ATKKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBRLS) {RLSKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBRAT) {RATKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBTRM) {TRMKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBMDE) {MDEKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBDIS) {DISKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBSMO) {SMOKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBMOR) {MORKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLOP) {LOPKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLPQ) {LPQKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHIP) {HIPKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHPQ) {HPQKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBPAN) {PANKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBFAD) {FADKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                break; //this can grab the knobs away from the user! Should cause the knob to repaint, too.
                
            case PluginProcessor::AudioToUIMessage::PEAK_LEFT: meter.pushPeakL(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::PEAK_RIGHT: meter.pushPeakR(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::SLEW_LEFT: meter.pushSlewL(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::SLEW_RIGHT: meter.pushSlewR(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::ZERO_LEFT: meter.pushZeroL(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::ZERO_RIGHT: meter.pushZeroR(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::BLINKEN_INPUT: meter.pushInput(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::BLINKEN_DISPLAY: meter.pushDisplay(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::BLINKEN_COMP: meter.pushComp(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::BLINKEN_TAPE: meter.pushTape(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::BLINKEN_OUTPUT: meter.pushOutput(msg.newValue); break;
            case PluginProcessor::AudioToUIMessage::INCREMENT: //Increment is running at 24 FPS and giving the above calculations
                meter.pushIncrement(); repaintTS = true; //we will repaint GUI after doing the following
                meter.outputMax = fmax(fmax(meter.outputR+0.00000001f,meter.outputG),meter.outputB);
                meter.outputMin = fmin(fmin(meter.outputR,meter.outputG),meter.outputB) / meter.outputMax;
                meter.gradientMin = meter.outputMin*meter.outputMin*meter.outputMin;
                //smallest number means brightest color, relative to highest number will be 1.0 meaning white
                meter.outputMin = 1.0f-((1.0f-meter.outputMin)*0.125f*meter.outputVol*meter.outputVol);
                //quiet parts converge on 1.0 also, to slow color activity in fades
                meter.outputMin = fmax(fmin(meter.outputMin,1.0f),0.0f);
                meter.storeR = pow(meter.outputR/meter.outputMax, 1.618033988749894f);
                meter.storeG = pow(meter.outputG/meter.outputMax, 1.618033988749894f);
                meter.storeB = pow(meter.outputB/meter.outputMax, 1.618033988749894f);
                //if all these are nearly white, they're near 1.0, but cyan is too much like white to be visible.
                //this causes scores to look wrong, because cyan tints 'should' score higher but it's an optical illusion.
                //Cyan is R0 G1 B1, so we will make up a color-tinter that reduces G and B by the amount that they match,
                meter.cyanFix = pow((1.0f-meter.storeR)*(1.0f-meter.storeR)*(1.0f-((meter.storeG-meter.storeB)*(meter.storeG-meter.storeB))),1.618033988749894f);
                meter.storeG -= meter.cyanFix; meter.storeB -= meter.cyanFix; //darken cyan tints for visual reference to white balance
                meter.backdropColour = juce::Colour::fromFloatRGBA (meter.storeR, meter.storeG, meter.storeB, 1.0f);
                 meter.outputR *= meter.outputMin;
                meter.outputG *= meter.outputMin;
                meter.outputB *= meter.outputMin;
                //we're doing the tone color as the backdrop for the full meter, as a gradient. Good way to have it take up a larger screen area
                
                TRMKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenInput*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenInput*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenInput*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                THRKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                MORKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenTape*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenTape*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenTape*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                FADKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenOutput*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenOutput*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenOutput*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                //User color LEDS are done like this: choose the same meter.data selection for each, and then the color will always be the user color
                //for an RGB or specified color blinken-knob, we don't reference user color, instead we just use the multiple meter.data directly without bringing in LEDColour
                //here is where we can make any control's thumb be a continuous blinkenlight with any value in meter. It runs at about 24fps.
                //We can do if statements etc. here, only thing we can NOT do is instantiate new variables. Do it as shown above and it works.
                //Also, this defaults to colors flashing against black, like LEDs: that's a good way to distinguish blinken-knobs from knobs
                break;
            default: break;
        } //end of switch statement for msg.what
    }
    if (repaintTS) {
        if (airwindowsLookAndFeel.useToneColor && meter.backdropColour.operator!=(meter.cachedColour)) {
            repaint();
            meter.cachedColour = meter.backdropColour;
            //we're going to maybe NOT constantly repaint everything, even in tonecolor mode
        } else {
            meter.repaint();
        }
        //normally it's the meter we have to update without touching it with the mouse,
        //but if the whole plugin has to be tone color, it all must repaint.
    }
}
