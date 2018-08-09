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
    auto guiTheme = make_unique<LedMapper::ofxDatGuiThemeLM>();
    
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
    
    m_gui->setTheme(guiTheme.get(), true);
    m_gui->setWidth(LM_GUI_WIDTH);
#endif
}

void Player::draw(const ofVec2f &pos, const ofVec2f &size) {
    m_videoPlayers[m_curContent].play();
    m_videoPlayers[m_curContent].update();
//    if (m_videoPlayers[m_curContent].getDuration() - m_videoPlayers[m_curContent].getPositionInSeconds() < .1)
//        m_videoPlayers[m_curContent].stop();
    m_videoPlayers[m_curContent].draw(pos, size.x, size.y);
}

void Player::drawGui(){
    if (m_gui == nullptr)
        return;
    m_gui->draw();

}

void Player::addContent(const string &path){
    std::filesystem::path pth(path);
    if (!pth.has_filename())
        return;
    
    ofVideoPlayer newVideo;
    newVideo.setPixelFormat(OF_PIXELS_RGBA);
    newVideo.loadAsync(path);

    m_listVideos->add(pth.filename().string());
    m_videoPlayers.emplace_back(move(newVideo));
}

void Player::deleteContent(const size_t id){
    m_listVideos->remove(id);
//    m_videoPlayers.erase();
}

void Player::setCurrentContent(size_t index) {
    if (index >= m_videoPlayers.size())
        return;
    m_videoPlayers[m_prevContent].stop();
    m_prevContent = m_curContent;
    m_curContent = index;
    m_fadeStart = ofGetElapsedTimeMillis();
    m_videoPlayers[m_curContent].play();
}
//
// --------------------------- GUI EVENTS ----------------------------
//
#ifndef LED_MAPPER_NO_GUI
void Player::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    if (e.parent->getName() == LMGUIListControllers) {
        // check if item from list selected
        if (e.target->getIndex() < m_videoPlayers.size())
            m_curContent = e.target->getIndex();
    }
}

void Player::onButtonClick(ofxDatGuiButtonEvent e){
    if (e.target->getName() == LMGUITogglePlay) {
        m_playing = !m_playing;
    }
}

#endif
