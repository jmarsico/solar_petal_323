#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Petal.h"
#include "ofxNetwork.h"
#include "ofEvents.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void sendToPhysical();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int xSpacing;		//horizontal spacing
    int ySpacing;		//vertical spacing
    int xSpaceCoeff;
    int ySpaceCoeff;
    int maxWait; 		//set a max for time/rate variable
    
    static const int maxNumCol = 2;		//number of columns
    static const int row = 5;		//number of rows
    static const int numPetals = 8 ;		//total number of petals
    
    int petalWidth;
    int petalHeight;
    int offset = 60;
    
    const static int numReadings = 20;
    int readings[numReadings];
    int index, totalLight;
    
    
    vector<Petal> petals;

    
    ofxPanel gui;
    ofxToggle changeColor;
    ofParameter<int> waitTime;
    ofParameter<float> voltage;
    ofParameter<float> current;
    ofParameter <float> currentDraw;
    ofParameter <float> numPetalsOn;
    ofParameter<int> wattage;
    ofParameter<int> lightVal;
    ofParameter <int> averageLight;
    
    ofxUDPManager udpConnection;
    
    ofArduino	ard;
	bool		bSetupArduino;			// flag variable for setting up arduino once

private:
    
    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
	void updateArduino();
    
    string buttonState;
    string potValue;
    
    
    
		
};
