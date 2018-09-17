#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxLedMapper.h"
#include "Player.h"

#ifdef TARGET_WIN32
#include "ofxSpout.h"
#elif defined(TARGET_OSX)
#include "ofxSyphon.h"
#endif

class ledMapperApp : public ofBaseApp{
    
public:
    void setup();
    void setupGui();
    
    void update();
    void updateGuiPosition();
    void selectMenuItem(const string &item);
    void updateVideoServers();
    void save(const string & folderPath = "");
    bool load(const string & folderPath = "");
    
    void draw();
    void exit();
    
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onButtonClick(ofxDatGuiButtonEvent e);
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo info);

    string m_configPath, m_configName;

    unique_ptr<LedMapper::ofxLedMapper> m_ledMapper;
    Player m_player;
    ofFbo m_fbo;
    
    ofPixels m_pixels;
    ofTexture tex;

    unique_ptr<ofxDatGuiFolder> m_guiMenu;
    unique_ptr<ofxDatGui> m_guiInput;
    unique_ptr<ofxDatGuiTheme> m_guiTheme;
    ofxDatGuiDropdown* m_syphonList;

    std::function<void(void)> m_drawMenuGuiFunc;
    string m_menuSelected;
    
#ifdef TARGET_WIN32
    //ofxSpout::Receiver Spout;
#elif defined(TARGET_OSX)
    ofxSyphonClient Syphon1, Syphon2;
    ofxSyphonServerDirectory SyphonDir;
#endif
    int prev_dirIdx, dirIdx;
    
    int idSyphonServer, syphonW, syphonH, syphonX, syphonY;
    int filterA, filterR, filterG, filterB;
    bool bRotate;
    float rotate;
    int rotatePos;
    
    ofxXmlSettings XML;
    
    bool bHelp = false, bSetupGui, bTestImage, bTestImageAnimate, bMenuExpanded;
    float animateHue;
    string textHelp;
};
