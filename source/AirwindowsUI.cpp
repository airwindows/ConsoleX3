// AirwindowsUI by Chris Johnson
// Initial seed code for the meter created by Paul Walker on 8/23/21.
// From then on all this wild stuff is Chris :)
//and huge support from Sudara and Pamplejuce!
#include "AirwindowsUI.h"
#ifndef M_PI
#  define M_PI (3.14159265358979323846)
#endif
#ifndef M_PI_2
#  define M_PI_2 (1.57079632679489661923)
#endif

void AirwindowsLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) {
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    float bevelW = sqrt((float)width);
    if (slider.isHorizontal()) bevelW = sqrt((float)height);
    float lineW = sqrt(bevelW)*0.52f;
    float trackWidth = bevelW;
    //basic variables we'll be using for our controls
    
    juce::Path backgroundTrack;
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::white, 0.75f)); //highlight
    backgroundTrack.startNewSubPath((slider.isHorizontal()?(float)x:(float)x+(float)width*0.5f)+(lineW*0.5f), (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)((height*0.97f)+y))+(lineW*0.5f));
    backgroundTrack.lineTo ((slider.isHorizontal()?(float)(width+x):(float)x+(float)width*0.5f)+(lineW*0.5f), (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)y)+(lineW*0.5f));
    g.strokePath (backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
    backgroundTrack.clear();
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.75f)); //shadow
    backgroundTrack.startNewSubPath((slider.isHorizontal()?(float)x:(float)x+(float)width*0.5f)-lineW, (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)((height*0.97f)+y))-lineW);
    backgroundTrack.lineTo ((slider.isHorizontal()?(float)(width+x):(float)x+(float)width*0.5f)-lineW, (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)y)-lineW);
    g.strokePath (backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
    backgroundTrack.clear();
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId)); //inside slot in which the slider goes
    backgroundTrack.startNewSubPath((slider.isHorizontal()?(float)x:(float)x+(float)width*0.5f), (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)((height*0.97f)+y)));
    backgroundTrack.lineTo ((slider.isHorizontal()?(float)(width+x):(float)x+(float)width*0.5f), (slider.isHorizontal()?(float)y+(float)height*0.5f:(float)y));
    g.strokePath (backgroundTrack, {trackWidth*0.618f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
    //draw the slot from which the slider moves. Note that we leave a bit of space on the bottom to show the label:
    
    g.setFont(juce::FontOptions(newFont, g.getCurrentFont().getHeight(), 0));
    g.setFont ((((lineW+bevelW)*32.0f) / (float)g.getCurrentFont().getHeight()));
    if (slider.isHorizontal()) bounds.removeFromBottom((bounds.getHeight()*0.5f)-(bevelW*3.2f));
    else bounds.removeFromBottom(-30.0f);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::white, 0.75f)); //highlight
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)(bounds.getWidth()+23+lineW),(int)(bounds.getHeight()+lineW)), juce::Justification::centredBottom, 1);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.75f)); //shadow
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)(bounds.getWidth()+23-lineW),(int)(bounds.getHeight()-lineW)), juce::Justification::centredBottom, 1);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.25f)); //text inside emboss
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)bounds.getWidth()+23,(int)bounds.getHeight()), juce::Justification::centredBottom, 1);
    //This is the drawing of the text under the slider, to allow the slider to obscure it. Sliders are designed to be packed pretty tightly,
    //but the horizontal ones can still have a lot of text. To control their bulk, narrow the slot they're in.
    
    juce::Point<float> maxPoint = {slider.isHorizontal()?(sliderPos*0.91f)+(width*0.06f):((float)x+(float)width*0.5f), slider.isHorizontal()?((float)y+(float)height*0.5f):(sliderPos*0.91f)+(height*0.06f)};
    auto thumbWidth = (pow(bevelW,1.72f)*0.83f);
    auto rectSlider = juce::Rectangle<float>(thumbWidth*1.12f, thumbWidth*1.02f).withCentre(maxPoint);
    if (slider.isHorizontal()) rectSlider = juce::Rectangle<float>(thumbWidth*1.04f, thumbWidth*1.14f).withCentre(maxPoint);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId)); g.setOpacity(1.0f); g.fillRoundedRectangle (rectSlider, bevelW);
    //solid background for knob so you can't see the track under it
    juce::ColourGradient cg = juce::ColourGradient(juce::Colours::white, rectSlider.getTopLeft(), juce::Colours::black, rectSlider.getBottomRight(),false);
    cg.addColour(0.2f, juce::Colours::white); cg.addColour(0.618f, juce::Colours::transparentBlack); cg.addColour(0.9f, juce::Colours::black); cg.isRadial = true;
    g.setGradientFill(cg);
    auto inset = rectSlider; inset.reduce(bevelW*0.25f, bevelW*0.25f);
    g.drawRoundedRectangle (inset, bevelW*0.8f, bevelW*0.5f);
    cg = juce::ColourGradient(juce::Colours::transparentWhite, rectSlider.getTopLeft(), juce::Colours::black, rectSlider.getBottomRight(),false);
    cg.addColour(0.0955f, juce::Colours::white); cg.addColour(0.382f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.addColour(0.618f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.isRadial = true;
    g.setGradientFill(cg); inset.reduce(bevelW*0.25f, bevelW*0.25f); g.drawRoundedRectangle (inset, bevelW*0.9f, bevelW*0.382f);
    cg = juce::ColourGradient(juce::Colours::transparentWhite, rectSlider.getTopLeft(), juce::Colours::transparentBlack, rectSlider.getBottomRight(),false);
    cg.addColour(0.04775f, juce::Colours::transparentWhite); cg.addColour(0.382f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.addColour(0.618f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.isRadial = true;
    g.setGradientFill(cg); inset.reduce(bevelW*0.382f, bevelW*0.382f); g.drawRoundedRectangle (inset, 1.0f, 0.618f);
    g.setColour (juce::Colours::black); g.drawRoundedRectangle (rectSlider, bevelW, lineW);
    //This is the outside area of the slider knob, with the shading/highlighting that renders the 3D effect.
    
    float thumbScale = 0.85f; rectSlider = juce::Rectangle<float> (thumbWidth*thumbScale, thumbWidth*thumbScale).withCentre (maxPoint);
    rectSlider = juce::Rectangle<float>(thumbWidth*thumbScale, thumbWidth*thumbScale).withCentre(maxPoint);
    g.setColour (slider.findColour (juce::Slider::thumbColourId)); g.fillEllipse (rectSlider);
    cg = juce::ColourGradient(juce::Colours::white, rectSlider.getBottomRight(), juce::Colours::black, rectSlider.getTopLeft(),false);
    cg.addColour(0.191f, juce::Colours::white); cg.addColour(0.382f, slider.findColour (juce::Slider::thumbColourId)); cg.addColour(0.618f, slider.findColour (juce::Slider::thumbColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset = rectSlider; inset.reduce(bevelW*0.382f, bevelW*0.382f);
    g.drawEllipse (inset, bevelW*0.5f);
    cg = juce::ColourGradient(juce::Colours::white, rectSlider.getBottomRight(), juce::Colours::black, rectSlider.getTopLeft(),false);
    cg.addColour(0.0955f, juce::Colours::transparentWhite); cg.addColour(0.382f, slider.findColour (juce::Slider::thumbColourId)); cg.addColour(0.618f, slider.findColour (juce::Slider::thumbColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset.reduce(bevelW*0.125f, bevelW*0.125f);
    g.drawEllipse (inset, bevelW*0.5f); g.setColour (juce::Colours::black); g.drawEllipse (rectSlider, lineW);
    //This is the thumb of the knob, allowing a custom color to the thumb
}


void AirwindowsLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) {
    float tilt = (slider.findColour (juce::Slider::backgroundColourId).getFloatRed()-0.5f)*1.236f;
    auto textbounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    auto bounds = textbounds; bounds.removeFromBottom(4.0f+(height*0.12f)+(height*tilt*tilt*0.12f)+(height*tilt*fabs(tilt)*0.12f)); bounds.reduce(1.0f, 1.0f);
    float scaleHeight = 1.0f-(fabs(tilt)); //proportion of vertical height relative to horizontal
    float trimscaleHeight = scaleHeight + (tilt*0.05f);
    float radius = bounds.getWidth()*0.5f; if (radius > (bounds.getHeight()/scaleHeight)*0.5f) radius = (bounds.getHeight()/scaleHeight)*0.5f;
    auto gradientSquare = juce::Rectangle<float>((float)bounds.getCentreX()-radius, (float)bounds.getCentreY()-(radius*(float)sqrt(scaleHeight)), radius*2.0f, radius*(float)sqrt(trimscaleHeight)*2.0f).toFloat();
    auto square = juce::Rectangle<float>((float)bounds.getCentreX()-radius, (float)bounds.getCentreY()-(radius*scaleHeight), radius*2.0f, radius*trimscaleHeight*2.0f).toFloat();
    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float bevelW = (float)sqrt(radius*0.5f)*1.618f;
    float lineW = (float)sqrt(bevelW)*0.52f;
    //basic variables we'll be using for our controls
    
    juce::ColourGradient cg = juce::ColourGradient(juce::Colours::white, gradientSquare.getTopLeft(), juce::Colours::black, gradientSquare.getBottomRight(),false);
    cg.addColour(0.2f, juce::Colours::white); cg.addColour(0.618f, juce::Colours::transparentBlack); cg.addColour(0.9f, juce::Colours::black); cg.isRadial = true;
    g.setGradientFill(cg);
    auto inset = square; inset.reduce(bevelW*0.25f, bevelW*0.25f); inset.removeFromTop(-bevelW*tilt*fabs(tilt+0.15f)*0.618f);
    g.drawEllipse (inset, bevelW*0.5f);
    cg = juce::ColourGradient(juce::Colours::transparentWhite, gradientSquare.getTopLeft(), juce::Colours::black, gradientSquare.getBottomRight(),false);
    cg.addColour(0.0955f, juce::Colours::white); cg.addColour(0.382f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.addColour(0.618f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset.reduce(bevelW*0.25f, bevelW*0.25f); inset.removeFromTop(-bevelW*tilt*fabs(tilt+0.15f)*0.618f);
    g.drawEllipse (inset, bevelW*0.382f);
    cg = juce::ColourGradient(juce::Colours::transparentWhite, gradientSquare.getTopLeft(), juce::Colours::transparentBlack, gradientSquare.getBottomRight(),false);
    cg.addColour(0.04775f, juce::Colours::transparentWhite); cg.addColour(0.382f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.addColour(0.618f, slider.findColour (juce::ResizableWindow::backgroundColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset.reduce(bevelW*0.382f, bevelW*0.382f); inset.removeFromTop(-bevelW*tilt*fabs(tilt+0.15f)*0.618f);
    g.drawEllipse (inset, bevelW*0.618f); g.setColour (juce::Colours::black); g.drawEllipse (square, lineW);
    //This is the outside circle of the knob, with the shading/highlighting that renders the 3D effect. Tilting of the knob is included
    
    g.setFont(juce::FontOptions(newFont, g.getCurrentFont().getHeight(), 0));
    g.setFont ((((lineW+bevelW)*32.0f) / (float)g.getCurrentFont().getHeight()) + (tilt*0.25f));
    auto padHeight = (sqrt(bevelW)*-2.0f) + ((bounds.getHeight())-(bounds.getWidth()*scaleHeight)) + (tilt*bevelW) + (fabs(tilt)*-bevelW); if (padHeight < 0.0) padHeight = 0.0;
    textbounds.removeFromBottom(padHeight*0.618f);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::white, 0.75f)); //highlight
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)(textbounds.getWidth()+lineW),(int)(textbounds.getHeight()+lineW)), juce::Justification::centredBottom, 1);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.75f)); //shadow
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)(textbounds.getWidth()-lineW),(int)(textbounds.getHeight()-lineW)), juce::Justification::centredBottom, 1);
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.25f)); //text inside emboss
    g.drawFittedText(slider.getName(), juce::Rectangle<int>((int)textbounds.getWidth(),(int)textbounds.getHeight()), juce::Justification::centredBottom, 1);
    //This is the drawing of the text just under the knob, no matter what tilt setting is included and what the real bounding box is doing. That's why it's so tricky :)
    
    auto arcRadius = (radius-(bevelW*0.5f))*0.7f;
    auto thumbWidth = pow(bevelW,1.618f)*0.7f;
    juce::Point<float> thumbPoint (bounds.getCentreX()+(arcRadius*std::cos(toAngle-juce::MathConstants<float>::halfPi)),bounds.getCentreY()+(-tilt*bevelW)+(arcRadius*trimscaleHeight*std::sin(toAngle-juce::MathConstants<float>::halfPi)));
    float thumbScale = 1.0f+(std::sin(toAngle-juce::MathConstants<float>::halfPi)*tilt*fabs(tilt)*0.5f);
    square = juce::Rectangle<float> (thumbWidth*thumbScale, thumbWidth*trimscaleHeight*thumbScale).withCentre (thumbPoint);
    gradientSquare = juce::Rectangle<float> (thumbWidth*trimscaleHeight*thumbScale, thumbWidth*thumbScale).withCentre (thumbPoint);
    g.setColour (slider.findColour (juce::Slider::thumbColourId)); g.fillEllipse (square);
    cg = juce::ColourGradient(juce::Colours::white, gradientSquare.getBottomRight(), juce::Colours::black, gradientSquare.getTopLeft(),false);
    cg.addColour(0.191f, juce::Colours::white); cg.addColour(0.382f, slider.findColour (juce::Slider::thumbColourId)); cg.addColour(0.618f, slider.findColour (juce::Slider::thumbColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset = square; inset.reduce(bevelW*0.382f, bevelW*0.382f); inset.removeFromBottom(bevelW*tilt*fabs(tilt+0.15f)*0.125f);
    g.drawEllipse (inset, bevelW*0.5f);
    cg = juce::ColourGradient(juce::Colours::white, gradientSquare.getBottomRight(), juce::Colours::black, gradientSquare.getTopLeft(),false);
    cg.addColour(0.0955f, juce::Colours::transparentWhite); cg.addColour(0.382f, slider.findColour (juce::Slider::thumbColourId)); cg.addColour(0.618f, slider.findColour (juce::Slider::thumbColourId)); cg.isRadial = true;
    g.setGradientFill(cg);
    inset.reduce(bevelW*0.125f, bevelW*0.125f); inset.removeFromBottom(bevelW*tilt*fabs(tilt+0.15f)*0.125f);
    g.drawEllipse (inset, bevelW*0.5f); g.setColour (juce::Colours::black); g.drawEllipse (square, lineW);
    //This is the thumb of the knob, also rendered on a tilt if needed, and allowing a custom color to the thumb
}

void AirwindowsMeter::mouseDown(const juce::MouseEvent &event)
{
    if (event.getNumberOfClicks() > 1) resetArrays();
}

void AirwindowsMeter::paint(juce::Graphics &g)
{
    float vS = displayHeight/200.0f; // short for vScale: everything * this
    juce::ColourGradient backdropGradient = juce::ColourGradient::vertical(juce::Colours::white, 0.0f, backdropColour, 200.0f*vS);
    backdropGradient.addColour(gradientMin, backdropColour.interpolatedWith(juce::Colours::white, gradientMin));
    //the mid point of the gradient goes toward the bottom and goes whiter when everything's in balance
    g.setFillType(backdropGradient);
    g.fillAll(); //blank screen before doing anything, unless our draw covers the whole display anyway
    if (drawInfo) {
        g.setColour(juce::Colours::lightgrey);
        g.fillRect(0,  (int)(60.0f*vS), getWidth(),1); // -6dB markings
        g.fillRect(0, (int)(101.02*vS), getWidth(),1); //-12dB markings
        g.fillRect(0, (int)(130.02f*vS), getWidth(),1); //-18dB markings
        g.fillRect(0, (int)(150.2f*vS), getWidth(),1); //-24dB markings
        g.fillRect(0, (int)(164.9f*vS), getWidth(),1); //-30dB markings
        g.fillRect(0, (int)(175.2f*vS), getWidth(),1); //-36dB markings
        g.fillRect(0, (int)(182.5f*vS), getWidth(),1); //-42dB markings
    }
    
    for (unsigned long count = 0; count < fmin(displayWidth,5150); ++count) //count through all the points in the array
    {
        g.setColour(juce::Colours::black);
        float psDotSizeL = 0.0f;
        float psDotSizeR = 0.0f;
        float slewDotSizeL = 0.0f;
        float slewDotSizeR = 0.0f;
        float bassDotSizeL = 0.0f;
        float bassDotSizeR = 0.0f;
       float peakL = dataPL[count]*200.0f;
        float peakR = dataPR[count]*200.0f;
        float slewL = sqrt(dataSL[count])*300.0f;
        float slewR = sqrt(dataSR[count])*300.0f;
        float meterZeroL = (sqrt(dataZL[count])*6.0f)-6.0f;
        if (meterZeroL > 192.0f) meterZeroL = 192.0f;
        float bassL = fmin((sqrt(meterZeroL)*16.2f)-20.0f,199.0f);
        float meterZeroR = (sqrt(dataZR[count])*6.0f)-6.0f;
        if (meterZeroR > 192.0f) meterZeroR = 192.0f;
        float bassR = fmin((sqrt(meterZeroR)*16.2f)-20.0f,199.0f);
        //begin draw dots on meters L
        if (peakL > 196.0f) {
            g.setColour(juce::Colour(255, 0, 0));
            g.fillRect((float)count, sustainedClip*vS, 1.9f, (201.9f-sustainedClip)*vS);
            sustainedClip *= 0.99f; //peak is clipping!
            if (peakR > 197.0f) sustainedClip *= 0.975f; //if both are clipping, escalate
        } else sustainedClip = 180.0f;
        if (peakL > 1.0f) { //peak isn't clipping, but is not literally zero so there's something here to work with
            psDotSizeL = (dataPL[count]*64.0f) / (fabs((peakL*0.945433426957143f)-slewL)+6.18033988749894f);
            slewDotSizeL = (sin(0.1618f/psDotSizeL)*6.18f)+(sqrt(slewL)*0.1618f);
            bassDotSizeL = sqrt(meterZeroL*0.1f*dataPL[count]);
            if (psDotSizeL > 1.0f) g.setColour(juce::Colour::fromFloatRGBA(fmax((slewL-(peakL*0.945433426957143f)),0.0f)*0.029f,
                                                                           fmax(((peakL*0.945433426957143f)-slewL),0.0f)*0.012f, 1.0f, 1.0f));
            else if (slewL > peakL) g.setColour(juce::Colour::fromFloatRGBA(fmin((64.0f+slewL)/128.0f,1.0f), 0.0f, 0.0f, 1.0f));
            else                    g.setColour(juce::Colour::fromFloatRGBA(0.0f, fmax((160.0f-peakL)/160.0f,0.0f), 0.0f, 1.0f)); //set COLOR
            g.setOpacity(sqrt(1.0f-fmin(1.0f,blinkenDisplay*2.0f)));
            g.fillRect((float)count, (float)((200.0f - peakL)*vS), psDotSizeL+0.618f, (psDotSizeL+0.618f)*vS);
            g.setOpacity(sqrt(1.0f-fabs(blinkenDisplay-0.5f)*2.0f));
            if (slewL > 194.0f) g.fillRect((float)count, (float)((200.0f-(sqrt(slewL-194.0f)*1.618f))*vS), 1.618f, (float)(fmin((slewL-194.0f),196.0f)*vS));
            else g.fillRect((float)count, (float)((200.0f-slewL)*vS), slewDotSizeL+0.618f, slewDotSizeL*vS);
            g.setOpacity(sqrt(fmax(0.0f,(blinkenDisplay-0.5f)*2.0f)));
            g.fillRect((float)count, bassL*vS, bassDotSizeL+fmax(pow(peakL/140.0f,4.0f),0.0f), (bassDotSizeL+fmax(pow(peakL/140.0f,4.0f),0.0f))*vS); //zero cross subs
        } //end draw dots on meters L
        
        //begin draw dots on meters R
        if (peakR > 197.0f) {
            g.setColour(juce::Colour(255, 0, 0));
            g.fillRect((float)count, sustainedClip*vS, 1.9f, (201.9f-sustainedClip)*vS);
            sustainedClip *= 0.99f; //peak is clipping!
            if (peakL > 197.0f) sustainedClip *= 0.975f; //if both are clipping, escalate
        } else sustainedClip = 180.0f;
        if (peakR > 1.0f) { //peak isn't clipping, but is not literally zero so there's something here to work with
            psDotSizeR = (dataPR[count]*64.0f) / (fabs((peakR*0.945433426957143f)-slewR)+6.18033988749894f);
            slewDotSizeR = (sin(0.1618f/psDotSizeR)*6.18f)+(sqrt(slewR)*0.1618f);
            bassDotSizeR = sqrt(meterZeroR*0.1f*dataPR[count]);
            if (psDotSizeR > 1.0f) g.setColour(juce::Colour::fromFloatRGBA(fmax((slewR-(peakR*0.945433426957143f)),0.0f)*0.029f,
                                                                           fmax(((peakR*0.945433426957143f)-slewR),0.0f)*0.012f, 1.0f, 1.0f));
            else if (slewR > peakR) g.setColour(juce::Colour::fromFloatRGBA(fmin((64.0f+slewR)/128.0f,1.0f), 0.0f, 0.0f, 1.0f));
            else                    g.setColour(juce::Colour::fromFloatRGBA(0.0f, fmax((160.0f-peakR)/160.0f,0.0f), 0.0f, 1.0f)); //set COLOR
            g.setOpacity(sqrt(1.0f-fmin(1.0f,blinkenDisplay*2.0f)));
            g.fillRect((float)count, (float)((200.0f - peakR)*vS), psDotSizeR+0.618f, (psDotSizeR+0.618f)*vS);
            g.setOpacity(sqrt(1.0f-fabs(blinkenDisplay-0.5f)*2.0f));
            if (slewR > 194.0f) g.fillRect((float)count, (float)((200.0f-(sqrt(slewR-194.0f)*1.618f))*vS), 1.618f, (float)(fmin((slewR-194.0f),196.0f)*vS));
            else g.fillRect((float)count, (float)((200.0f-slewR)*vS), slewDotSizeR+0.618f, slewDotSizeR*vS);
            g.setOpacity(sqrt(fmax(0.0f,(blinkenDisplay-0.5f)*2.0f)));
            g.fillRect((float)count, bassR*vS, bassDotSizeR+fmax(pow(peakR/140.0f,4.0f),0.0f), (bassDotSizeR+fmax(pow(peakL/140.0f,4.0f),0.0f))*vS); //zero cross subs
            //zero cross subs
        } //end draw dots on meters R
        
        unsigned long bintracker;
        bintracker = (unsigned long)((peakL-((180.0f-sustainedClip)*1.618033988749894f)) * (0.005f*(float)totalBins));
        //converts 0-200 to 0-bin number for textscore bins, and the peak measurement will also try to incorporate varying clip lengths
        //on the grounds that FOR loudenated stuff, variety in the length of total clip registers for our purposes.
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins))); //mirror around max value so it reflects
        if (bintracker > 0 && bintracker <= totalBins) peakTrack[bintracker] += psDotSizeL;
        bintracker = (unsigned long)((peakR-((180.0f-sustainedClip)*1.618033988749894f)) * (0.005f*(float)totalBins));
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins)));
        if (bintracker > 0 && bintracker <= totalBins) peakTrack[bintracker] += psDotSizeR;
        //peak textscore bins
       bintracker = (unsigned long)(slewL * (0.005f*(float)totalBins));//converts 0-200 to 0-bin number for textscore bins
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins))); //mirror around max value so it reflects
        if (bintracker > 0 && bintracker <= totalBins) slewTrack[bintracker] += slewDotSizeL;
        bintracker = (unsigned long)(slewR * (0.005f*(float)totalBins));
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins)));
        if (bintracker > 0 && bintracker <= totalBins) slewTrack[bintracker] += slewDotSizeR;
        //slew textscore bins
        bintracker = (unsigned long)(bassL * (0.005f*(float)totalBins));//converts 0-200 to 0-bin number for textscore bins
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins))); //mirror around max value so it reflects
        if (bintracker > 0 && bintracker <= totalBins) bassTrack[bintracker] += bassDotSizeL;
        bintracker = (unsigned long)(bassR * (0.005f*(float)totalBins));
        bintracker = (unsigned long)(totalBins - (fabs((int)bintracker-(int)totalBins)));
        if (bintracker > 0 && bintracker <= totalBins) bassTrack[bintracker] += bassDotSizeR;
        //bass textscore bins
        
        //bins are for reinforcing score of a dispersed cloud of dot positions rather than maxing out a position
        //this applies not only to Meter but also to the backdrop color in ConsoleX3, to make them consistent
        float peakScore = 0.0;
        float slewScore = 0.0;
        float bassScore = 0.0;
        float sinScale = 0.0;
        for (unsigned long binscale = 0; binscale < totalBins; ++binscale) {
            peakTrack[binscale] = fmax(fmin(peakTrack[binscale]-0.1618033988749894f, 1.618033988749894f),0.0f); //decrement speed
            sinScale = fmax(sin(((float)binscale/(float)totalBins)*(float)M_PI_2),0.0f);
            //sin(1.57) weighting means big red dots in good places, ignore green slews at bottom
            peakScore += (float)sin(peakTrack[binscale]) * sinScale;
            
            slewTrack[binscale] = fmax(fmin(slewTrack[binscale]-0.1618033988749894f, 1.618033988749894f),0.0f); //decrement speed
            slewScore += (float)sin(slewTrack[binscale]) * sinScale;
            
            bassTrack[binscale] = fmax(fmin(bassTrack[binscale]-0.1618033988749894f, 1.618033988749894f),0.0f); //decrement speed
            sinScale = fmax(sin(((float)binscale/(float)totalBins)*(float)M_PI),0.0f);
            //sin(3.1415) does a sine weighting on zero cross meter, but resists overweighting midrange sections
            bassScore += (float)sin(bassTrack[binscale]) * sinScale;
        }
        if (count < dataPosition && count > dataPosition-2) {
            outputB += peakScore;
            outputR += slewScore;
            outputG += bassScore;
            outputVol = sqrt(fmax(fmax(dataPL[count],dataPR[count]),fmax(dataSL[count],dataSR[count])));
        }
    }
    float scaleFont = (float)sqrt(fmin(displayHeight*1.618f,displayWidth*0.618f));
    g.setColour(juce::Colours::black);
    g.setFont(scaleFont);
    if (drawInfo) {
        if (blinkenDisplay <= 0.5) {
            g.setOpacity(sqrt(1.0f-fmin(1.0f,blinkenDisplay*2.0f)));
            g.drawText("peaks", 8, (int)(20.0f*vS)-(int)(scaleFont/2), displayWidth-11, (int)scaleFont, juce::Justification::bottomLeft);
            g.drawText(" -6 dB", 8, (int)(60.0f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-12 dB", 8, (int)(101.02f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-18 dB", 8, (int)(130.02f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-24 dB", 8, (int)(150.2f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-30 dB", 8, (int)(164.9f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-36 dB", 8, (int)(175.2f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft);
            g.drawText("-42 dB", 8, (int)(182.5f*vS)-(int)(scaleFont/2), displayWidth-6, (int)(scaleFont), juce::Justification::bottomLeft); // was 182.5
            //dB markings
        }
        g.setOpacity(sqrt(1.0f-fabs(blinkenDisplay-0.5f)*2.0f));
        g.drawText("slew", 8, (int)(20.0f*vS)-(int)(scaleFont/2), displayWidth-11, (int)scaleFont, juce::Justification::centredBottom);
        //slew markings (not dB scaled, but lines are still there)
        if (blinkenDisplay > 0.5) {
            g.setOpacity(sqrt(fmax(0.0f,(blinkenDisplay-0.5f)*2.0f)));
            g.drawText("zero cross", 0, (int)(20.0f*vS)-(int)(scaleFont/2), displayWidth-11, (int)scaleFont, juce::Justification::bottomRight);
            g.drawText("900 Hz", 0, (int)(60.0f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("210 Hz", 0, (int)(101.02f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("100 Hz", 0, (int)(130.02f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("60 Hz", 0, (int)(150.2f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("45 Hz", 0, (int)(164.9f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("35 Hz", 0, (int)(175.2f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            g.drawText("30 Hz", 0, (int)(182.5f*vS)-(int)(scaleFont/2), displayWidth-11, (int)(scaleFont), juce::Justification::bottomRight);
            //zero cross markings
        } //selectively draw backgrounds
    } else {
        if (displayTrackName.compare("ConsoleX3") != 0) { //if zero, the track is NOT named ConsoleX3
            if (displayWidth>displayHeight) { //wider
                g.setFont(scaleFont*fmin(1.0f+(displayWidth/displayHeight),(float)sqrt(displayHeight)*0.618f));
                g.setOpacity(0.21f);
                g.setColour(juce::Colours::white);
                g.drawText(displayTrackName, 0, (int)(175.2f*vS)-(int)(scaleFont-1), displayWidth, (int)(scaleFont), juce::Justification::centredBottom);
                g.drawText(displayTrackName, 1, (int)(175.2f*vS)-(int)(scaleFont), displayWidth+1, (int)(scaleFont), juce::Justification::centredBottom);
                g.setOpacity(0.51f);
                g.setColour(juce::Colours::white);
                g.drawText(displayTrackName, 1, (int)(175.2f*vS)-(int)(scaleFont-1), displayWidth, (int)(scaleFont), juce::Justification::centredBottom);
                g.setOpacity(1.0f);
                g.setColour(juce::Colours::black);
                g.drawText(displayTrackName, 0, (int)(175.2f*vS)-(int)(scaleFont), displayWidth+1, (int)(scaleFont), juce::Justification::centredBottom);
            } else { //taller
                g.setFont(scaleFont*(float)sqrt(displayWidth)*1.618f); //as large as possible for vertical arrangement
                g.setOpacity(0.21f);
                g.setColour(juce::Colours::white);
                g.drawText(displayTrackLetter, 0, (int)(199.0f*vS)-(int)(scaleFont-1.0f), displayWidth, (int)(scaleFont), juce::Justification::centredBottom); //+0,+1
                g.drawText(displayTrackLetter, 1, (int)(199.0f*vS)-(int)(scaleFont), displayWidth+1, (int)(scaleFont), juce::Justification::centredBottom); //+1,+0
                g.setOpacity(0.51f);
                g.setColour(juce::Colours::white);
                g.drawText(displayTrackLetter, 1, (int)(199.0f*vS)-(int)(scaleFont-1.0f), displayWidth+1, (int)(scaleFont), juce::Justification::centredBottom); //+1,+1
                g.setOpacity(1.0f);
                g.setColour(juce::Colours::black);
                g.drawText(displayTrackLetter, 0, (int)(199.0f*vS)-(int)(scaleFont), displayWidth, (int)(scaleFont), juce::Justification::centredBottom);
            }
        }
    }
    g.setColour(juce::Colours::grey);
    g.fillRect((int)dataPosition, 0, 1, (int)(199.0*vS)); //the moving line
}
