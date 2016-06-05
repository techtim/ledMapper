#pragma once

#include "ofMain.h"
#include "ofxLedMapper.h"
#include "ofxSyphon.h"
#include "ofxFastFboReader.h"
#include "Config.h"

class ofApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxLedMapper *ledMapper;

    ofxSyphonClient Syphon1, Syphon2;
    ofxSyphonServerDirectory SyphonDir;
    int prev_dirIdx, dirIdx;

    ofFbo fbo1, fbo2;
    ofxFastFboReader fboReader;
    ofPixels pix;
    ofTexture tex;

    bool bShowGui;
    
    ofxPanel gui;
    ofParameter<ofVec2f> syphonPosition;
    ofParameter<int> idSyphonServer;
    ofParameter<int> syphonW, syphonH, syphonX, syphonY;
    ofParameter<ofColor> colorFilter;
    ofParameter<bool> bRotate;
    ofParameter<float> rotate;
    int rotatePos;
    
    bool bHelp = false;
    string textHelp;
};
