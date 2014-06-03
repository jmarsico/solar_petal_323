#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    //ofSetLogLevel(OF_LOG_VERBOSE);
    petalWidth = 300;
    petalHeight = 50;
    
    xSpaceCoeff = 40;
    ySpaceCoeff = 1;
    
    xSpacing = petalWidth+xSpaceCoeff;
    ySpacing = petalHeight+ySpaceCoeff;
    
    maxWait = 2000;
    numPetalsOn = 0;

    
    offset = petalWidth;
    
    gui.setup();
    gui.add(waitTime.set("rate", 0, 0, 2000));
    gui.add(changeColor.setup("show on", false));
    gui.add(voltage.set("voltage per petal", 2.5, 0, 8));
    gui.add(current.set("current per petal", 1.0, 0, 2));
    gui.add(numPetalsOn.set("number of petals on", 0, 0, numPetals));
    gui.add(currentDraw.set("total current draw", 0, 0, 100));
    gui.add(wattage.set("wattage", 0, 0, 400));
    gui.add(lightVal.set("lightVal", 0));
    gui.add(averageLight.set("averageLightVal", 0));
    gui.setPosition(10, ofGetHeight() - 200);
    
    for(int i = 0; i < numPetals; i++)
    {
        Petal p = *new Petal();
        p.init(petalWidth, petalHeight);
        petals.push_back(p);
    }
    
    for(int i = 0; i < petals.size(); i ++)
    {
        petals[i].setWaitTime((int)ofRandom(200));
    }
    
    //create the socket and set to send to 169.254.0.2:11999
	udpConnection.Create();
	udpConnection.Connect("169.254.0.2",11999);
	udpConnection.SetNonBlocking(true);
    
    
    ard.connect("/dev/tty.usbmodem1421", 57600);
	
	// listen for EInitialized notification. this indicates that
	// the arduino is ready to receive commands and it is safe to
	// call setupArduino()
	ofAddListener(ard.EInitialized, this, &testApp::setupArduino);
	bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    
    index = 0;
    totalLight = 0;
    averageLight = 0;
    
    for (int i = 0; i < numReadings; i++){
        readings[i] = 0;
    }
    

}

//--------------------------------------------------------------
void testApp::update(){
    updateArduino();
    
    for(int i=0; i < petals.size(); i++)
    {
        petals[i].update();
        petals[i].setWaitTime(maxWait-waitTime);
        
        //get a count of how many petals are currently "on"
		if(petals[i].state == true && petals[i].prevState == false)
		{
			numPetalsOn++;
		}
		if(petals[i].state == false && petals[i].prevState == true)
		{
			numPetalsOn--;
		}
    }
    
    currentDraw = numPetalsOn * current;
    wattage = numPetalsOn * current * voltage;
    
    
    ///////  smooth the light readings //////////
    totalLight= totalLight - readings[index];
    readings[index] = lightVal;
    totalLight= totalLight + readings[index];
    index = index + 1;
    // if we're at the end of the array...
    if (index >= numReadings)
    {
        index = 0;
    }
    averageLight = totalLight / numReadings;
    
    
    waitTime = ofMap(averageLight, 0, 500, 0, 2000);
    
    //sendToPhysical();

}


//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(80);
    gui.draw();
    ofSetColor(255);
    stringstream message;
    message << "number of petals on: " << numPetalsOn << endl;
    message << "current draw @ 4.5v: " << (int)(numPetalsOn * current) << endl;
    message << "wattage: " << (int)(numPetalsOn * current * voltage) << endl;
    ofDrawBitmapString(message.str(), 10, ofGetHeight() - 300);
    
   
    
    ofPushMatrix();
        ofTranslate(petalWidth, petalHeight);
        int numCols;
        int x = 0;
        for(int i = 1; i < row+2; i++)
        {
            if(i%2 == 0)
            {
                numCols = maxNumCol - 1;
            }
            else numCols = maxNumCol;
            
            for(int j = 0; j < numCols; j++)
            {
                ofPushMatrix();
                    if(i%2 == 0) ofTranslate(petalWidth/2 +20 , 0);
                    ofTranslate(j*xSpacing, i*ySpacing);
                    petals[x].display(changeColor);
                ofPopMatrix();
                x++;
            }
        }
        ofFill();
    
	ofPopMatrix();
    

}

//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::sendToPhysical(){
    
    string message = "";
    int val;
    for(int i = 0; i < numPetals; i++)
    {
        if(petals[i].state == true)
        {
            val = 255;
        } else if(petals[i].state == false)
        {
            val = 0;
        }
        
        message+= ofToString(i) + "|" + ofToString(val) + "[/p]";
        //ofLog() << "index: " << i << " || value: " << val;
    }
    udpConnection.Send(message.c_str(),message.length());
    //ofLog() << "Message Length: " << message.length();
}



//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::setupArduino(const int & version) {
	
	// remove listener because we don't need it anymore
	ofRemoveListener(ard.EInitialized, this, &testApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // Note: pins A0 - A5 can be used as digital input and output.
    // Refer to them as pins 14 - 19 if using StandardFirmata from Arduino 1.0.
    // If using Arduino 0022 or older, then use 16 - 21.
    // Firmata pin numbering changed in version 2.3 (which is included in Arduino 1.0)

    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);

    ofAddListener(ard.EAnalogPinChanged, this, &testApp::analogPinChanged);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::updateArduino(){
    
	// update the arduino, get any data or messages.
    // the call to ard.update() is required
	ard.update();
	
	// do not send anything until the arduino has been set up
	if (bSetupArduino) {
        // fade the led connected to pin D11
		ard.sendPwm(11, (int)(128 + 128 * sin(ofGetElapsedTimef())));   // pwm...
	}
    
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
    ofLogVerbose() << potValue;
    lightVal = ard.getAnalog(pinNum);
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
