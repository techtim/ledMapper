//
//  Player.cpp
//  ledMapper
//
//  Created by Tim Tvl on 7/15/18.
//
//

#include "Player.h"

Player::Player()
: m_gui(nullptr)
, m_playing(false)
{
    setupGui();
}

Player::~Player(){;}

void Player::setupGui(){
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

void Player::draw(float x, float y, float w, float h) {
    if (m_videoPlayers.empty())
        return;

    m_videoPlayers[m_curContent].update();
    
    /// Duration and position return in seconds, check if it's time to show next video
    auto dur = m_videoPlayers[m_curContent].getDuration();
    auto pos = m_videoPlayers[m_curContent].getPosition();
    if (m_videoPlayers[m_curContent].getDuration() - m_videoPlayers[m_curContent].getPosition() < m_fadeMs * .001f)
        setCurrentContent(m_curContent++);
    
    if (ofGetElapsedTimeMillis() - m_fadeStart < m_fadeMs){
        m_videoPlayers[m_prevContent].update();
        m_videoPlayers[m_prevContent].draw(x, y, w, h);
    }
        
    m_videoPlayers[m_curContent].draw(x, y, w, h);
}

void Player::drawGui(){
    if (m_gui == nullptr)
        return;

    m_listVideos->update();
    m_gui->update();
    m_gui->draw();
    
}

void Player::addContent(const string &path){
    std::filesystem::path pth(path);
    if (!pth.has_filename())
        return;
    
    ofVideoPlayer newVideo;
    newVideo.setPixelFormat(OF_PIXELS_RGBA);
    if (!newVideo.load(path))
        return;

    m_listVideos->add(pth.filename().string());
    m_videoPlayers.emplace_back(move(newVideo));
}

void Player::deleteContent(const size_t id){
    if (id >=m_videoPlayers.size())
        return;

    m_listVideos->remove(id);

    m_videoPlayers.erase(m_videoPlayers.begin()+id);
}

void Player::setCurrentContent(size_t index) {
    
    index %= m_videoPlayers.size();
    ofLogNotice() << "set current Content to #" << index;
    m_videoPlayers[m_prevContent].stop();
    m_prevContent = m_curContent;
    m_curContent = index;
    m_fadeStart = ofGetElapsedTimeMillis();

    m_videoPlayers[m_curContent].play();
}

//

// --------------------------- GUI EVENTS ----------------------------
//
void Player::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    if (e.parent->getName() == LMGUIListPlaylist) {
        // check if item from list selected
        if (e.target->getIndex() < m_videoPlayers.size()) {
            setCurrentContent(e.index);
        }
        
    }
}

void Player::onButtonClick(ofxDatGuiButtonEvent e){
    if (e.target->getName() == LMGUITogglePlay) {
        m_playing = !m_playing;
    }
}
