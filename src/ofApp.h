#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxLedMapper.h"

#ifdef TARGET_WIN32
#include "ofxSpout.h"
#elif defined(TARGET_OSX)
#include "ofxSyphon.h"
#endif

#include "Config.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void setupGui();
    
    void update();
    void updateGuiPosition();
    void updateVideoServers();
    void saveToFile(const string & path);
    void loadFromFile(const string & path);
    
    void draw();
    void exit();
    
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);
    
    unique_ptr<ofxLedMapper> ledMapper;
    
    ofFbo fbo1, fbo2;
    
    ofPixels pix;
    ofTexture tex;
    ofShader shader;
    
    unique_ptr<ofxDatGui> gui;
    unique_ptr<ofxDatGuiTheme> guiTheme;
    ofxDatGuiDropdown* syphonList;
    bool bShowGui;
    
#ifdef TARGET_WIN32
    //ofxSpout::Receiver Spout;
#elif defined(TARGET_OSX)
    ofxSyphonClient Syphon1, Syphon2;
    ofxSyphonServerDirectory SyphonDir;
#endif
    int prev_dirIdx, dirIdx;
    
    int idSyphonServer, syphonW, syphonH, syphonX, syphonY;
    int filterA, filterR, filterG, filterB;
    ofColor colorFilter;
    bool bRotate;
    float rotate;
    int rotatePos;
    
    ofxXmlSettings XML;
    
    bool bHelp = false, bSetupGui, bTestImage, bTestImageAnimate;
    float animateHue;
    string textHelp;
};
