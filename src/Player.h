//
//  Player.hpp
//  ledMapper
//
//  Created by Tim Tvl on 7/15/18.
//
//
#pragma once

#include "Common.h"
#include "ofMain.h"
#include "ofxDatGui.h"

struct Content {
    string id;
    string path;
    string type;
    uint64_t durationMs, startMs, endMs, fadeMs;
    ofVideoPlayer video;
    void draw(float x, float y, float w, float h)
    {
        video.update();
        video.draw(x, y, w, h);
    }
};

static void to_json(ofJson &j, const Content &c)
{
    j = ofJson{ { "id", c.id }, { "path", c.path }, { "type", c.type } };
}

static void from_json(const ofJson &j, Content &c)
{
    c.id = j.at("id").get<string>();
    c.path = j.at("path").get<string>();
    c.type = j.at("type").get<string>();
}

class Player {
    bool m_playing;
    string m_curContent, m_prevContent;
    std::map<string, Content> m_contentPlayers;
    std::vector<string> m_contentCue;
    uint64_t m_fadeMs, m_fadeStart;
    long m_curVideoStart; /// hack aroung VideoPlayer.getPosition that don't work
    string m_configPath;
    ofColor m_colorize;
#ifndef LED_MAPPER_NO_GUI
    unique_ptr<ofxDatGui> m_gui;
    ofxDatGuiScrollView *m_listVideos;
    unique_ptr<LedMapper::ofxDatGuiThemeLM> m_guiTheme;
#endif

public:
    Player();
    ~Player();

    void load(const string &path);
    void save(const string &path);
    void reset();
    void setColorize(int r, int g, int b, int a);
    void setupGui();

    void setGuiPosition(int x, int y);
    void draw(float x, float y, float w, float h);
    void drawGui();
    ofxDatGui *getGui();
    void addContent(const string &path);
    void deleteContent(const string &id);
    void setCurrentContent(const string &id);
    const string getNextContent(const string &id);

    void updateListView();

#ifndef LED_MAPPER_NO_GUI
    void onButtonClick(ofxDatGuiButtonEvent e);
    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
#endif
};
