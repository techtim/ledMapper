#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxDatGui.h"

#include "ofxLedMapper.h"

#include "Config.h"

class ofApp : public ofBaseApp{

  public:
    void setup();
    void setupGui();
    
    void update();
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

    ofxDatGui* gui;
    ofxDatGuiDropdown* syphonList;
    bool bShowGui;

    ofxSyphonClient Syphon1, Syphon2;
    ofxSyphonServerDirectory SyphonDir;
    int prev_dirIdx, dirIdx;

    int idSyphonServer, syphonW, syphonH, syphonX, syphonY;
    int filterA, filterR, filterG, filterB;
    ofColor colorFilter;
    bool bRotate;
    float rotate;
    int rotatePos;

    ofxXmlSettings XML;
    
    bool bHelp = false, bSetupGui, bTestImage;
    string textHelp;
};
