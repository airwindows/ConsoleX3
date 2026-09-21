#include "PluginProcessor.h"
#include "PluginEditor.h"
#ifndef M_PI
#  define M_PI (3.14159265358979323846)
#endif
#ifndef M_PI_2
#  define M_PI_2 (1.57079632679489661923)
#endif

//==============================================================================
PluginProcessor::PluginProcessor():AudioProcessor (
                    BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
){
    for (int x = 0; x < bezier_total; x++) bezier[x] = 0.0;
    //Control Smoothing
    
    for (int x = 0; x < bez_EQtotal; x++) {for (int y = 0; y < 3; y++) bezEQ[x][y] = 0.0;}
    //BezEQ4
    
    for (int x = 0; x < bez_total; x++) bezComp[x] = 0.0;
    //Dynamics3
    
    for(int count = 0; count < 28; count++) {
        iirHPositionL[count] = 0.0;
        iirHAngleL[count] = 0.0;
        iirHPositionR[count] = 0.0;
        iirHAngleR[count] = 0.0;
    }
    hBypass = false;
    for(int count = 0; count < 28; count++) {
        iirLPositionL[count] = 0.0;
        iirLAngleL[count] = 0.0;
        iirLPositionR[count] = 0.0;
        iirLAngleR[count] = 0.0;
    }
    lBypass = false;
    //Cabs2
    
    for (int x = 0; x < bip_total; x++) bip[x] = 0.0;

    fpdL = 1.0; while (fpdL < 16386) fpdL = (uint)rand()*UINT32_MAX;
    fpdR = 1.0; while (fpdR < 16386) fpdR = (uint)rand()*UINT32_MAX;
    //this is reset: values being initialized only once. Startup values, whatever they are.

    // (internal ID, how it's shown in DAW generic view, {min, max}, default)
    addParameter(params[KNOBHIG] = new juce::AudioParameterFloat("high", "High", {0.0f, 1.0f}, 0.5f));             params[KNOBHIG]->addListener(this);
    addParameter(params[KNOBHMG] = new juce::AudioParameterFloat("hmid", "HMid", {0.0f, 1.0f}, 0.5f));             params[KNOBHMG]->addListener(this);
    addParameter(params[KNOBLMG] = new juce::AudioParameterFloat("lmid", "LMid", {0.0f, 1.0f}, 0.5f));             params[KNOBLMG]->addListener(this);
    addParameter(params[KNOBBSG] = new juce::AudioParameterFloat("bass", "Bass", {0.0f, 1.0f}, 0.5f));             params[KNOBBSG]->addListener(this);
    addParameter(params[KNOBHIF] = new juce::AudioParameterFloat("highf", "HighF", {0.0f, 1.0f}, 0.5f));           params[KNOBHIF]->addListener(this);
    addParameter(params[KNOBHMF] = new juce::AudioParameterFloat("hmidf", "HMidF", {0.0f, 1.0f}, 0.5f));           params[KNOBHMF]->addListener(this);
    addParameter(params[KNOBLMF] = new juce::AudioParameterFloat("lmidf", "LMidF", {0.0f, 1.0f}, 0.5f));           params[KNOBLMF]->addListener(this);
    addParameter(params[KNOBBSF] = new juce::AudioParameterFloat("bassf", "BassF", {0.0f, 1.0f}, 0.5f));           params[KNOBBSF]->addListener(this);
    addParameter(params[KNOBTHR] = new juce::AudioParameterFloat("thresh", "Threshold", {0.0f, 1.0f}, 1.0f));      params[KNOBTHR]->addListener(this);
    addParameter(params[KNOBATK] = new juce::AudioParameterFloat("attack", "Attack", {0.0f, 1.0f}, 0.384f));       params[KNOBATK]->addListener(this);
    addParameter(params[KNOBRLS] = new juce::AudioParameterFloat("release", "Release", {0.0f, 1.0f}, 0.618f));     params[KNOBRLS]->addListener(this);
    addParameter(params[KNOBRAT] = new juce::AudioParameterFloat("invwet", "Inv/Wet", {0.0f, 1.0f}, 1.0f));        params[KNOBRAT]->addListener(this);
    addParameter(params[KNOBTRM] = new juce::AudioParameterFloat("input", "Input", {0.0f, 1.0f}, 0.5f));           params[KNOBTRM]->addListener(this);
    addParameter(params[KNOBMDE] = new juce::AudioParameterFloat("channelbuss", "Channel | Buss", {0.0f, 1.0f}, 0.0f)); params[KNOBMDE]->addListener(this);
    addParameter(params[KNOBDIS] = new juce::AudioParameterFloat("display", "Display", {0.0f, 1.0f}, 0.25f));       params[KNOBDIS]->addListener(this);
    addParameter(params[KNOBSMO] = new juce::AudioParameterFloat("smooth", "Smooth", {0.0f, 1.0f}, 0.5f));         params[KNOBSMO]->addListener(this);
    addParameter(params[KNOBMOR] = new juce::AudioParameterFloat("more", "More", {0.0f, 1.0f}, 0.0f));             params[KNOBMOR]->addListener(this);
    addParameter(params[KNOBLOP] = new juce::AudioParameterFloat("lowpass", "Lowpass", {0.0f, 1.0f}, 1.0f));       params[KNOBLOP]->addListener(this);
    addParameter(params[KNOBLPQ] = new juce::AudioParameterFloat("lowpassq", "Resonance", {0.0f, 1.0f}, 0.0f));    params[KNOBLPQ]->addListener(this);
    addParameter(params[KNOBHIP] = new juce::AudioParameterFloat("highpass", "Highpass", {0.0f, 1.0f}, 0.0f));     params[KNOBHIP]->addListener(this);
    addParameter(params[KNOBHPQ] = new juce::AudioParameterFloat("highpassq", "Resonance", {0.0f, 1.0f}, 0.0f));   params[KNOBHPQ]->addListener(this);
    addParameter(params[KNOBPAN] = new juce::AudioParameterFloat("pan", "Pan", {0.0f, 1.0f}, 0.5f));               params[KNOBPAN]->addListener(this);
    addParameter(params[KNOBFAD] = new juce::AudioParameterFloat("fader", "Fader", {0.0f, 1.0f}, 1.0f));           params[KNOBFAD]->addListener(this);
}

PluginProcessor::~PluginProcessor() {}
void PluginProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
    AudioToUIMessage msg;
    msg.what = AudioToUIMessage::NEW_VALUE;
    msg.which = (PluginProcessor::Parameters)parameterIndex;
    msg.newValue = params[parameterIndex]->convertFrom0to1(newValue);
    audioToUI.push(msg);
}
void PluginProcessor::parameterGestureChanged(int parameterIndex, bool starting) {}
const juce::String PluginProcessor::getName() const {return JucePlugin_Name;}
bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::supportsDoublePrecisionProcessing() const
{
   #if JucePlugin_SupportsDoublePrecisionProcessing
    return true;
   #else
    return true;
    //note: I don't know whether that config option is set, so I'm hardcoding it
    //knowing I have enabled such support: keeping the boilerplate stuff tho
    //in case I can sort out where it's enabled as a flag
   #endif
}
bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double PluginProcessor::getTailLengthSeconds() const {return 0.0;}
int PluginProcessor::getNumPrograms() {return 1;}
int PluginProcessor::getCurrentProgram() {return 0;}
void PluginProcessor::setCurrentProgram (int index) {juce::ignoreUnused (index);}
const juce::String PluginProcessor::getProgramName (int index) {juce::ignoreUnused (index); return {};}
void PluginProcessor::changeProgramName (int index, const juce::String& newName) {juce::ignoreUnused (index, newName);}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {juce::ignoreUnused (sampleRate, samplesPerBlock);}
void PluginProcessor::releaseResources() {}
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this metering code we only support stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

//==============================================================================

bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("consolex3"));
    xml->setAttribute("streamingVersion", (int)8524);

    for (unsigned long i = 0; i < n_params; ++i)
    {
        juce::String nm = juce::String("awcx3_") + std::to_string(i);
        float val = 0.0f; if (i < n_params) val = *(params[i]);
        xml->setAttribute(nm, val);
    }
    if (pluginWidth < 8 || pluginWidth > 16386) pluginWidth = 618;
    xml->setAttribute(juce::String("awcx3_width"), pluginWidth);
    if (pluginHeight < 8 || pluginHeight > 16386) pluginHeight = 375;
    xml->setAttribute(juce::String("awcx3_height"), pluginHeight);
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("consolex3"))
        {
            for (unsigned long i = 0; i < n_params; ++i)
            {
                juce::String nm = juce::String("awcx3_") + std::to_string(i);
                auto f = xmlState->getDoubleAttribute(nm);
                params[i]->setValueNotifyingHost((float)f);
            }
            auto w = xmlState->getIntAttribute(juce::String("awcx3_width"));
            if (w < 8 || w > 16386) w = 618;
            auto h = xmlState->getIntAttribute(juce::String("awcx3_height"));
            if (h < 8 || h > 16386) h = 375;
            updatePluginSize(w, h);
        }
        updateHostDisplay();
    }
    //These functions are adapted (simplified) from baconpaul's airwin2rack and all thanks to getting
    //it working shall go there, though sudara or anyone could've spotted that I hadn't done these.
    //baconpaul pointed me to the working versions in airwin2rack, that I needed to see.
}

void PluginProcessor::updateTrackProperties(const TrackProperties& properties)
{
    trackProperties = properties;
    // call the version in the editor to update there
    if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        editor->updateTrackProperties();
}

void PluginProcessor::updatePluginSize(int w, int h)
{
    pluginWidth = w;
    pluginHeight = h;
    // call the version in the editor to update there
    if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        editor->updatePluginSize();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}


void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (!(getBus(false, 0)->isEnabled() && getBus(true, 0)->isEnabled())) return;
    auto mainOutput = getBusBuffer(buffer, false, 0); //if we have audio busses at all,
    auto mainInput = getBusBuffer(buffer, true, 0); //they're now mainOutput and mainInput.
    
    UIToAudioMessage uim;
    while (uiToAudio.pop(uim)) {
        switch (uim.what) {
        case UIToAudioMessage::NEW_VALUE: params[uim.which]->setValueNotifyingHost(params[uim.which]->convertTo0to1(uim.newValue)); break;
        case UIToAudioMessage::BEGIN_EDIT: params[uim.which]->beginChangeGesture(); break;
        case UIToAudioMessage::END_EDIT: params[uim.which]->endChangeGesture(); break;
        }
    } //Handle inbound messages from the UI thread

    double rmsSize = (1881.0 / 44100.0)*getSampleRate(); //higher is slower with larger RMS buffers
    double zeroCrossScale = (1.0 / getSampleRate())*44100.0;
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();
    
    double bezierRez = fmax(pow((1.0-params[KNOBSMO]->get())*0.25,3.0)/overallscale,0.00001);
    int stepped = 999999; if (bezierRez > 0.000001) stepped = (int)(1.0/bezierRez); bezierRez = 0.99999999/stepped;
    const double bezierTrim = 1.0-(bezierRez*((double)stepped/(stepped+1.0)));
    //manages the overall Bezier control smoothing system plugin-wide
    int spacing = (int)floor(overallscale*2.0);
    if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
    const double gainTrim = params[KNOBTRM]->get()*2.0;
    const double trebleRef =  params[KNOBHIF]->get()-0.3;
    const double highmidRef =  params[KNOBHMF]->get()-0.5;
    const double lowmidRef =  params[KNOBLMF]->get()-0.7;
    const double bassRef =  params[KNOBBSF]->get()-0.9;
    double HMderez = 0.75 + (trebleRef*0.25);
    double LMderez = 0.25 + (bassRef*0.25);
    double Mderez = ((HMderez+LMderez+highmidRef+lowmidRef)*0.25);
    HMderez /= overallscale; Mderez /= overallscale; LMderez /= overallscale;
    if (HMderez > 1.0) HMderez = 1.0;
    stepped = 999999; if (HMderez > 0.000001) stepped = (int)(1.0/HMderez);
    HMderez = 0.99999999 / stepped;
    const double HMtrim = 1.0-(HMderez*((double)stepped/(stepped+1.0)));
    stepped = 999999; if (Mderez > 0.000001) stepped = (int)(1.0/Mderez);
    Mderez = 0.99999999 / stepped;
    const double Mtrim = 1.0-(Mderez*((double)stepped/(stepped+1.0)));
    stepped = 999999; if (LMderez > 0.000001) stepped = (int)(1.0/LMderez);
    LMderez = 0.99999999 / stepped;
    const double LMtrim = 1.0-(LMderez*((double)stepped/(stepped+1.0)));
    //BezEQ3 stepped elements
    double bezRez = fmax(pow((1.0-params[KNOBATK]->get())*0.4,4.0)/overallscale,0.0001);
    bezRez /= (2.0/pow(overallscale,0.5-((overallscale-1.0)*0.0375)));
    stepped = 999999; if (bezRez > 0.000001) stepped = (int)(1.0/bezRez);
    bezRez = 0.99999999 / stepped;
    const double bezTrim = 1.0-(bezRez*((double)stepped/(stepped+1.0)));
    double sloRez = fmax(pow((1.0-params[KNOBRLS]->get())*0.4,4.0)/overallscale,0.00001);
    sloRez /= (2.0/pow(overallscale,0.5-((overallscale-1.0)*0.0375)));
    double sqrThresh = 1.0; //recalculated in bezier section
    double bezThresh = bezier[bezierThresh];
    double invDry = bezier[bezierRatio];
    bool compress = (params[KNOBRAT]->get() > 0.499999);
    bool compBypass = (params[KNOBTHR]->get() > 0.9999);
    bool tapeBypass = (params[KNOBMOR]->get() < 0.0001);
    double pluginMode = params[KNOBMDE]->get();
    //Dynamics3 stepped elements
    const int Lpoles = (int)pow(params[KNOBLPQ]->get()*3.0,3.0)+1;
    const int Hpoles = (int)pow(params[KNOBHPQ]->get()*3.0,3.0)+1; //1-28 pole filter
    //Lowpass/Highpass stepped elements
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto outL = mainOutput.getWritePointer(0, i);
        auto outR = mainOutput.getWritePointer(1, i);
        auto inL = mainInput.getReadPointer(0, i); //in isBussesLayoutSupported, we have already
        auto inR = mainInput.getReadPointer(1, i); //specified that we can only be stereo and never mono
        double inputSampleL = *inL;
        double inputSampleR = *inR;
        if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
        
        if (pluginMode > 0.5) {
            double dryL = inputSampleL;
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949));
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949));
            bip[bip_dvLA] = dryL - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA]-bip[bip_dvLA]; bip[bip_pvLA] = bip[bip_dvLA];//velocity
            bip[bip_dvLC] = bip[bip_pvLB]-bip[bip_dvLB]; bip[bip_pvLB] = bip[bip_dvLB];//acceleration
            bip[bip_dvLD] = bip[bip_pvLC]-bip[bip_dvLC]; bip[bip_pvLC] = bip[bip_dvLC];//jerk
            double bip_dvE = bip[bip_pvLD]-bip[bip_dvLD]; bip[bip_pvLD] = bip[bip_dvLD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvLC])*0.0618)+(fabs(bip[bip_dvLD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            double dryR = inputSampleR;
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949));
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949));
            bip[bip_dvRA] = dryR - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA]-bip[bip_dvRA]; bip[bip_pvRA] = bip[bip_dvRA];//velocity
            bip[bip_dvRC] = bip[bip_pvRB]-bip[bip_dvRB]; bip[bip_pvRB] = bip[bip_dvRB];//acceleration
            bip[bip_dvRD] = bip[bip_pvRC]-bip[bip_dvRC]; bip[bip_pvRC] = bip[bip_dvRC];//jerk
            bip_dvE = bip[bip_pvRD]-bip[bip_dvRD]; bip[bip_pvRD] = bip[bip_dvRD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvRC])*0.0618)+(fabs(bip[bip_dvRD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            if (pluginMode < 1.0f) {
                inputSampleL = (dryL*(1.0-((pluginMode-0.5)*2.0))) + (inputSampleL*((pluginMode-0.5)*2.0));
                inputSampleR = (dryR*(1.0-((pluginMode-0.5)*2.0))) + (inputSampleR*((pluginMode-0.5)*2.0));
            }
        } //engage pluginMode, buss
        
        //begin Bezier control smoothing engine
        bezier[bezier_cycle] += bezierRez;
        if (bezier[bezier_cycle] > bezierTrim) {bezier[bezier_cycle] = 0.0;
            bezier[bezierHigh_C] =  bezier[bezierHigh_B];  bezier[bezierHigh_B] =  bezier[bezierHigh_A];
            bezier[bezierHmid_C] =  bezier[bezierHmid_B];  bezier[bezierHmid_B] =  bezier[bezierHmid_A];
            bezier[bezierLmid_C] =  bezier[bezierLmid_B];  bezier[bezierLmid_B] =  bezier[bezierLmid_A];
            bezier[bezierBass_C] =  bezier[bezierBass_B];  bezier[bezierBass_B] =  bezier[bezierBass_A];
            bezier[bezierHighQ_C] =  bezier[bezierHighQ_B];  bezier[bezierHighQ_B] =  bezier[bezierHighQ_A];
            bezier[bezierMidQ_C] =  bezier[bezierMidQ_B];  bezier[bezierMidQ_B] =  bezier[bezierMidQ_A];
            bezier[bezierLowQ_C] =  bezier[bezierLowQ_B];  bezier[bezierLowQ_B] =  bezier[bezierLowQ_A];
            bezier[bezierThresh_C]= bezier[bezierThresh_B];bezier[bezierThresh_B]= bezier[bezierThresh_A];
            bezier[bezierRatio_C] = bezier[bezierRatio_B]; bezier[bezierRatio_B] = bezier[bezierRatio_A];
            bezier[bezierMore_C] =  bezier[bezierMore_B];  bezier[bezierMore_B] =  bezier[bezierMore_A];
            bezier[bezierLFreq_C] = bezier[bezierLFreq_B]; bezier[bezierLFreq_B] = bezier[bezierLFreq_A];
            bezier[bezierHFreq_C] = bezier[bezierHFreq_B]; bezier[bezierHFreq_B] = bezier[bezierHFreq_A];
            bezier[bezierGainL_C] = bezier[bezierGainL_B]; bezier[bezierGainL_B] = bezier[bezierGainL_A];
            bezier[bezierGainR_C] = bezier[bezierGainR_B]; bezier[bezierGainR_B] = bezier[bezierGainR_A];
            //one of these bucket brigade lines for every smoothed control
            //begin smoothed control calculations
            bezier[bezierHigh_A] = (pow(params[KNOBHIG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierHmid_A] = (pow(params[KNOBHMG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierLmid_A] = (pow(params[KNOBLMG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierBass_A] = (pow(params[KNOBBSG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierHighQ_A] = 0.84+((highmidRef-trebleRef)*0.1666666);
            bezier[bezierMidQ_A] = 0.84+((lowmidRef-highmidRef)*0.1666666);
            bezier[bezierLowQ_A] = 0.84+((bassRef-lowmidRef)*0.1666666);
            //BezEQ3
            bezier[bezierThresh_A] = pow(params[KNOBTHR]->get()+0.6180339887498949,2.0)*1.6180339887498949;
            sqrThresh = sqrt(bezier[bezierThresh_A]);
            bezier[bezierRatio_A] = pow(1.0-(fabs(params[KNOBRAT]->get()-0.5)*2.0),1.6180339887498949);
            //Dynamics3
            bezier[bezierMore_A] = (params[KNOBMOR]->get()*3.0)+1.0;
            //TapeHack
            bezier[bezierLFreq_A] = pow(fmax(params[KNOBLOP]->get(),0.002),overallscale); //the lowpass
            bezier[bezierHFreq_A] = pow(params[KNOBHIP]->get(),overallscale+2.0); //the highpass
            //Cabs2
            double gain = pow(params[KNOBFAD]->get(),2.0)*1.414213562373094; //Pan will pad this
            bezier[bezierGainL_A] = gain*sin(M_PI_2-(params[KNOBPAN]->get()*M_PI_2));
            bezier[bezierGainR_A] = gain*sin(params[KNOBPAN]->get()*M_PI_2);
            //Fader and Pan
            //end expensive control calculations
        }
        const double lerp = bezier[bezier_cycle]; //with this many controls we'll make temp variables
        const double La = (1.0-lerp)*(1.0-lerp); const double Lb = 2.0*(1.0-lerp)*lerp; const double Lc = lerp*lerp;
        bezier[bezierHigh] = (bezier[bezierHigh_B] + (bezier[bezierHigh_C]*La) + (bezier[bezierHigh_B]*Lb) + (bezier[bezierHigh_A]*Lc))*0.5;
        bezier[bezierHmid] = (bezier[bezierHmid_B] + (bezier[bezierHmid_C]*La) + (bezier[bezierHmid_B]*Lb) + (bezier[bezierHmid_A]*Lc))*0.5;
        bezier[bezierLmid] = (bezier[bezierLmid_B] + (bezier[bezierLmid_C]*La) + (bezier[bezierLmid_B]*Lb) + (bezier[bezierLmid_A]*Lc))*0.5;
        bezier[bezierBass] = (bezier[bezierBass_B] + (bezier[bezierBass_C]*La) + (bezier[bezierBass_B]*Lb) + (bezier[bezierBass_A]*Lc))*0.5;
        bezier[bezierHighQ] = (bezier[bezierHighQ_B] + (bezier[bezierHighQ_C]*La) + (bezier[bezierHighQ_B]*Lb) + (bezier[bezierHighQ_A]*Lc))*0.5;
        bezier[bezierMidQ] = (bezier[bezierMidQ_B] + (bezier[bezierMidQ_C]*La) + (bezier[bezierMidQ_B]*Lb) + (bezier[bezierMidQ_A]*Lc))*0.5;
        bezier[bezierLowQ] = (bezier[bezierLowQ_B] + (bezier[bezierLowQ_C]*La) + (bezier[bezierLowQ_B]*Lb) + (bezier[bezierLowQ_A]*Lc))*0.5;
        bezier[bezierThresh] = (bezier[bezierThresh_B] + (bezier[bezierThresh_C]*La) + (bezier[bezierThresh_B]*Lb) + (bezier[bezierThresh_A]*Lc))*0.5;
        bezier[bezierRatio] = (bezier[bezierRatio_B] + (bezier[bezierRatio_C]*La) + (bezier[bezierRatio_B]*Lb) + (bezier[bezierRatio_A]*Lc))*0.5;
        bezier[bezierMore] = (bezier[bezierMore_B] + (bezier[bezierMore_C]*La) + (bezier[bezierMore_B]*Lb) + (bezier[bezierMore_A]*Lc))*0.5;
        bezier[bezierLFreq] = (bezier[bezierLFreq_B] + (bezier[bezierLFreq_C]*La) + (bezier[bezierLFreq_B]*Lb) + (bezier[bezierLFreq_A]*Lc))*0.5;
        bezier[bezierHFreq] = (bezier[bezierHFreq_B] + (bezier[bezierHFreq_C]*La) + (bezier[bezierHFreq_B]*Lb) + (bezier[bezierHFreq_A]*Lc))*0.5;
        bezier[bezierGainL] = (bezier[bezierGainL_B] + (bezier[bezierGainL_C]*La) + (bezier[bezierGainL_B]*Lb) + (bezier[bezierGainL_A]*Lc))*0.5;
        bezier[bezierGainR] = (bezier[bezierGainR_B] + (bezier[bezierGainR_C]*La) + (bezier[bezierGainR_B]*Lb) + (bezier[bezierGainR_A]*Lc))*0.5;
        //end Bezier control smoothing engine
        
        if (fabs(inputSampleL) > maxTrim) maxTrim = fabs(inputSampleL);
        if (fabs(inputSampleR) > maxTrim) maxTrim = fabs(inputSampleR);
        //begin Trim switch
        if (gainTrim != 1.0) {inputSampleL *= gainTrim; inputSampleR *= gainTrim;}
        //end Trim control
        

        //begin BezEQ3 Pre
        double highL = inputSampleL;
        double highR = inputSampleR;
        bezEQ[bez_HMcycle][0] += HMderez;
        bezEQ[bez_HMAL][0] += (highL * HMderez);
        bezEQ[bez_HMAR][0] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][0] > HMtrim) {
            bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0]; bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0];
            bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][0] = 0.0;
            bezEQ[bez_HMDR][0] = bezEQ[bez_HMCR][0]; bezEQ[bez_HMCR][0] = bezEQ[bez_HMBR][0];
            bezEQ[bez_HMBR][0] = bezEQ[bez_HMAR][0]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][0] = 0.0;
            bezEQ[bez_HMcycle][0] = 0.0;
        }
        double X = bezEQ[bez_HMcycle][0];
        double hmidL = bezEQ[bez_HMCL][0]+(bezEQ[bez_HMDL][0]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][0]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        double hmidR = bezEQ[bez_HMCR][0]+(bezEQ[bez_HMDR][0]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][0]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][0] += Mderez;
        bezEQ[bez_MAL][0] += (hmidL * Mderez);
        bezEQ[bez_MAR][0] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][0] > Mtrim) {
            bezEQ[bez_MDL][0] = bezEQ[bez_MCL][0]; bezEQ[bez_MCL][0] = bezEQ[bez_MBL][0];
            bezEQ[bez_MBL][0] = bezEQ[bez_MAL][0]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][0] = 0.0;
            bezEQ[bez_MDR][0] = bezEQ[bez_MCR][0]; bezEQ[bez_MCR][0] = bezEQ[bez_MBR][0];
            bezEQ[bez_MBR][0] = bezEQ[bez_MAR][0]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][0] = 0.0;
            bezEQ[bez_Mcycle][0] = 0.0;
        }
        X = bezEQ[bez_Mcycle][0];
        double lmidL = bezEQ[bez_MCL][0]+(bezEQ[bez_MDL][0]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][0]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        double lmidR = bezEQ[bez_MCR][0]+(bezEQ[bez_MDR][0]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][0]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][0] += LMderez;
        bezEQ[bez_LMAL][0] += (lmidL * LMderez);
        bezEQ[bez_LMAR][0] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][0] > LMtrim) {
            bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0]; bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0];
            bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][0] = 0.0;
            bezEQ[bez_LMDR][0] = bezEQ[bez_LMCR][0]; bezEQ[bez_LMCR][0] = bezEQ[bez_LMBR][0];
            bezEQ[bez_LMBR][0] = bezEQ[bez_LMAR][0]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][0] = 0.0;
            bezEQ[bez_LMcycle][0] = 0.0;
        }
        X = bezEQ[bez_LMcycle][0];
        double bassL = bezEQ[bez_LMCL][0]+(bezEQ[bez_LMDL][0]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][0]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        double bassR = bezEQ[bez_LMCR][0]+(bezEQ[bez_LMDR][0]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][0]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        //end BezEQ3 Pre
        
        if (!compBypass) {
            //begin Dynamics3
            double dryCompL = inputSampleL;
            double dryCompR = inputSampleR;
            if (compress) {
                inputSampleL *= (bezComp[bez_comp]/bezThresh);
                inputSampleR *= (bezComp[bez_comp]/bezThresh);
            } else {
                inputSampleL /= bezThresh;
                inputSampleR /= bezThresh;
            }
            double ctrl = fmin(fmax(fabs(inputSampleL),fabs(inputSampleR)),sqrThresh*bezComp[bez_comp]*0.6180339887498949);
            bezComp[bez_min] = fmax(bezComp[bez_min]-sloRez,ctrl);
            bezComp[bez_Ctrl] += (bezComp[bez_min] * bezRez);
            bezComp[bez_cycle] += bezRez;
            if (bezComp[bez_cycle] > bezTrim) {bezComp[bez_cycle] = 0.0;
                bezComp[bez_C] = bezComp[bez_B]; bezComp[bez_B] = bezComp[bez_A];
                bezComp[bez_A] = bezComp[bez_Ctrl]; bezComp[bez_Ctrl] = 0.0;}
            X = bezComp[bez_cycle];
            bezComp[bez_comp] = bezComp[bez_B]+(bezComp[bez_C]*(1.0-X)*(1.0-X))+(bezComp[bez_B]*2.0*(1.0-X)*X)+(bezComp[bez_A]*X*X);
            if (bezComp[bez_comp] > maxComp) maxComp = bezComp[bez_comp]; //assign the blinkenlight value
            bezComp[bez_comp] = ((1.0-(fmin(bezComp[bez_comp],0.9999))));
            if (compress) {
                inputSampleL = inputSampleL*(1.0-invDry)*bezComp[bez_comp]*bezThresh;
                inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
                double addtwo = inputSampleL * inputSampleL;
                double empower = inputSampleL * addtwo; // inputSampleL to the third power
                inputSampleL -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleL += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleL -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleL += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleL -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleL += dryCompL*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
                inputSampleR = inputSampleR*(1.0-invDry)*bezComp[bez_comp]*bezThresh;
                inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
                addtwo = inputSampleR * inputSampleR;
                empower = inputSampleR * addtwo; // inputSampleR to the third power
                inputSampleR -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleR += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleR -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleR += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleR -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleR += dryCompR*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
            } else {
                inputSampleL = ((inputSampleL/(0.1+bezThresh))*(1.0-invDry))/bezComp[bez_comp];
                inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
                double addtwo = inputSampleL * inputSampleL;
                double empower = inputSampleL * addtwo; // inputSampleL to the third power
                inputSampleL -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleL += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleL -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleL += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleL -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleL += dryCompL*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
                inputSampleR = ((inputSampleR/(0.1+bezThresh))*(1.0-invDry))/bezComp[bez_comp];
                inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
                addtwo = inputSampleR * inputSampleR;
                empower = inputSampleR * addtwo; // inputSampleR to the third power
                inputSampleR -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleR += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleR -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleR += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleR -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleR += dryCompR*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
            }
        } //end Dynamics3
        
        //begin BezEQ3 Mid
        highL = inputSampleL;
        highR = inputSampleR;
        bezEQ[bez_HMcycle][1] += HMderez;
        bezEQ[bez_HMAL][1] += (highL * HMderez);
        bezEQ[bez_HMAR][1] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][1] > HMtrim) {
            bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1]; bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1];
            bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][1] = 0.0;
            bezEQ[bez_HMDR][1] = bezEQ[bez_HMCR][1]; bezEQ[bez_HMCR][1] = bezEQ[bez_HMBR][1];
            bezEQ[bez_HMBR][1] = bezEQ[bez_HMAR][1]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][1] = 0.0;
            bezEQ[bez_HMcycle][1] = 0.0;
        }
        X = bezEQ[bez_HMcycle][1];
        hmidL = bezEQ[bez_HMCL][1]+(bezEQ[bez_HMDL][1]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][1]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        hmidR = bezEQ[bez_HMCR][1]+(bezEQ[bez_HMDR][1]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][1]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][1] += Mderez;
        bezEQ[bez_MAL][1] += (hmidL * Mderez);
        bezEQ[bez_MAR][1] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][1] > Mtrim) {
            bezEQ[bez_MDL][1] = bezEQ[bez_MCL][1]; bezEQ[bez_MCL][1] = bezEQ[bez_MBL][1];
            bezEQ[bez_MBL][1] = bezEQ[bez_MAL][1]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][1] = 0.0;
            bezEQ[bez_MDR][1] = bezEQ[bez_MCR][1]; bezEQ[bez_MCR][1] = bezEQ[bez_MBR][1];
            bezEQ[bez_MBR][1] = bezEQ[bez_MAR][1]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][1] = 0.0;
            bezEQ[bez_Mcycle][1] = 0.0;
        }
        X = bezEQ[bez_Mcycle][1];
        lmidL = bezEQ[bez_MCL][1]+(bezEQ[bez_MDL][1]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][1]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        lmidR = bezEQ[bez_MCR][1]+(bezEQ[bez_MDR][1]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][1]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][1] += LMderez;
        bezEQ[bez_LMAL][1] += (lmidL * LMderez);
        bezEQ[bez_LMAR][1] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][1] > LMtrim) {
            bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1]; bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1];
            bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][1] = 0.0;
            bezEQ[bez_LMDR][1] = bezEQ[bez_LMCR][1]; bezEQ[bez_LMCR][1] = bezEQ[bez_LMBR][1]; 
            bezEQ[bez_LMBR][1] = bezEQ[bez_LMAR][1]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][1] = 0.0;
            bezEQ[bez_LMcycle][1] = 0.0;
        }
        X = bezEQ[bez_LMcycle][1];
        bassL = bezEQ[bez_LMCL][1]+(bezEQ[bez_LMDL][1]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][1]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        bassR = bezEQ[bez_LMCR][1]+(bezEQ[bez_LMDR][1]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][1]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        
        //end BezEQ3 Mid
        
        //begin Weave
        if (!tapeBypass) {
            inputSampleL *= bezier[bezierMore];
            double bip_delta = inputSampleL; //delta can be just local and re-used
            if (inputSampleL != 0.0) {
                inputSampleL = fmax(fmin(inputSampleL,1.0),-1.0);
                inputSampleL *= 1.27;
                long double weave = sqrt(fabs(inputSampleL));
                long double spiral = inputSampleL * weave;
                long double Y = spiral; Y *= Y;
                long double temp = spiral * Y;
                spiral -= temp*0.16666666666666666666666666666666666; temp *= Y;//-8 dB
                spiral += temp*0.00833333333333333333333333333333333; temp *= Y;//-20 dB
                spiral -= temp*0.00019841269841269841269841269841269; temp *= Y;//-37 dB
                spiral += temp*0.00000275573192239858906525573192239; temp *= Y;//-55 dB
                spiral -= temp*0.00000002505210838544171877521083854; temp *= Y;//-76 dB
                spiral += temp*0.00000000016059043836821614599392377; temp *= Y;//-108 dB
                spiral -= temp*0.00000000000076471637318198164759011; temp *= Y;//-121 dB
                spiral += temp*0.00000000000000281145725434552076319; temp *= Y;//-146 dB
                spiral -= temp*0.00000000000000000822063524662432971; temp *= Y;//-171 dB
                spiral += temp*0.00000000000000000001957294106339126;           //-197 dB
                inputSampleL = spiral / weave; //purest form is full Spiral
            } //version of Weave in ConsoleX uses A and B for derivatives, not L and R
            bip[bip_dvAA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvAB] = bip[bip_pvAA]-bip[bip_dvAA]; bip[bip_pvAA] = bip[bip_dvAA];//velocity
            bip[bip_dvAC] = bip[bip_pvAB]-bip[bip_dvAB]; bip[bip_pvAB] = bip[bip_dvAB];//acceleration
            bip[bip_dvAD] = bip[bip_pvAC]-bip[bip_dvAC]; bip[bip_pvAC] = bip[bip_dvAC];//jerk
            double bip_dvE = bip[bip_pvAD]-bip[bip_dvAD]; bip[bip_pvAD] = bip[bip_dvAD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvAC])*0.0618)+(fabs(bip[bip_dvAD])*-0.05982)+(fabs(bip_dvE)*0.0206));

            inputSampleR *= bezier[bezierMore];
            bip_delta = inputSampleR; //delta can be just local and re-used
            if (inputSampleR != 0.0) {
                inputSampleR = fmax(fmin(inputSampleR,1.0),-1.0);
                inputSampleR *= 1.27;
                double weave = sqrt(fabs(inputSampleR));
                double spiral = inputSampleR * weave;
                long double Y = spiral; Y *= Y;
                long double temp = spiral * Y;
                spiral -= temp*0.16666666666666666666666666666666666; temp *= Y;//-8 dB
                spiral += temp*0.00833333333333333333333333333333333; temp *= Y;//-20 dB
                spiral -= temp*0.00019841269841269841269841269841269; temp *= Y;//-37 dB
                spiral += temp*0.00000275573192239858906525573192239; temp *= Y;//-55 dB
                spiral -= temp*0.00000002505210838544171877521083854; temp *= Y;//-76 dB
                spiral += temp*0.00000000016059043836821614599392377; temp *= Y;//-108 dB
                spiral -= temp*0.00000000000076471637318198164759011; temp *= Y;//-121 dB
                spiral += temp*0.00000000000000281145725434552076319; temp *= Y;//-146 dB
                spiral -= temp*0.00000000000000000822063524662432971; temp *= Y;//-171 dB
                spiral += temp*0.00000000000000000001957294106339126;           //-197 dB
                inputSampleR = spiral / weave; //purest form is full Spiral
            } //version of Weave in ConsoleX uses A and B for derivatives, not L and R
            bip[bip_dvBA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvBB] = bip[bip_pvBA]-bip[bip_dvBA]; bip[bip_pvBA] = bip[bip_dvBA];//velocity
            bip[bip_dvBC] = bip[bip_pvBB]-bip[bip_dvBB]; bip[bip_pvBB] = bip[bip_dvBB];//acceleration
            bip[bip_dvBD] = bip[bip_pvBC]-bip[bip_dvBC]; bip[bip_pvBC] = bip[bip_dvBC];//jerk
            bip_dvE = bip[bip_pvBD]-bip[bip_dvBD]; bip[bip_pvBD] = bip[bip_dvBD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvBC])*0.0618)+(fabs(bip[bip_dvBD])*-0.05982)+(fabs(bip_dvE)*0.0206));
        } //end Weave

        //begin BezEQ3 Post
        highL = inputSampleL;
        highR = inputSampleR;
        bezEQ[bez_HMcycle][2] += HMderez;
        bezEQ[bez_HMAL][2] += (highL * HMderez);
        bezEQ[bez_HMAR][2] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][2] > HMtrim) {
            bezEQ[bez_HMDL][2] = bezEQ[bez_HMCL][2]; bezEQ[bez_HMCL][2] = bezEQ[bez_HMBL][2];
            bezEQ[bez_HMBL][2] = bezEQ[bez_HMAL][2]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][2] = 0.0;
            bezEQ[bez_HMDR][2] = bezEQ[bez_HMCR][2]; bezEQ[bez_HMCR][2] = bezEQ[bez_HMBR][2];
            bezEQ[bez_HMBR][2] = bezEQ[bez_HMAR][2]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][2] = 0.0;
            bezEQ[bez_HMcycle][2] = 0.0;
        }
        X = bezEQ[bez_HMcycle][2];
        hmidL = bezEQ[bez_HMCL][2]+(bezEQ[bez_HMDL][2]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][2]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        hmidR = bezEQ[bez_HMCR][2]+(bezEQ[bez_HMDR][2]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][2]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][2] += Mderez;
        bezEQ[bez_MAL][2] += (hmidL * Mderez);
        bezEQ[bez_MAR][2] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][2] > Mtrim) {
            bezEQ[bez_MDL][2] = bezEQ[bez_MCL][2]; bezEQ[bez_MCL][2] = bezEQ[bez_MBL][2];
            bezEQ[bez_MBL][2] = bezEQ[bez_MAL][2]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][2] = 0.0;
            bezEQ[bez_MDR][2] = bezEQ[bez_MCR][2]; bezEQ[bez_MCR][2] = bezEQ[bez_MBR][2];
            bezEQ[bez_MBR][2] = bezEQ[bez_MAR][2]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][2] = 0.0;
            bezEQ[bez_Mcycle][2] = 0.0;
        }
        X = bezEQ[bez_Mcycle][2];
        lmidL = bezEQ[bez_MCL][2]+(bezEQ[bez_MDL][2]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][2]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        lmidR = bezEQ[bez_MCR][2]+(bezEQ[bez_MDR][2]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][2]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][2] += LMderez;
        bezEQ[bez_LMAL][2] += (lmidL * LMderez);
        bezEQ[bez_LMAR][2] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][2] > LMtrim) {
            bezEQ[bez_LMDL][2] = bezEQ[bez_LMCL][2]; bezEQ[bez_LMCL][2] = bezEQ[bez_LMBL][2];
            bezEQ[bez_LMBL][2] = bezEQ[bez_LMAL][2]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][2] = 0.0;
            bezEQ[bez_LMDR][2] = bezEQ[bez_LMCR][2]; bezEQ[bez_LMCR][2] = bezEQ[bez_LMBR][2];
            bezEQ[bez_LMBR][2] = bezEQ[bez_LMAR][2]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][2] = 0.0;
            bezEQ[bez_LMcycle][2] = 0.0;
        }
        X = bezEQ[bez_LMcycle][2];
        bassL = bezEQ[bez_LMCL][2]+(bezEQ[bez_LMDL][2]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][2]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        bassR = bezEQ[bez_LMCR][2]+(bezEQ[bez_LMDR][2]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][2]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        //end BezEQ3 Post
        
        //begin Lowpass/Highpass
        if (bezier[bezierHFreq] > 0.0) {
            double lowSampleL = inputSampleL;
            double lowSampleR = inputSampleR;
            for(int count = 0; count < Hpoles; count++) {
                iirHAngleL[count] = (iirHAngleL[count]*(1.0-bezier[bezierHFreq]))+((lowSampleL-iirHPositionL[count])*bezier[bezierHFreq]);
                lowSampleL = ((iirHPositionL[count]+(iirHAngleL[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleL*bezier[bezierHFreq]);
                iirHPositionL[count] = ((iirHPositionL[count]+(iirHAngleL[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleL*bezier[bezierHFreq]);
                inputSampleL -= (lowSampleL * (1.0/(double)Hpoles));//left
                iirHAngleR[count] = (iirHAngleR[count]*(1.0-bezier[bezierHFreq]))+((lowSampleR-iirHPositionR[count])*bezier[bezierHFreq]);
                lowSampleR = ((iirHPositionR[count]+(iirHAngleR[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleR*bezier[bezierHFreq]);
                iirHPositionR[count] = ((iirHPositionR[count]+(iirHAngleR[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleR*bezier[bezierHFreq]);
                inputSampleR -= (lowSampleR * (1.0/(double)Hpoles));//right
            } hBypass = false; //the highpass
        } else {
            if (!hBypass) {
                hBypass = true;
                for(int count = 0; count < 29; count++) {
                    iirHPositionL[count] = 0.0;
                    iirHAngleL[count] = 0.0;
                    iirHPositionR[count] = 0.0;
                    iirHAngleR[count] = 0.0;
                }//blank out highpass if jut switched off
            }
        }
        if (bezier[bezierLFreq] < 1.0) {
            for(int count = 0; count < Lpoles; count++) {
                iirLAngleL[count] = (iirLAngleL[count]*(1.0-bezier[bezierLFreq]))+((inputSampleL-iirLPositionL[count])*bezier[bezierLFreq]);
                inputSampleL = ((iirLPositionL[count]+(iirLAngleL[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleL*bezier[bezierLFreq]);
                iirLPositionL[count] = ((iirLPositionL[count]+(iirLAngleL[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleL*bezier[bezierLFreq]);//left
                iirLAngleR[count] = (iirLAngleR[count]*(1.0-bezier[bezierLFreq]))+((inputSampleR-iirLPositionR[count])*bezier[bezierLFreq]);
                inputSampleR = ((iirLPositionR[count]+(iirLAngleR[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleR*bezier[bezierLFreq]);
                iirLPositionR[count] = ((iirLPositionR[count]+(iirLAngleR[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleR*bezier[bezierLFreq]);//right
            } lBypass = false; //the lowpass
        } else {
            if (!lBypass) {
                lBypass = true;
                for(int count = 0; count < 29; count++) {
                    iirLPositionL[count] = 0.0;
                    iirLAngleL[count] = 0.0;
                    iirLPositionR[count] = 0.0;
                    iirLAngleR[count] = 0.0;
                }//blank out lowpass if just switched off
            }
        }
        //end Lowpass/Highpass
        
        if (fabs(inputSampleL) > maxFader) maxFader = fabs(inputSampleL);
        if (fabs(inputSampleR) > maxFader) maxFader = fabs(inputSampleR);
        
        inputSampleL *= bezier[bezierGainL];
        inputSampleR *= bezier[bezierGainR];
        //applies pan section, and smoothed fader gain
        
        bool audioPlaying = true;
        auto playHead = getPlayHead();
        if (playHead) {auto playHeadPos = playHead->getPosition();
            if(playHeadPos->getTimeInSeconds() > 0.0) audioPlaying = playHeadPos->getIsPlaying();
            //I have a sound editor that generates a playhead but won't do IsPlaying. This checks if the DAW speaks 'playHead'.
        }
        if (audioPlaying) {
            //begin bar display section
            if ((fabs(inputSampleL-previousLeft)/32000.0f)*getSampleRate() > slewLeft) slewLeft =  (fabs(inputSampleL-previousLeft)/32000.0f)*getSampleRate();
            if ((fabs(inputSampleR-previousRight)/32000.0f)*getSampleRate() > slewRight) slewRight = (fabs(inputSampleR-previousRight)/32000.0f)*getSampleRate();
            previousLeft = inputSampleL; previousRight = inputSampleR; //slew measurement is NOT rectified
            double rectifiedL = fabs(inputSampleL);
            double rectifiedR = fabs(inputSampleR);
            if (rectifiedL > peakLeft) peakLeft = rectifiedL;
            if (rectifiedR > peakRight) peakRight = rectifiedR;
            windowCount++;
            zeroLeft += zeroCrossScale;
            if (longestZeroLeft < zeroLeft) longestZeroLeft = zeroLeft;
            if (wasPositiveL && inputSampleL < 0.0) {
                wasPositiveL = false;
                zeroLeft = 0.0;
            } else if (!wasPositiveL && inputSampleL > 0.0) {
                wasPositiveL = true;
                zeroLeft = 0.0;
            }
            zeroRight += zeroCrossScale;
            if (longestZeroRight < zeroRight) longestZeroRight = zeroRight;
            if (wasPositiveR && inputSampleR < 0.0) {
                wasPositiveR = false;
                zeroRight = 0.0;
            } else if (!wasPositiveR && inputSampleR > 0.0) {
                wasPositiveR = true;
                zeroRight = 0.0;
            } //end bar display section
        }
        
        if (pluginMode <= 0.5) {
            double dryL = inputSampleL;
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
            bip[bip_dvLA] = dryL - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA]-bip[bip_dvLA]; bip[bip_pvLA] = bip[bip_dvLA];//velocity
            bip[bip_dvLC] = bip[bip_pvLB]-bip[bip_dvLB]; bip[bip_pvLB] = bip[bip_dvLB];//acceleration
            bip[bip_dvLD] = bip[bip_pvLC]-bip[bip_dvLC]; bip[bip_pvLC] = bip[bip_dvLC];//jerk
            double bip_dvE = bip[bip_pvLD]-bip[bip_dvLD]; bip[bip_pvLD] = bip[bip_dvLD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvLC])*0.0618)+(fabs(bip[bip_dvLD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            double dryR = inputSampleR;
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
            bip[bip_dvRA] = dryR - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA]-bip[bip_dvRA]; bip[bip_pvRA] = bip[bip_dvRA];//velocity
            bip[bip_dvRC] = bip[bip_pvRB]-bip[bip_dvRB]; bip[bip_pvRB] = bip[bip_dvRB];//acceleration
            bip[bip_dvRD] = bip[bip_pvRC]-bip[bip_dvRC]; bip[bip_pvRC] = bip[bip_dvRC];//jerk
            bip_dvE = bip[bip_pvRD]-bip[bip_dvRD]; bip[bip_pvRD] = bip[bip_dvRD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvRC])*0.0618)+(fabs(bip[bip_dvRD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            if (pluginMode > 0.0f) {
                inputSampleL = (inputSampleL*(1.0-(pluginMode*2.0)))+(dryL*(pluginMode*2.0));
                inputSampleR = (inputSampleR*(1.0-(pluginMode*2.0)))+(dryR*(pluginMode*2.0));
            }
        } //engage pluginMode, channel version
        
        //begin 32 bit stereo floating point dither
        int expon; frexpf((float)inputSampleL, &expon);
        fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
        inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
        frexpf((float)inputSampleR, &expon);
        fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
        inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
        //end 32 bit stereo floating point dither
        
        *outL = (float)inputSampleL;
        *outR = (float)inputSampleR;
    }


    if (windowCount > rmsSize)
    {
        AudioToUIMessage msg; //define the thing we're telling JUCE
        msg.what = AudioToUIMessage::SLEW_LEFT; msg.newValue = (float)slewLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::SLEW_RIGHT; msg.newValue = (float)slewRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_LEFT; msg.newValue = (float)sqrt(peakLeft); audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_RIGHT; msg.newValue = (float)sqrt(peakRight); audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_LEFT; msg.newValue = (float)longestZeroLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_RIGHT; msg.newValue = (float)longestZeroRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_INPUT; msg.newValue = (float)cbrt(maxTrim); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_DISPLAY; msg.newValue = (float)params[KNOBDIS]->get(); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_COMP; msg.newValue = (float)cbrt(maxComp); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_TAPE; msg.newValue = (float)cbrt(maxTape); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_OUTPUT; msg.newValue = (float)fmin(maxFader*maxFader*maxFader,1.0f); audioToUI.push(msg);
        msg.what = AudioToUIMessage::INCREMENT; msg.newValue = 1200.0f; audioToUI.push(msg);
        slewLeft = 0.0;
        slewRight = 0.0;
        peakLeft = 0.0;
        peakRight = 0.0;
        zeroLeft = 0.0;
        zeroRight = 0.0;
        longestZeroLeft = 0.0;
        longestZeroRight = 0.0;
        maxTrim = 0.0f;
        maxComp = 0.0f;
        maxTape = 0.0f;
        maxFader = 0.0f;
        windowCount = 0;
    }
}

//==============================================================================

void PluginProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    if (!(getBus(false, 0)->isEnabled() && getBus(true, 0)->isEnabled())) return;
    auto mainOutput = getBusBuffer(buffer, false, 0); //if we have audio busses at all,
    auto mainInput = getBusBuffer(buffer, true, 0); //they're now mainOutput and mainInput.
    UIToAudioMessage uim;
    while (uiToAudio.pop(uim)) {
        switch (uim.what) {
        case UIToAudioMessage::NEW_VALUE: params[uim.which]->setValueNotifyingHost(params[uim.which]->convertTo0to1(uim.newValue)); break;
        case UIToAudioMessage::BEGIN_EDIT: params[uim.which]->beginChangeGesture(); break;
        case UIToAudioMessage::END_EDIT: params[uim.which]->endChangeGesture(); break;
        }
    } //Handle inbound messages from the UI thread
    
    double rmsSize = (1881.0 / 44100.0)*getSampleRate(); //higher is slower with larger RMS buffers
    double zeroCrossScale = (1.0 / getSampleRate())*44100.0;
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();
    
    double bezierRez = fmax(pow((1.0-params[KNOBSMO]->get())*0.25,3.0)/overallscale,0.00001);
    int stepped = 999999; if (bezierRez > 0.000001) stepped = (int)(1.0/bezierRez); bezierRez = 0.99999999/stepped;
    const double bezierTrim = 1.0-(bezierRez*((double)stepped/(stepped+1.0)));
    //manages the overall Bezier control smoothing system plugin-wide
    int spacing = (int)floor(overallscale*2.0);
    if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
    const double gainTrim = params[KNOBTRM]->get()*2.0;
    const double trebleRef =  params[KNOBHIF]->get()-0.3;
    const double highmidRef =  params[KNOBHMF]->get()-0.5;
    const double lowmidRef =  params[KNOBLMF]->get()-0.7;
    const double bassRef =  params[KNOBBSF]->get()-0.9;
    double HMderez = 0.75 + (trebleRef*0.25);
    double LMderez = 0.25 + (bassRef*0.25);
    double Mderez = ((HMderez+LMderez+highmidRef+lowmidRef)*0.25);
    HMderez /= overallscale; Mderez /= overallscale; LMderez /= overallscale;
    if (HMderez > 1.0) HMderez = 1.0;
    stepped = 999999; if (HMderez > 0.000001) stepped = (int)(1.0/HMderez);
    HMderez = 0.99999999 / stepped;
    const double HMtrim = 1.0-(HMderez*((double)stepped/(stepped+1.0)));
    stepped = 999999; if (Mderez > 0.000001) stepped = (int)(1.0/Mderez);
    Mderez = 0.99999999 / stepped;
    const double Mtrim = 1.0-(Mderez*((double)stepped/(stepped+1.0)));
    stepped = 999999; if (LMderez > 0.000001) stepped = (int)(1.0/LMderez);
    LMderez = 0.99999999 / stepped;
    const double LMtrim = 1.0-(LMderez*((double)stepped/(stepped+1.0)));
    //BezEQ3 stepped elements
    double bezRez = fmax(pow((1.0-params[KNOBATK]->get())*0.4,4.0)/overallscale,0.0001);
    bezRez /= (2.0/pow(overallscale,0.5-((overallscale-1.0)*0.0375)));
    stepped = 999999; if (bezRez > 0.000001) stepped = (int)(1.0/bezRez);
    bezRez = 0.99999999 / stepped;
    const double bezTrim = 1.0-(bezRez*((double)stepped/(stepped+1.0)));
    double sloRez = fmax(pow((1.0-params[KNOBRLS]->get())*0.4,4.0)/overallscale,0.00001);
    sloRez /= (2.0/pow(overallscale,0.5-((overallscale-1.0)*0.0375)));
    double sqrThresh = 1.0; //recalculated in bezier section
    double bezThresh = bezier[bezierThresh];
    double invDry = bezier[bezierRatio];
    bool compress = (params[KNOBRAT]->get() > 0.499999);
    bool compBypass = (params[KNOBTHR]->get() > 0.9999);
    bool tapeBypass = (params[KNOBMOR]->get() < 0.0001);
    double pluginMode = params[KNOBMDE]->get();
    //Dynamics3 stepped elements
    const int Lpoles = (int)pow(params[KNOBLPQ]->get()*3.0,3.0)+1;
    const int Hpoles = (int)pow(params[KNOBHPQ]->get()*3.0,3.0)+1; //1-28 pole filter
    //Lowpass/Highpass stepped elements
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto outL = mainOutput.getWritePointer(0, i);
        auto outR = mainOutput.getWritePointer(1, i);
        auto inL = mainInput.getReadPointer(0, i); //in isBussesLayoutSupported, we have already
        auto inR = mainInput.getReadPointer(1, i); //specified that we can only be stereo and never mono
        double inputSampleL = *inL;
        double inputSampleR = *inR;
        if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
        
        if (pluginMode > 0.5) {
            double dryL = inputSampleL;
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949));
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949));
            bip[bip_dvLA] = dryL - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA]-bip[bip_dvLA]; bip[bip_pvLA] = bip[bip_dvLA];//velocity
            bip[bip_dvLC] = bip[bip_pvLB]-bip[bip_dvLB]; bip[bip_pvLB] = bip[bip_dvLB];//acceleration
            bip[bip_dvLD] = bip[bip_pvLC]-bip[bip_dvLC]; bip[bip_pvLC] = bip[bip_dvLC];//jerk
            double bip_dvE = bip[bip_pvLD]-bip[bip_dvLD]; bip[bip_pvLD] = bip[bip_dvLD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvLC])*0.0618)+(fabs(bip[bip_dvLD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            double dryR = inputSampleR;
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949));
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949));
            bip[bip_dvRA] = dryR - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA]-bip[bip_dvRA]; bip[bip_pvRA] = bip[bip_dvRA];//velocity
            bip[bip_dvRC] = bip[bip_pvRB]-bip[bip_dvRB]; bip[bip_pvRB] = bip[bip_dvRB];//acceleration
            bip[bip_dvRD] = bip[bip_pvRC]-bip[bip_dvRC]; bip[bip_pvRC] = bip[bip_dvRC];//jerk
            bip_dvE = bip[bip_pvRD]-bip[bip_dvRD]; bip[bip_pvRD] = bip[bip_dvRD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvRC])*0.0618)+(fabs(bip[bip_dvRD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            if (pluginMode < 1.0f) {
                inputSampleL = (dryL*(1.0-((pluginMode-0.5)*2.0))) + (inputSampleL*((pluginMode-0.5)*2.0));
                inputSampleR = (dryR*(1.0-((pluginMode-0.5)*2.0))) + (inputSampleR*((pluginMode-0.5)*2.0));
            }
        } //engage pluginMode, buss
        
        //begin Bezier control smoothing engine
        bezier[bezier_cycle] += bezierRez;
        if (bezier[bezier_cycle] > bezierTrim) {bezier[bezier_cycle] = 0.0;
            bezier[bezierHigh_C] =  bezier[bezierHigh_B];  bezier[bezierHigh_B] =  bezier[bezierHigh_A];
            bezier[bezierHmid_C] =  bezier[bezierHmid_B];  bezier[bezierHmid_B] =  bezier[bezierHmid_A];
            bezier[bezierLmid_C] =  bezier[bezierLmid_B];  bezier[bezierLmid_B] =  bezier[bezierLmid_A];
            bezier[bezierBass_C] =  bezier[bezierBass_B];  bezier[bezierBass_B] =  bezier[bezierBass_A];
            bezier[bezierHighQ_C] =  bezier[bezierHighQ_B];  bezier[bezierHighQ_B] =  bezier[bezierHighQ_A];
            bezier[bezierMidQ_C] =  bezier[bezierMidQ_B];  bezier[bezierMidQ_B] =  bezier[bezierMidQ_A];
            bezier[bezierLowQ_C] =  bezier[bezierLowQ_B];  bezier[bezierLowQ_B] =  bezier[bezierLowQ_A];
            bezier[bezierThresh_C]= bezier[bezierThresh_B];bezier[bezierThresh_B]= bezier[bezierThresh_A];
            bezier[bezierRatio_C] = bezier[bezierRatio_B]; bezier[bezierRatio_B] = bezier[bezierRatio_A];
            bezier[bezierMore_C] =  bezier[bezierMore_B];  bezier[bezierMore_B] =  bezier[bezierMore_A];
            bezier[bezierLFreq_C] = bezier[bezierLFreq_B]; bezier[bezierLFreq_B] = bezier[bezierLFreq_A];
            bezier[bezierHFreq_C] = bezier[bezierHFreq_B]; bezier[bezierHFreq_B] = bezier[bezierHFreq_A];
            bezier[bezierGainL_C] = bezier[bezierGainL_B]; bezier[bezierGainL_B] = bezier[bezierGainL_A];
            bezier[bezierGainR_C] = bezier[bezierGainR_B]; bezier[bezierGainR_B] = bezier[bezierGainR_A];
            //one of these bucket brigade lines for every smoothed control
            //begin smoothed control calculations
            bezier[bezierHigh_A] = (pow(params[KNOBHIG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierHmid_A] = (pow(params[KNOBHMG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierLmid_A] = (pow(params[KNOBLMG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierBass_A] = (pow(params[KNOBBSG]->get()+0.5,3.0)+2.0)*0.3333333;
            bezier[bezierHighQ_A] = 0.84+((highmidRef-trebleRef)*0.1666666);
            bezier[bezierMidQ_A] = 0.84+((lowmidRef-highmidRef)*0.1666666);
            bezier[bezierLowQ_A] = 0.84+((bassRef-lowmidRef)*0.1666666);
            //BezEQ3
            bezier[bezierThresh_A] = pow(params[KNOBTHR]->get()+0.6180339887498949,2.0)*1.6180339887498949;
            sqrThresh = sqrt(bezier[bezierThresh_A]);
            bezier[bezierRatio_A] = pow(1.0-(fabs(params[KNOBRAT]->get()-0.5)*2.0),1.6180339887498949);
            //Dynamics3
            bezier[bezierMore_A] = (params[KNOBMOR]->get()*3.0)+1.0;
            //TapeHack
            bezier[bezierLFreq_A] = pow(fmax(params[KNOBLOP]->get(),0.002),overallscale); //the lowpass
            bezier[bezierHFreq_A] = pow(params[KNOBHIP]->get(),overallscale+2.0); //the highpass
            //Cabs2
            double gain = pow(params[KNOBFAD]->get(),2.0)*1.414213562373094; //Pan will pad this
            bezier[bezierGainL_A] = gain*sin(M_PI_2-(params[KNOBPAN]->get()*M_PI_2));
            bezier[bezierGainR_A] = gain*sin(params[KNOBPAN]->get()*M_PI_2);
            //Fader and Pan
            //end expensive control calculations
        }
        const double lerp = bezier[bezier_cycle]; //with this many controls we'll make temp variables
        const double La = (1.0-lerp)*(1.0-lerp); const double Lb = 2.0*(1.0-lerp)*lerp; const double Lc = lerp*lerp;
        bezier[bezierHigh] = (bezier[bezierHigh_B] + (bezier[bezierHigh_C]*La) + (bezier[bezierHigh_B]*Lb) + (bezier[bezierHigh_A]*Lc))*0.5;
        bezier[bezierHmid] = (bezier[bezierHmid_B] + (bezier[bezierHmid_C]*La) + (bezier[bezierHmid_B]*Lb) + (bezier[bezierHmid_A]*Lc))*0.5;
        bezier[bezierLmid] = (bezier[bezierLmid_B] + (bezier[bezierLmid_C]*La) + (bezier[bezierLmid_B]*Lb) + (bezier[bezierLmid_A]*Lc))*0.5;
        bezier[bezierBass] = (bezier[bezierBass_B] + (bezier[bezierBass_C]*La) + (bezier[bezierBass_B]*Lb) + (bezier[bezierBass_A]*Lc))*0.5;
        bezier[bezierHighQ] = (bezier[bezierHighQ_B] + (bezier[bezierHighQ_C]*La) + (bezier[bezierHighQ_B]*Lb) + (bezier[bezierHighQ_A]*Lc))*0.5;
        bezier[bezierMidQ] = (bezier[bezierMidQ_B] + (bezier[bezierMidQ_C]*La) + (bezier[bezierMidQ_B]*Lb) + (bezier[bezierMidQ_A]*Lc))*0.5;
        bezier[bezierLowQ] = (bezier[bezierLowQ_B] + (bezier[bezierLowQ_C]*La) + (bezier[bezierLowQ_B]*Lb) + (bezier[bezierLowQ_A]*Lc))*0.5;
        bezier[bezierThresh] = (bezier[bezierThresh_B] + (bezier[bezierThresh_C]*La) + (bezier[bezierThresh_B]*Lb) + (bezier[bezierThresh_A]*Lc))*0.5;
        bezier[bezierRatio] = (bezier[bezierRatio_B] + (bezier[bezierRatio_C]*La) + (bezier[bezierRatio_B]*Lb) + (bezier[bezierRatio_A]*Lc))*0.5;
        bezier[bezierMore] = (bezier[bezierMore_B] + (bezier[bezierMore_C]*La) + (bezier[bezierMore_B]*Lb) + (bezier[bezierMore_A]*Lc))*0.5;
        bezier[bezierLFreq] = (bezier[bezierLFreq_B] + (bezier[bezierLFreq_C]*La) + (bezier[bezierLFreq_B]*Lb) + (bezier[bezierLFreq_A]*Lc))*0.5;
        bezier[bezierHFreq] = (bezier[bezierHFreq_B] + (bezier[bezierHFreq_C]*La) + (bezier[bezierHFreq_B]*Lb) + (bezier[bezierHFreq_A]*Lc))*0.5;
        bezier[bezierGainL] = (bezier[bezierGainL_B] + (bezier[bezierGainL_C]*La) + (bezier[bezierGainL_B]*Lb) + (bezier[bezierGainL_A]*Lc))*0.5;
        bezier[bezierGainR] = (bezier[bezierGainR_B] + (bezier[bezierGainR_C]*La) + (bezier[bezierGainR_B]*Lb) + (bezier[bezierGainR_A]*Lc))*0.5;
        //end Bezier control smoothing engine
        
        if (fabs(inputSampleL) > maxTrim) maxTrim = fabs(inputSampleL);
        if (fabs(inputSampleR) > maxTrim) maxTrim = fabs(inputSampleR);
        //begin Trim switch
        if (gainTrim != 1.0) {inputSampleL *= gainTrim; inputSampleR *= gainTrim;}
        //end Trim control
        
        
        //begin BezEQ3 Pre
        double highL = inputSampleL;
        double highR = inputSampleR;
        bezEQ[bez_HMcycle][0] += HMderez;
        bezEQ[bez_HMAL][0] += (highL * HMderez);
        bezEQ[bez_HMAR][0] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][0] > HMtrim) {
            bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0]; bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0];
            bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][0] = 0.0;
            bezEQ[bez_HMDR][0] = bezEQ[bez_HMCR][0]; bezEQ[bez_HMCR][0] = bezEQ[bez_HMBR][0];
            bezEQ[bez_HMBR][0] = bezEQ[bez_HMAR][0]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][0] = 0.0;
            bezEQ[bez_HMcycle][0] = 0.0;
        }
        double X = bezEQ[bez_HMcycle][0];
        double hmidL = bezEQ[bez_HMCL][0]+(bezEQ[bez_HMDL][0]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][0]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        double hmidR = bezEQ[bez_HMCR][0]+(bezEQ[bez_HMDR][0]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][0]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][0] += Mderez;
        bezEQ[bez_MAL][0] += (hmidL * Mderez);
        bezEQ[bez_MAR][0] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][0] > Mtrim) {
            bezEQ[bez_MDL][0] = bezEQ[bez_MCL][0]; bezEQ[bez_MCL][0] = bezEQ[bez_MBL][0];
            bezEQ[bez_MBL][0] = bezEQ[bez_MAL][0]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][0] = 0.0;
            bezEQ[bez_MDR][0] = bezEQ[bez_MCR][0]; bezEQ[bez_MCR][0] = bezEQ[bez_MBR][0];
            bezEQ[bez_MBR][0] = bezEQ[bez_MAR][0]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][0] = 0.0;
            bezEQ[bez_Mcycle][0] = 0.0;
        }
        X = bezEQ[bez_Mcycle][0];
        double lmidL = bezEQ[bez_MCL][0]+(bezEQ[bez_MDL][0]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][0]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        double lmidR = bezEQ[bez_MCR][0]+(bezEQ[bez_MDR][0]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][0]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][0] += LMderez;
        bezEQ[bez_LMAL][0] += (lmidL * LMderez);
        bezEQ[bez_LMAR][0] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][0] > LMtrim) {
            bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0]; bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0];
            bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][0] = 0.0;
            bezEQ[bez_LMDR][0] = bezEQ[bez_LMCR][0]; bezEQ[bez_LMCR][0] = bezEQ[bez_LMBR][0];
            bezEQ[bez_LMBR][0] = bezEQ[bez_LMAR][0]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][0] = 0.0;
            bezEQ[bez_LMcycle][0] = 0.0;
        }
        X = bezEQ[bez_LMcycle][0];
        double bassL = bezEQ[bez_LMCL][0]+(bezEQ[bez_LMDL][0]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][0]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][0]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        double bassR = bezEQ[bez_LMCR][0]+(bezEQ[bez_LMDR][0]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][0]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][0]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        //end BezEQ3 Pre
        
        if (!compBypass) {
            //begin Dynamics3
            double dryCompL = inputSampleL;
            double dryCompR = inputSampleR;
            if (compress) {
                inputSampleL *= (bezComp[bez_comp]/bezThresh);
                inputSampleR *= (bezComp[bez_comp]/bezThresh);
            } else {
                inputSampleL /= bezThresh;
                inputSampleR /= bezThresh;
            }
            double ctrl = fmin(fmax(fabs(inputSampleL),fabs(inputSampleR)),sqrThresh*bezComp[bez_comp]*0.6180339887498949);
            bezComp[bez_min] = fmax(bezComp[bez_min]-sloRez,ctrl);
            bezComp[bez_Ctrl] += (bezComp[bez_min] * bezRez);
            bezComp[bez_cycle] += bezRez;
            if (bezComp[bez_cycle] > bezTrim) {bezComp[bez_cycle] = 0.0;
                bezComp[bez_C] = bezComp[bez_B]; bezComp[bez_B] = bezComp[bez_A];
                bezComp[bez_A] = bezComp[bez_Ctrl]; bezComp[bez_Ctrl] = 0.0;}
            X = bezComp[bez_cycle];
            bezComp[bez_comp] = bezComp[bez_B]+(bezComp[bez_C]*(1.0-X)*(1.0-X))+(bezComp[bez_B]*2.0*(1.0-X)*X)+(bezComp[bez_A]*X*X);
            if (bezComp[bez_comp] > maxComp) maxComp = bezComp[bez_comp]; //assign the blinkenlight value
            bezComp[bez_comp] = ((1.0-(fmin(bezComp[bez_comp],0.9999))));
            if (compress) {
                inputSampleL = inputSampleL*(1.0-invDry)*bezComp[bez_comp]*bezThresh;
                inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
                double addtwo = inputSampleL * inputSampleL;
                double empower = inputSampleL * addtwo; // inputSampleL to the third power
                inputSampleL -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleL += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleL -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleL += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleL -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleL += dryCompL*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
                inputSampleR = inputSampleR*(1.0-invDry)*bezComp[bez_comp]*bezThresh;
                inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
                addtwo = inputSampleR * inputSampleR;
                empower = inputSampleR * addtwo; // inputSampleR to the third power
                inputSampleR -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleR += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleR -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleR += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleR -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleR += dryCompR*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
            } else {
                inputSampleL = ((inputSampleL/(0.1+bezThresh))*(1.0-invDry))/bezComp[bez_comp];
                inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
                double addtwo = inputSampleL * inputSampleL;
                double empower = inputSampleL * addtwo; // inputSampleL to the third power
                inputSampleL -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleL += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleL -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleL += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleL -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleL += dryCompL*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
                inputSampleR = ((inputSampleR/(0.1+bezThresh))*(1.0-invDry))/bezComp[bez_comp];
                inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
                addtwo = inputSampleR * inputSampleR;
                empower = inputSampleR * addtwo; // inputSampleR to the third power
                inputSampleR -= (empower / 6.0); empower *= addtwo; // to the fifth power
                inputSampleR += (empower / 69.0); empower *= addtwo; //seventh
                inputSampleR -= (empower / 2530.08); empower *= addtwo; //ninth
                inputSampleR += (empower / 224985.6); empower *= addtwo; //eleventh
                inputSampleR -= (empower / 9979200.0f);
                //this is a degenerate form of a Taylor Series to approximate sin()
                inputSampleR += dryCompR*invDry*(1.0-(bezComp[bez_comp]*(1.0-invDry)));
            }
        } //end Dynamics3
        
        //begin BezEQ3 Mid
        highL = inputSampleL;
        highR = inputSampleR;
        bezEQ[bez_HMcycle][1] += HMderez;
        bezEQ[bez_HMAL][1] += (highL * HMderez);
        bezEQ[bez_HMAR][1] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][1] > HMtrim) {
            bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1]; bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1];
            bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][1] = 0.0;
            bezEQ[bez_HMDR][1] = bezEQ[bez_HMCR][1]; bezEQ[bez_HMCR][1] = bezEQ[bez_HMBR][1];
            bezEQ[bez_HMBR][1] = bezEQ[bez_HMAR][1]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][1] = 0.0;
            bezEQ[bez_HMcycle][1] = 0.0;
        }
        X = bezEQ[bez_HMcycle][1];
        hmidL = bezEQ[bez_HMCL][1]+(bezEQ[bez_HMDL][1]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][1]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        hmidR = bezEQ[bez_HMCR][1]+(bezEQ[bez_HMDR][1]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][1]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][1] += Mderez;
        bezEQ[bez_MAL][1] += (hmidL * Mderez);
        bezEQ[bez_MAR][1] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][1] > Mtrim) {
            bezEQ[bez_MDL][1] = bezEQ[bez_MCL][1]; bezEQ[bez_MCL][1] = bezEQ[bez_MBL][1];
            bezEQ[bez_MBL][1] = bezEQ[bez_MAL][1]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][1] = 0.0;
            bezEQ[bez_MDR][1] = bezEQ[bez_MCR][1]; bezEQ[bez_MCR][1] = bezEQ[bez_MBR][1];
            bezEQ[bez_MBR][1] = bezEQ[bez_MAR][1]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][1] = 0.0;
            bezEQ[bez_Mcycle][1] = 0.0;
        }
        X = bezEQ[bez_Mcycle][1];
        lmidL = bezEQ[bez_MCL][1]+(bezEQ[bez_MDL][1]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][1]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        lmidR = bezEQ[bez_MCR][1]+(bezEQ[bez_MDR][1]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][1]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][1] += LMderez;
        bezEQ[bez_LMAL][1] += (lmidL * LMderez);
        bezEQ[bez_LMAR][1] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][1] > LMtrim) {
            bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1]; bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1];
            bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][1] = 0.0;
            bezEQ[bez_LMDR][1] = bezEQ[bez_LMCR][1]; bezEQ[bez_LMCR][1] = bezEQ[bez_LMBR][1];
            bezEQ[bez_LMBR][1] = bezEQ[bez_LMAR][1]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][1] = 0.0;
            bezEQ[bez_LMcycle][1] = 0.0;
        }
        X = bezEQ[bez_LMcycle][1];
        bassL = bezEQ[bez_LMCL][1]+(bezEQ[bez_LMDL][1]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][1]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][1]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        bassR = bezEQ[bez_LMCR][1]+(bezEQ[bez_LMDR][1]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][1]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][1]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        
        //end BezEQ3 Mid
        
        //begin Weave
        if (!tapeBypass) {
            inputSampleL *= bezier[bezierMore];
            double bip_delta = inputSampleL; //delta can be just local and re-used
            if (inputSampleL != 0.0) {
                inputSampleL = fmax(fmin(inputSampleL,1.0),-1.0);
                inputSampleL *= 1.27;
                long double weave = sqrt(fabs(inputSampleL));
                long double spiral = inputSampleL * weave;
                long double Y = spiral; Y *= Y;
                long double temp = spiral * Y;
                spiral -= temp*0.16666666666666666666666666666666666; temp *= Y;//-8 dB
                spiral += temp*0.00833333333333333333333333333333333; temp *= Y;//-20 dB
                spiral -= temp*0.00019841269841269841269841269841269; temp *= Y;//-37 dB
                spiral += temp*0.00000275573192239858906525573192239; temp *= Y;//-55 dB
                spiral -= temp*0.00000002505210838544171877521083854; temp *= Y;//-76 dB
                spiral += temp*0.00000000016059043836821614599392377; temp *= Y;//-108 dB
                spiral -= temp*0.00000000000076471637318198164759011; temp *= Y;//-121 dB
                spiral += temp*0.00000000000000281145725434552076319; temp *= Y;//-146 dB
                spiral -= temp*0.00000000000000000822063524662432971; temp *= Y;//-171 dB
                spiral += temp*0.00000000000000000001957294106339126;           //-197 dB
                inputSampleL = spiral / weave; //purest form is full Spiral
            } //version of Weave in ConsoleX uses A and B for derivatives, not L and R
            bip[bip_dvAA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvAB] = bip[bip_pvAA]-bip[bip_dvAA]; bip[bip_pvAA] = bip[bip_dvAA];//velocity
            bip[bip_dvAC] = bip[bip_pvAB]-bip[bip_dvAB]; bip[bip_pvAB] = bip[bip_dvAB];//acceleration
            bip[bip_dvAD] = bip[bip_pvAC]-bip[bip_dvAC]; bip[bip_pvAC] = bip[bip_dvAC];//jerk
            double bip_dvE = bip[bip_pvAD]-bip[bip_dvAD]; bip[bip_pvAD] = bip[bip_dvAD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvAC])*0.0618)+(fabs(bip[bip_dvAD])*-0.05982)+(fabs(bip_dvE)*0.0206));

            inputSampleR *= bezier[bezierMore];
            bip_delta = inputSampleR; //delta can be just local and re-used
            if (inputSampleR != 0.0) {
                inputSampleR = fmax(fmin(inputSampleR,1.0),-1.0);
                inputSampleR *= 1.27;
                double weave = sqrt(fabs(inputSampleR));
                double spiral = inputSampleR * weave;
                long double Y = spiral; Y *= Y;
                long double temp = spiral * Y;
                spiral -= temp*0.16666666666666666666666666666666666; temp *= Y;//-8 dB
                spiral += temp*0.00833333333333333333333333333333333; temp *= Y;//-20 dB
                spiral -= temp*0.00019841269841269841269841269841269; temp *= Y;//-37 dB
                spiral += temp*0.00000275573192239858906525573192239; temp *= Y;//-55 dB
                spiral -= temp*0.00000002505210838544171877521083854; temp *= Y;//-76 dB
                spiral += temp*0.00000000016059043836821614599392377; temp *= Y;//-108 dB
                spiral -= temp*0.00000000000076471637318198164759011; temp *= Y;//-121 dB
                spiral += temp*0.00000000000000281145725434552076319; temp *= Y;//-146 dB
                spiral -= temp*0.00000000000000000822063524662432971; temp *= Y;//-171 dB
                spiral += temp*0.00000000000000000001957294106339126;           //-197 dB
                inputSampleR = spiral / weave; //purest form is full Spiral
            } //version of Weave in ConsoleX uses A and B for derivatives, not L and R
            bip[bip_dvBA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvBB] = bip[bip_pvBA]-bip[bip_dvBA]; bip[bip_pvBA] = bip[bip_dvBA];//velocity
            bip[bip_dvBC] = bip[bip_pvBB]-bip[bip_dvBB]; bip[bip_pvBB] = bip[bip_dvBB];//acceleration
            bip[bip_dvBD] = bip[bip_pvBC]-bip[bip_dvBC]; bip[bip_pvBC] = bip[bip_dvBC];//jerk
            bip_dvE = bip[bip_pvBD]-bip[bip_dvBD]; bip[bip_pvBD] = bip[bip_dvBD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvBC])*0.0618)+(fabs(bip[bip_dvBD])*-0.05982)+(fabs(bip_dvE)*0.0206));
        } //end Weave

        //begin BezEQ3 Post
        highL = inputSampleL;
        highR = inputSampleR;
        bezEQ[bez_HMcycle][2] += HMderez;
        bezEQ[bez_HMAL][2] += (highL * HMderez);
        bezEQ[bez_HMAR][2] += (highR * HMderez);
        if (bezEQ[bez_HMcycle][2] > HMtrim) {
            bezEQ[bez_HMDL][2] = bezEQ[bez_HMCL][2]; bezEQ[bez_HMCL][2] = bezEQ[bez_HMBL][2];
            bezEQ[bez_HMBL][2] = bezEQ[bez_HMAL][2]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAL][2] = 0.0;
            bezEQ[bez_HMDR][2] = bezEQ[bez_HMCR][2]; bezEQ[bez_HMCR][2] = bezEQ[bez_HMBR][2];
            bezEQ[bez_HMBR][2] = bezEQ[bez_HMAR][2]*(0.5-(HMderez*0.082));
            bezEQ[bez_HMAR][2] = 0.0;
            bezEQ[bez_HMcycle][2] = 0.0;
        }
        X = bezEQ[bez_HMcycle][2];
        hmidL = bezEQ[bez_HMCL][2]+(bezEQ[bez_HMDL][2]*(1.0-X)*(1.0-X));
        hmidL += (bezEQ[bez_HMCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBL][2]*X*X);
        hmidL *= bezier[bezierHighQ];
        highL -= hmidL; //final high and hmid
        hmidR = bezEQ[bez_HMCR][2]+(bezEQ[bez_HMDR][2]*(1.0-X)*(1.0-X));
        hmidR += (bezEQ[bez_HMCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_HMBR][2]*X*X);
        hmidR *= bezier[bezierHighQ];
        highR -= hmidR; //final high and hmid
        bezEQ[bez_Mcycle][2] += Mderez;
        bezEQ[bez_MAL][2] += (hmidL * Mderez);
        bezEQ[bez_MAR][2] += (hmidR * Mderez);
        if (bezEQ[bez_Mcycle][2] > Mtrim) {
            bezEQ[bez_MDL][2] = bezEQ[bez_MCL][2]; bezEQ[bez_MCL][2] = bezEQ[bez_MBL][2];
            bezEQ[bez_MBL][2] = bezEQ[bez_MAL][2]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAL][2] = 0.0;
            bezEQ[bez_MDR][2] = bezEQ[bez_MCR][2]; bezEQ[bez_MCR][2] = bezEQ[bez_MBR][2];
            bezEQ[bez_MBR][2] = bezEQ[bez_MAR][2]*(0.5-(Mderez*0.082));
            bezEQ[bez_MAR][2] = 0.0;
            bezEQ[bez_Mcycle][2] = 0.0;
        }
        X = bezEQ[bez_Mcycle][2];
        lmidL = bezEQ[bez_MCL][2]+(bezEQ[bez_MDL][2]*(1.0-X)*(1.0-X));
        lmidL += (bezEQ[bez_MCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_MBL][2]*X*X);
        lmidL *= bezier[bezierMidQ];
        hmidL -= lmidL; //final hmid and lmid
        lmidR = bezEQ[bez_MCR][2]+(bezEQ[bez_MDR][2]*(1.0-X)*(1.0-X));
        lmidR += (bezEQ[bez_MCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_MBR][2]*X*X);
        lmidR *= bezier[bezierMidQ];
        hmidR -= lmidR; //final hmid and lmid
        bezEQ[bez_LMcycle][2] += LMderez;
        bezEQ[bez_LMAL][2] += (lmidL * LMderez);
        bezEQ[bez_LMAR][2] += (lmidR * LMderez);
        if (bezEQ[bez_LMcycle][2] > LMtrim) {
            bezEQ[bez_LMDL][2] = bezEQ[bez_LMCL][2]; bezEQ[bez_LMCL][2] = bezEQ[bez_LMBL][2];
            bezEQ[bez_LMBL][2] = bezEQ[bez_LMAL][2]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAL][2] = 0.0;
            bezEQ[bez_LMDR][2] = bezEQ[bez_LMCR][2]; bezEQ[bez_LMCR][2] = bezEQ[bez_LMBR][2];
            bezEQ[bez_LMBR][2] = bezEQ[bez_LMAR][2]*(0.5-(LMderez*0.082));
            bezEQ[bez_LMAR][2] = 0.0;
            bezEQ[bez_LMcycle][2] = 0.0;
        }
        X = bezEQ[bez_LMcycle][2];
        bassL = bezEQ[bez_LMCL][2]+(bezEQ[bez_LMDL][2]*(1.0-X)*(1.0-X));
        bassL += (bezEQ[bez_LMCL][2]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBL][2]*X*X);
        bassL *= bezier[bezierLowQ];
        lmidL -= bassL; //final lmid and bass
        bassR = bezEQ[bez_LMCR][2]+(bezEQ[bez_LMDR][2]*(1.0-X)*(1.0-X));
        bassR += (bezEQ[bez_LMCR][2]*2.0*(1.0-X)*X)+(bezEQ[bez_LMBR][2]*X*X);
        bassR *= bezier[bezierLowQ];
        lmidR -= bassR; //final lmid and bass
        inputSampleL = (highL*bezier[bezierHigh])+(hmidL*bezier[bezierHmid])+(lmidL*bezier[bezierLmid])+(bassL*bezier[bezierBass]);
        inputSampleR = (highR*bezier[bezierHigh])+(hmidR*bezier[bezierHmid])+(lmidR*bezier[bezierLmid])+(bassR*bezier[bezierBass]);
        //end BezEQ3 Post
        
        //begin Lowpass/Highpass
        if (bezier[bezierHFreq] > 0.0) {
            double lowSampleL = inputSampleL;
            double lowSampleR = inputSampleR;
            for(int count = 0; count < Hpoles; count++) {
                iirHAngleL[count] = (iirHAngleL[count]*(1.0-bezier[bezierHFreq]))+((lowSampleL-iirHPositionL[count])*bezier[bezierHFreq]);
                lowSampleL = ((iirHPositionL[count]+(iirHAngleL[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleL*bezier[bezierHFreq]);
                iirHPositionL[count] = ((iirHPositionL[count]+(iirHAngleL[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleL*bezier[bezierHFreq]);
                inputSampleL -= (lowSampleL * (1.0/(double)Hpoles));//left
                iirHAngleR[count] = (iirHAngleR[count]*(1.0-bezier[bezierHFreq]))+((lowSampleR-iirHPositionR[count])*bezier[bezierHFreq]);
                lowSampleR = ((iirHPositionR[count]+(iirHAngleR[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleR*bezier[bezierHFreq]);
                iirHPositionR[count] = ((iirHPositionR[count]+(iirHAngleR[count]*bezier[bezierHFreq]))*(1.0-bezier[bezierHFreq]))+(lowSampleR*bezier[bezierHFreq]);
                inputSampleR -= (lowSampleR * (1.0/(double)Hpoles));//right
            } hBypass = false; //the highpass
        } else {
            if (!hBypass) {
                hBypass = true;
                for(int count = 0; count < 29; count++) {
                    iirHPositionL[count] = 0.0;
                    iirHAngleL[count] = 0.0;
                    iirHPositionR[count] = 0.0;
                    iirHAngleR[count] = 0.0;
                }//blank out highpass if jut switched off
            }
        }
        if (bezier[bezierLFreq] < 1.0) {
            for(int count = 0; count < Lpoles; count++) {
                iirLAngleL[count] = (iirLAngleL[count]*(1.0-bezier[bezierLFreq]))+((inputSampleL-iirLPositionL[count])*bezier[bezierLFreq]);
                inputSampleL = ((iirLPositionL[count]+(iirLAngleL[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleL*bezier[bezierLFreq]);
                iirLPositionL[count] = ((iirLPositionL[count]+(iirLAngleL[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleL*bezier[bezierLFreq]);//left
                iirLAngleR[count] = (iirLAngleR[count]*(1.0-bezier[bezierLFreq]))+((inputSampleR-iirLPositionR[count])*bezier[bezierLFreq]);
                inputSampleR = ((iirLPositionR[count]+(iirLAngleR[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleR*bezier[bezierLFreq]);
                iirLPositionR[count] = ((iirLPositionR[count]+(iirLAngleR[count]*bezier[bezierLFreq]))*(1.0-bezier[bezierLFreq]))+(inputSampleR*bezier[bezierLFreq]);//right
            } lBypass = false; //the lowpass
        } else {
            if (!lBypass) {
                lBypass = true;
                for(int count = 0; count < 29; count++) {
                    iirLPositionL[count] = 0.0;
                    iirLAngleL[count] = 0.0;
                    iirLPositionR[count] = 0.0;
                    iirLAngleR[count] = 0.0;
                }//blank out lowpass if just switched off
            }
        }
        //end Lowpass/Highpass
        
        if (fabs(inputSampleL) > maxFader) maxFader = fabs(inputSampleL);
        if (fabs(inputSampleR) > maxFader) maxFader = fabs(inputSampleR);
        
        inputSampleL *= bezier[bezierGainL];
        inputSampleR *= bezier[bezierGainR];
        //applies pan section, and smoothed fader gain
        
        bool audioPlaying = true;
        auto playHead = getPlayHead();
        if (playHead) {auto playHeadPos = playHead->getPosition();
            if(playHeadPos->getTimeInSeconds() > 0.0) audioPlaying = playHeadPos->getIsPlaying();
            //I have a sound editor that generates a playhead but won't do IsPlaying. This checks if the DAW speaks 'playHead'.
        }
        if (audioPlaying) {
            //begin bar display section
            if ((fabs(inputSampleL-previousLeft)/32000.0f)*getSampleRate() > slewLeft) slewLeft =  (fabs(inputSampleL-previousLeft)/32000.0f)*getSampleRate();
            if ((fabs(inputSampleR-previousRight)/32000.0f)*getSampleRate() > slewRight) slewRight = (fabs(inputSampleR-previousRight)/32000.0f)*getSampleRate();
            previousLeft = inputSampleL; previousRight = inputSampleR; //slew measurement is NOT rectified
            double rectifiedL = fabs(inputSampleL);
            double rectifiedR = fabs(inputSampleR);
            if (rectifiedL > peakLeft) peakLeft = rectifiedL;
            if (rectifiedR > peakRight) peakRight = rectifiedR;
            windowCount++;
            zeroLeft += zeroCrossScale;
            if (longestZeroLeft < zeroLeft) longestZeroLeft = zeroLeft;
            if (wasPositiveL && inputSampleL < 0.0) {
                wasPositiveL = false;
                zeroLeft = 0.0;
            } else if (!wasPositiveL && inputSampleL > 0.0) {
                wasPositiveL = true;
                zeroLeft = 0.0;
            }
            zeroRight += zeroCrossScale;
            if (longestZeroRight < zeroRight) longestZeroRight = zeroRight;
            if (wasPositiveR && inputSampleR < 0.0) {
                wasPositiveR = false;
                zeroRight = 0.0;
            } else if (!wasPositiveR && inputSampleR > 0.0) {
                wasPositiveR = true;
                zeroRight = 0.0;
            } //end bar display section
        }
        
        if (pluginMode <= 0.5) {
            double dryL = inputSampleL;
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
            bip[bip_dvLA] = dryL - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA]-bip[bip_dvLA]; bip[bip_pvLA] = bip[bip_dvLA];//velocity
            bip[bip_dvLC] = bip[bip_pvLB]-bip[bip_dvLB]; bip[bip_pvLB] = bip[bip_dvLB];//acceleration
            bip[bip_dvLD] = bip[bip_pvLC]-bip[bip_dvLC]; bip[bip_pvLC] = bip[bip_dvLC];//jerk
            double bip_dvE = bip[bip_pvLD]-bip[bip_dvLD]; bip[bip_pvLD] = bip[bip_dvLD];//snap
            inputSampleL *= (1.0+(fabs(bip[bip_dvLC])*0.0618)+(fabs(bip[bip_dvLD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            double dryR = inputSampleR;
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
            bip[bip_dvRA] = dryR - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA]-bip[bip_dvRA]; bip[bip_pvRA] = bip[bip_dvRA];//velocity
            bip[bip_dvRC] = bip[bip_pvRB]-bip[bip_dvRB]; bip[bip_pvRB] = bip[bip_dvRB];//acceleration
            bip[bip_dvRD] = bip[bip_pvRC]-bip[bip_dvRC]; bip[bip_pvRC] = bip[bip_dvRC];//jerk
            bip_dvE = bip[bip_pvRD]-bip[bip_dvRD]; bip[bip_pvRD] = bip[bip_dvRD];//snap
            inputSampleR *= (1.0+(fabs(bip[bip_dvRC])*0.0618)+(fabs(bip[bip_dvRD])*-0.05982)+(fabs(bip_dvE)*0.0206));
            
            if (pluginMode > 0.0f) {
                inputSampleL = (inputSampleL*(1.0-(pluginMode*2.0)))+(dryL*(pluginMode*2.0));
                inputSampleR = (inputSampleR*(1.0-(pluginMode*2.0)))+(dryR*(pluginMode*2.0));
            }
        } //engage pluginMode, channel version

        //begin 64 bit stereo floating point dither
        //int expon; frexp((double)inputSampleL, &expon);
        fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
        //inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
        //frexp((double)inputSampleR, &expon);
        fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
        //inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
        //end 64 bit stereo floating point dither
        
        *outL = inputSampleL;
        *outR = inputSampleR;
    }

    if (windowCount > rmsSize)
    {
        AudioToUIMessage msg; //define the thing we're telling JUCE
        msg.what = AudioToUIMessage::SLEW_LEFT; msg.newValue = (float)slewLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::SLEW_RIGHT; msg.newValue = (float)slewRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_LEFT; msg.newValue = (float)sqrt(peakLeft); audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_RIGHT; msg.newValue = (float)sqrt(peakRight); audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_LEFT; msg.newValue = (float)longestZeroLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_RIGHT; msg.newValue = (float)longestZeroRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_INPUT; msg.newValue = (float)cbrt(maxTrim); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_DISPLAY; msg.newValue = (float)params[KNOBDIS]->get(); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_COMP; msg.newValue = (float)cbrt(maxComp); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_TAPE; msg.newValue = (float)cbrt(maxTape); audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_OUTPUT; msg.newValue = (float)fmin(maxFader*maxFader*maxFader,1.0f); audioToUI.push(msg);
        msg.what = AudioToUIMessage::INCREMENT; msg.newValue = 1200.0f; audioToUI.push(msg);
        slewLeft = 0.0;
        slewRight = 0.0;
        peakLeft = 0.0;
        peakRight = 0.0;
        zeroLeft = 0.0;
        zeroRight = 0.0;
        longestZeroLeft = 0.0;
        longestZeroRight = 0.0;
        maxTrim = 0.0f;
        maxComp = 0.0f;
        maxTape = 0.0f;
        maxFader = 0.0f;
        windowCount = 0;
    }
}

