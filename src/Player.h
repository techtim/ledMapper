//
//  Player.hpp
//  ledMapper
//
//  Created by Tim Tvl on 7/15/18.
//
//
#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Common.h"

class Player {
    std::vector<ofVideoPlayer> m_videoPlayers;
    size_t m_curContent, m_prevContent;
    long m_fadeMs, m_fadeStart;
    bool m_playing;
    
#ifndef LED_MAPPER_NO_GUI
    unique_ptr<ofxDatGui> m_gui;
    unique_ptr<ofxDatGuiScrollView> m_listVideos;
#endif
    
public:
    Player();
    ~Player();
    
	void setupGui();
	void draw(const ofVec2f &pos, const ofVec2f &size);
    void drawGui();

    void addContent(const string &path);
    void deleteContent(const size_t id);
    void setCurrentContent(size_t index);
        
#ifndef LED_MAPPER_NO_GUI
    void onButtonClick(ofxDatGuiButtonEvent e);
    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
#endif
};
