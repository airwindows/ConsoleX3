#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool starting) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    bool supportsDoublePrecisionProcessing() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    // updateTrackProperties may be called by the host if it feels like it
    // this method calls a similar one in the editor class that updates the editor
    void updateTrackProperties(const TrackProperties& properties) override;
    void updatePluginSize(int w, int h);
    
    TrackProperties trackProperties;
    int pluginWidth;
    int pluginHeight;

    //now we can declare variables used in the audio thread
    
    enum Parameters
    {
        KNOBHIG,
        KNOBHMG,
        KNOBLMG,
        KNOBBSG,
        KNOBHIF,
        KNOBHMF,
        KNOBLMF,
        KNOBBSF,
        KNOBTHR,
        KNOBATK,
        KNOBRLS,
        KNOBRAT,
        KNOBTRM,
        KNOBMDE,
        KNOBDIS,
        KNOBSMO,
        KNOBMOR,
        KNOBLOP,
        KNOBLPQ,
        KNOBHIP,
        KNOBHPQ,
        KNOBPAN,
        KNOBFAD,
    };
    static constexpr int n_params = KNOBFAD + 1;
    std::array<juce::AudioParameterFloat *, n_params> params;
    //This is where we're defining things that go into the plugin's interface.
    
    struct UIToAudioMessage
    {
        enum What
        {
            NEW_VALUE,
            BEGIN_EDIT,
            END_EDIT
        } what{NEW_VALUE};
        Parameters which;
        float newValue = 0.0;
    };
    //This is things the interface can tell the audio thread about.
    
    struct AudioToUIMessage
    {
        enum What
        {
            NEW_VALUE,
            PEAK_LEFT,
            PEAK_RIGHT,
            SLEW_LEFT,
            SLEW_RIGHT,
            ZERO_LEFT,
            ZERO_RIGHT,
            BLINKEN_INPUT,
            BLINKEN_DISPLAY,
            BLINKEN_COMP,
            BLINKEN_TAPE,
            BLINKEN_OUTPUT,
            INCREMENT
        } what{NEW_VALUE};
        Parameters which;
        float newValue = 0.0;
    };
    //This is kinds of information the audio thread can give the interface.
    
    template <typename T, int qSize = 4096> class LockFreeQueue
    {
      public:
        LockFreeQueue() : af(qSize) {}
        bool push(const T &ad)
        {
            auto ret = false;
            int start1, size1, start2, size2;
            af.prepareToWrite(1, start1, size1, start2, size2);
            if (size1 > 0)
            {
                dq[start1] = ad;
                ret = true;
            }
            af.finishedWrite(size1 + size2);
            return ret;
        }
        bool pop(T &ad)
        {
            bool ret = false;
            int start1, size1, start2, size2;
            af.prepareToRead(1, start1, size1, start2, size2);
            if (size1 > 0)
            {
                ad = dq[start1];
                ret = true;
            }
            af.finishedRead(size1 + size2);
            return ret;
        }
        juce::AbstractFifo af;
        T dq[qSize];
    };
    
    LockFreeQueue<UIToAudioMessage> uiToAudio;
    LockFreeQueue<AudioToUIMessage> audioToUI;
    
    enum {
        bezierHigh_A,bezierHigh_B,bezierHigh_C,bezierHigh,
        bezierHmid_A,bezierHmid_B,bezierHmid_C,bezierHmid,
        bezierLmid_A,bezierLmid_B,bezierLmid_C,bezierLmid,
        bezierBass_A,bezierBass_B,bezierBass_C,bezierBass,
        bezierHighQ_A,bezierHighQ_B,bezierHighQ_C,bezierHighQ,
        bezierMidQ_A,bezierMidQ_B,bezierMidQ_C,bezierMidQ,
        bezierLowQ_A,bezierLowQ_B,bezierLowQ_C,bezierLowQ,
        bezierThresh_A,bezierThresh_B,bezierThresh_C,bezierThresh,
        bezierRatio_A,bezierRatio_B,bezierRatio_C,bezierRatio,
        bezierMore_A,bezierMore_B,bezierMore_C,bezierMore,
        bezierLFreq_A,bezierLFreq_B,bezierLFreq_C,bezierLFreq,
        bezierHFreq_A,bezierHFreq_B,bezierHFreq_C,bezierHFreq,
        bezierGainL_A,bezierGainL_B,bezierGainL_C,bezierGainL,
        bezierGainR_A,bezierGainR_B,bezierGainR_C,bezierGainR,
        bezier_cycle,
        bezier_total
    }; //control smoothing as basic use of bezier curves
    double bezier[bezier_total];
    
    enum {
        bez_HMAL, bez_HMBL, bez_HMCL, bez_HMDL,
        bez_MAL, bez_MBL, bez_MCL, bez_MDL,
        bez_LMAL, bez_LMBL, bez_LMCL, bez_LMDL,
        bez_HMAR, bez_HMBR, bez_HMCR, bez_HMDR, bez_HMcycle,
        bez_MAR, bez_MBR, bez_MCR, bez_MDR, bez_Mcycle,
        bez_LMAR, bez_LMBR, bez_LMCR, bez_LMDR, bez_LMcycle,
        bez_EQtotal
    }; //the new undersampling. bez signifies the bezier curve reconstruction
    double bezEQ[bez_EQtotal][3];
    
    enum {
        bez_A,
        bez_B,
        bez_C,
        bez_Ctrl,
        bez_cycle,
        bez_min,
        bez_comp,
        bez_total
    }; //the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total];
    //Dynamics3
    
    double iirHPositionL[29];
    double iirHAngleL[29];
    double iirHPositionR[29];
    double iirHAngleR[29];
    bool hBypass;
    double iirLPositionL[29];
    double iirLAngleL[29];
    double iirLPositionR[29];
    double iirLAngleR[29];
    bool lBypass;
    //Cabs2
    
    enum {
        bip_dvAA, bip_dvAB, bip_dvAC, bip_dvAD, bip_pvAA, bip_pvAB, bip_pvAC, bip_pvAD,
        bip_dvBA, bip_dvBB, bip_dvBC, bip_dvBD, bip_pvBA, bip_pvBB, bip_pvBC, bip_pvBD,
        bip_dvLA, bip_dvLB, bip_dvLC, bip_dvLD, bip_pvLA, bip_pvLB, bip_pvLC, bip_pvLD,
        bip_dvRA, bip_dvRB, bip_dvRC, bip_dvRD, bip_pvRA, bip_pvRB, bip_pvRC, bip_pvRD,
        bip_total //each distortion section can have one of these, it stacks well
    }; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total];

    uint32_t fpdL;
    uint32_t fpdR;
    //default stuff
    
    double peakLeft = 0.0;
    double peakRight = 0.0;
    double slewLeft = 0.0;
    double slewRight = 0.0;
    double previousLeft = 0.0;
    double previousRight = 0.0;
    double zeroLeft = 0.0;
    double zeroRight = 0.0;
    double longestZeroLeft = 0.0;
    double longestZeroRight = 0.0;
    double maxTrim = 0.0;
    double display = 0.0;
    double maxComp = 0.0;
    double maxTape = 0.0;
    double maxFader = 0.0;
    bool wasPositiveL = false;
    bool wasPositiveR = false;
    int windowCount = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
