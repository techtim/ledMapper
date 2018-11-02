//
//  Player.cpp
//  ledMapper
//
//  Created by Tim Tvl on 7/15/18.
//
//

#include "Player.h"

static string s_playerConfName = "playerConf.json";

ostream &operator<<(ostream &os, const Content &c)
{
    os << "content id=" << c.id << ", " << c.path << ", durationMs=" << c.durationMs
       << ", startMs=" << c.startMs;
    return os;
}

Player::Player()
    : m_playing(true)
    , m_curContent("")
    , m_prevContent("")
    , m_fadeMs(3000)
    , m_configPath(LedMapper::LM_CONFIG_PATH)
#ifndef LED_MAPPER_NO_GUI
    , m_gui(nullptr)
    , m_listVideos(nullptr)
#endif
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
    togglePlay->bind(m_playing);
    togglePlay->onButtonEvent(this, &Player::onButtonClick);

    auto slider = m_gui->addSlider(LMGUISliderFadeTime, 0, 10);
    slider->setPrecision(2);
    slider->onSliderEvent(this, &Player::onSliderEvent);

    auto button = m_gui->addButton(LMGUIButtonSave);
    button->onButtonEvent(this, &Player::onButtonClick);
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    button = m_gui->addButton(LMGUIButtonLoad);
    button->onButtonEvent(this, &Player::onButtonClick);
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);

    /// TODO fix for unique_ptr, need to move DatGui to smart ptrs for that
    m_listVideos = new ofxDatGuiScrollView(LMGUIListPlaylist, 10);
    m_listVideos->onScrollViewEvent(this, &Player::onScrollViewEvent);
    m_listVideos->setBackgroundColor(ofColor(10));
    m_listVideos->setTheme(m_guiTheme.get());

    button = m_gui->addButton(LMGUIListPlaylistDelete);
    button->onButtonEvent(this, &Player::onButtonClick);
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);

    auto folder = m_gui->addFolder(LMGUIListPlaylist);
    folder->attachItem(m_listVideos);
    folder->expand();

    m_gui->setTheme(m_guiTheme.get(), true);
    m_gui->setWidth(LM_GUI_WIDTH);
    m_gui->setAutoDraw(false);
#endif
}

void Player::draw(float x, float y, float w, float h)
{
    if (m_contentPlayers.empty() || !m_playing || m_curContent == "")
        return;

    auto nowMs = ofGetElapsedTimeMillis();
    auto &cur = m_contentPlayers[m_curContent];
    auto &prev = m_contentPlayers[m_prevContent];

    /// Duration and position return in seconds, check if it's time to show next video
    if (nowMs > cur.endMs - cur.fadeMs) {
        auto nextC = getNextContent(m_curContent);
        setCurrentContent(getNextContent(m_curContent));
    }

    ofSetColor(m_colorize);
    cur.draw(x, y, w, h);

    if (m_prevContent != "" && nowMs <= prev.endMs && nowMs > prev.endMs - prev.fadeMs) {
        float fadeColor = ofMap(prev.endMs - nowMs, prev.fadeMs, 0, 1.f, 0.f);
        ofSetColor(m_colorize.r, m_colorize.g, m_colorize.b, m_colorize.a * fadeColor);
        prev.draw(x, y, w, h);
    }
}

void Player::setGuiPosition(int x, int y) { m_gui->setPosition(x, y); }

ofxDatGui *Player::getGui() { return m_gui.get(); };

void Player::drawGui()
{
    if (m_gui == nullptr)
        return;

    m_gui->update();
    m_listVideos->update();
    m_gui->draw();
}

#include <regex>
template <typename T> inline string checkUniqueName(const string _name, map<string, T> &mapToCheck)
{
    auto it = mapToCheck.lower_bound(_name);
    if (it == mapToCheck.end()) {
        ofLogVerbose() << "this name is unique=" << _name;
        return _name;
    }

    regex clean_from_num(" [0-9]+$");

    string newName = regex_replace(_name, clean_from_num, "");
    regex check_for_integer(newName + " ([0-9]+)");

    smatch base_match;
    string num = "0";

    while (it != mapToCheck.end() && regex_match(it->first, base_match, check_for_integer)) {
        if (base_match.size() > 1) {
            string tmp_num = base_match[1].str();
            if (ofToInt(tmp_num) > ofToInt(num))
                num = tmp_num; // find out how to sort right
            ofLogVerbose() << _name << " has a num " << num << '\n';
        }
        ++it;
    }

    newName += ofToString(ofToInt(num) + 1);
    ofLogVerbose("FINAL unique name=" + newName);
    return newName + " " + ofToString(ofToInt(num) + 1);
}

void Player::addContent(const string &path)
{
    std::filesystem::path pth(path);
    if (!pth.has_filename())
        return;

    ofVideoPlayer newVideo;
    newVideo.setPixelFormat(OF_PIXELS_RGBA);
    newVideo.setLoopState(OF_LOOP_NONE);
    if (!newVideo.load(path))
        return;
    
    string id = checkUniqueName(pth.filename().string(), m_contentPlayers);
    Content cont;
    cont.id = id;
    cont.path = path;
    cont.durationMs = newVideo.getDuration() * 1000;
    cont.type = "video";
    cont.video = move(newVideo);
    ofLogVerbose() << "Add " << cont;

    m_listVideos->add(cont.id);
    m_contentCue.push_back(cont.id);
    m_contentPlayers[cont.id] = move(cont);

    setCurrentContent(id);
}

void Player::deleteContent(const string &id)
{
    auto it = find_if(begin(m_contentCue), end(m_contentCue),
                      [&id](const string &cueId) { return cueId == id; });
    if (it == end(m_contentCue))
        return;

    /// if deleting current set next content
    m_listVideos->remove(it - begin(m_contentCue));
    m_contentCue.erase(it);
    m_contentPlayers.erase(id);

    ofLogVerbose(m_prevContent == id ? "m_prevContent=" + m_prevContent + " was Deleted" : "");

    /// if deleting last set empty current and prev videos
    if (m_contentCue.size() == 1) {
        ofLogVerbose() << "[Player] Removing last content id=" << id;
        m_curContent = "";
        m_prevContent = "";
    }
    else if (id == m_curContent)
        setCurrentContent(getNextContent(m_curContent));

    updateListView();
}

void Player::setCurrentContent(const string &id)
{
    if (m_contentPlayers.empty() || m_contentPlayers.count(id) == 0)
        return;

    ofLogNotice() << "set current Content to id=" << id;

    auto nowMs = ofGetElapsedTimeMillis();

    if (m_contentPlayers.count(m_prevContent) && m_contentPlayers[m_prevContent].type == "video"
        && nowMs > m_contentPlayers[m_prevContent].endMs)
        m_contentPlayers[m_prevContent].video.stop();

    if (m_curContent == id) {
        m_contentPlayers[m_curContent].endMs = nowMs + m_contentPlayers[m_curContent].durationMs;
        if (m_contentPlayers[m_prevContent].type == "video") {
            m_contentPlayers[m_prevContent].video.play();
        }
        return;
    }

    /// if content deleted set prev content to empty
    m_prevContent = m_contentPlayers.count(m_curContent) ? m_curContent : "";

    ofLogVerbose(m_prevContent == id ? "m_prevContent=" + m_prevContent + " was Deleted" : "");

    m_curContent = id;
    m_contentPlayers[m_curContent].startMs = nowMs;
    m_contentPlayers[m_curContent].endMs = nowMs + m_contentPlayers[m_curContent].durationMs;
    /// fade longer than duration - set fade to duration/3
    m_contentPlayers[m_curContent].fadeMs = m_fadeMs > m_contentPlayers[m_curContent].durationMs
                                                ? m_contentPlayers[m_curContent].durationMs * 0.33
                                                : m_fadeMs;

    if (m_contentPlayers[m_curContent].type == "video")
        m_contentPlayers[m_curContent].video.play();

    updateListView();
}

const string Player::getNextContent(const string &id)
{
    if (m_contentCue.empty())
        return "";

    auto it = find_if(begin(m_contentCue), end(m_contentCue),
                      [&id](const string &cueId) { return cueId == id; });
    ofLogVerbose() << "[Player] getNextContent id=" << *it;
    /// if id not found or is last one return first in cue
    return (it == m_contentCue.end() || (it + 1) == m_contentCue.end()) ? m_contentCue.front()
                                                                          : *(it + 1);
}

void Player::setColorize(int r, int g, int b, int a) { m_colorize.set(r, g, b, a); }

void Player::updateListView()
{
#ifndef LED_MAPPER_NO_GUI
    for (auto &id : m_contentCue) {
        assert(m_listVideos->get(id));
        if (m_listVideos->get(id) != nullptr)
            m_listVideos->get(id)->setBackgroundColor(ofColor(30));
    }

    if (m_curContent != "") {
        assert(m_listVideos->get(m_curContent));
        m_listVideos->get(m_curContent)
            ->setBackgroundColor(ofColor::fromHex(LedMapper::LM_COLOR_GREEN_DARK));
    }
#endif
}

/// --------------------------- GUI EVENTS ----------------------------
#ifndef LED_MAPPER_NO_GUI
void Player::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    if (e.parent->getName() == LMGUIListPlaylist) {
        // check if item from list selected
        ofLogNotice() << LMGUIListPlaylist << " clicked content id=" << e.target->getLabel();
        setCurrentContent(e.target->getLabel());
    }
}

void Player::onButtonClick(ofxDatGuiButtonEvent e)
{
    if (e.target->getName() == LMGUIListPlaylistDelete) {
        deleteContent(m_curContent);
        return;
    }
    if (e.target->getName() == LMGUIButtonSave) {
        save(m_configPath);
        return;
    }
    if (e.target->getName() == LMGUIButtonLoad) {
        load(m_configPath);
        return;
    }
}

void Player::onSliderEvent(ofxDatGuiSliderEvent e)
{
    if (e.target->getName() == LMGUISliderFadeTime)
        m_fadeMs = e.target->getValue() * 1000; /// slide sets fade in seconds
}

#endif

/// --------------------------- LOAD / SAVE / RESET -----------------------

void Player::save(const string &path)
{
    m_configPath = path;
    ofJson conf;
    conf["isPlaying"] = m_playing;
    conf["fadeMs"] = m_fadeMs;

    /// Store contents in right order
    vector<Content> tmp;
    for (const auto &id : m_contentCue)
        tmp.push_back(m_contentPlayers[id]);
    conf["contents"] = tmp;

    ofSavePrettyJson(path + s_playerConfName, conf);
}

void Player::load(const string &path)
{
    m_configPath = path;
    auto json = ofLoadJson(path + s_playerConfName);

    if (json.empty())
        return;

    if (json.count("contents")) {
        /// clear current content
        reset();

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

    m_playing = json.at("isPlaying") ? json.at("isPlaying").get<bool>() : true;
    m_fadeMs = json.count("fadeMs") ? json.at("fadeMs").get<uint64_t>() : 1000;
}

void Player::reset()
{
    m_contentPlayers.clear();
    m_contentCue.clear();
#ifndef LED_MAPPER_NO_GUI
    m_listVideos->clear();
#endif
}
