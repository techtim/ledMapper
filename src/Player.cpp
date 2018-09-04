//
//  Player.cpp
//  ledMapper
//
//  Created by Tim Tvl on 7/15/18.
//
//

#include "Player.h"

static string s_playerConfName = "playerConf.json";

Player::Player()
    : m_gui(nullptr)
    , m_playing(false)
{
    setupGui();
}

Player::~Player() { ; }

void Player::setupGui()
{
#ifndef LED_MAPPER_NO_GUI
    m_gui = make_unique<ofxDatGui>(ofxDatGuiAnchor::TOP_RIGHT);
    m_guiTheme = make_unique<LedMapper::ofxDatGuiThemeLM>();

    m_gui->addHeader(LMGUIPlayer);

    auto togglePlay = m_gui->addToggle(LMGUITogglePlay, true);
    togglePlay->onButtonEvent(this, &Player::onButtonClick);

    auto slider = m_gui->addSlider(LMGUISliderFadeTime, 1, 30);
    slider->setPrecision(1);

    auto button = m_gui->addButton(LMGUIButtonSave);
    button->onButtonEvent(this, &Player::onButtonClick);
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    button = m_gui->addButton(LMGUIButtonLoad);
    button->onButtonEvent(this, &Player::onButtonClick);
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);

    /// TODO fix for unique_ptr, need to move DatGui to smart ptrs for that
    m_listVideos = new ofxDatGuiScrollView(LMGUIListPlaylist, 10);
    m_listVideos->onScrollViewEvent(this, &Player::onScrollViewEvent);
    m_listVideos->setNumVisible(10);
    m_listVideos->setBackgroundColor(ofColor(10));

    auto folder = m_gui->addFolder(LMGUIListPlaylist);
    folder->attachItem(m_listVideos);
    folder->expand();

    m_gui->setTheme(m_guiTheme.get(), true);
    m_gui->setWidth(LM_GUI_WIDTH);

#endif
}

void Player::draw(float x, float y, float w, float h)
{
    if (m_contentPlayers.empty() || m_playing)
        return;

    auto nowMs = ofGetElapsedTimeMillis();
    /// Duration and position return in seconds, check if it's time to show next video
    auto len = nowMs - m_contentPlayers[m_curContent].startMs;
    if (nowMs - m_contentPlayers[m_curContent].startMs
        > m_contentPlayers[m_curContent].durationMs - m_fadeMs) {
        setCurrentContent(m_curContent++);
    }

    ofSetColor(255);
    m_contentPlayers[m_curContent].draw(x, y, w, h);

    if (nowMs - m_contentPlayers[m_prevContent].startMs < m_fadeMs
        && nowMs - m_contentPlayers[m_prevContent].startMs
               < m_contentPlayers[m_prevContent].durationMs) {
        ofSetColor(ofMap(m_fadeMs - (nowMs - m_contentPlayers[m_prevContent].startMs), 0, m_fadeMs,
                         0, 255));
        m_contentPlayers[m_prevContent].draw(x, y, w, h);
    }

}

void Player::drawGui()
{
    if (m_gui == nullptr)
        return;

    m_listVideos->update();
    m_gui->update();
    m_gui->draw();
}

void Player::addContent(const string &path)
{
    std::filesystem::path pth(path);
    if (!pth.has_filename())
        return;

    ofVideoPlayer newVideo;
    newVideo.setPixelFormat(OF_PIXELS_RGBA);
    if (!newVideo.load(path))
        return;
    Content cont;

    cont.id = pth.filename().string();
    cont.path = path;
    cont.durationMs = newVideo.getDuration() * 1000;
    cont.type = "video";
    cont.video = move(newVideo);
    m_listVideos->add(cont.id);
    m_contentPlayers.emplace_back(move(cont));
}

void Player::deleteContent(const size_t id)
{
    if (id >= m_contentPlayers.size())
        return;

    m_listVideos->remove(id);

    m_contentPlayers.erase(m_contentPlayers.begin() + id);
}

void Player::setCurrentContent(size_t index)
{
    index %= m_contentPlayers.size();
    ofLogNotice() << "set current Content to #" << index;
    if (m_contentPlayers[m_prevContent].type == "video")
        m_contentPlayers[m_prevContent].video.stop();

    m_prevContent = m_curContent;
    m_curContent = index;
    m_contentPlayers[m_curContent].startMs = ofGetElapsedTimeMillis();

    if (m_contentPlayers[m_prevContent].type == "video")
        m_contentPlayers[m_curContent].video.play();
}

/// --------------------------- GUI EVENTS ----------------------------

void Player::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    if (e.parent->getName() == LMGUIListPlaylist) {
        // check if item from list selected
        if (e.target->getIndex()  < m_contentPlayers.size()) {
            ofLogNotice() << LMGUIListPlaylist << " clicked index #" << e.index;
            setCurrentContent(e.index);
        }
    }
}

void Player::onButtonClick(ofxDatGuiButtonEvent e)
{
    if (e.target->getName() == LMGUITogglePlay) {
        m_playing = !m_playing;
    }
}

/// --------------------------- LOAD / SAVE ---------------------------

void Player::save(const string &path)
{
    ofJson conf;
    conf["isPlaying"] = m_playing;
    conf["contents"] = m_contentPlayers;

    ofSavePrettyJson(path + s_playerConfName, conf);
}

void Player::load(const string &path)
{
    auto json = ofLoadJson(path + s_playerConfName);

    if (json.empty())
        return;

    if (json.count("contents")) {
        try {
            vector<Content> contents = json.at("contents").get<vector<Content>>();
            for (const auto &c : contents) {
                addContent(c.path);
            }
        }
        catch (...) {
            ofLogError() << "invalid json for Player";
        }
    }
    m_playing = json.at("isPlaying").get<bool>();
}
