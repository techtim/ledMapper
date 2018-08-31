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

struct Content{
	string id;
	string path;
    string type;
	ofVideoPlayer player;
	void draw(float x, float y, float w, float h) { player.draw(x, y, w, h); }
};

static void to_json(ofJson &j, const Content &c) { j = ofJson{ { "id", c.id }, { "path", c.path }, {"type", c.type} }; }

static void from_json(const ofJson &j, Content &c)
{
    c.id = j.at("id").get<string>();
    c.path = j.at("path").get<string>();
    c.type = j.at("type").get<string>();
}

class Player {
    std::vector<ofVideoPlayer> m_videoPlayers;
    size_t m_curContent, m_prevContent;
    long m_fadeMs, m_fadeStart;
    long m_curVideoStart; /// hack aroung VideoPlayer.getPosition that don't work
    bool m_playing;
    string m_path;
#ifndef LED_MAPPER_NO_GUI
    unique_ptr<ofxDatGui> m_gui;
    ofxDatGuiScrollView* m_listVideos;
    unique_ptr<LedMapper::ofxDatGuiThemeLM> m_guiTheme;
#endif
    
public:
    Player();
    ~Player();

    void load(const string &path);
    void save(const string &path);

	void setupGui();
	void draw(float x,float y,float w,float h);
    void drawGui();

    void addContent(const string &path);
    void deleteContent(const size_t id);
    void setCurrentContent(size_t index);

#ifndef LED_MAPPER_NO_GUI
    void onButtonClick(ofxDatGuiButtonEvent e);
    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
#endif
};
