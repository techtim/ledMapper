#include "ledMapperApp.h"

#define SYPHON_W 1600
#define SYPHON_H 1280

static const string s_configName = "config" + LedMapper::LM_CONFIG_EXTENSION;
static const string s_playerFolderPath = "Player";

static const vector<string> s_menuItems{ "Input", "Control", "Player" };

using namespace LedMapper;

//--------------------------------------------------------------
void ledMapperApp::setup()
{

#ifndef NDEBUG
    ofSetLogLevel(OF_LOG_VERBOSE);
#else
    ofSetLogLevel(OF_LOG_WARNING);
#endif
    
#ifdef WIN32
    // no-op
#elif defined(__APPLE__)
    ofSetDataPathRoot("../Resources/");
#elif defined(TARGET_LINUX)
    // no-op
#endif

    ofSetFrameRate(60);
    ofSetEscapeQuitsApp(false);
    ofSetVerticalSync(true);

    ofEnableAntiAliasing();
    ofSetCircleResolution(60);

    syphonW = 600;
    syphonH = 400;
    syphonX = 310;
    syphonY = 210;

    m_configPath = LM_CONFIG_PATH;
    m_configName = s_configName;

    bMenuExpanded = true;
    bSetupGui = false;
    bTestImage = false;
    bTestImageAnimate = false;
    animateHue = 0.0;

    m_menuSelected = s_menuItems.front();

    /// Init ofxLedMapper and Player instances before load
    m_ledMapper = make_unique<ofxLedMapper>();
    m_player = make_unique<Player>();
    
    load(m_configPath);

#ifdef TARGET_WIN32
    m_spoutIn.setup();
#elif defined(TARGET_OSX)
    SyphonDir.setup();
    Syphon1.setup();
#endif

    prev_dirIdx = -1;
    dirIdx = 0;

    m_fbo.allocate(SYPHON_W, SYPHON_H, GL_RGB);
    m_fbo.begin();
    ofClear(0, 0, 0);
    m_fbo.end();
    
    ofSetWindowTitle("ledMapper TVL");
    
    textHelp = " Hold '1' / '2' / '3' + Left Click - add 'line' / 'circle' / 'region' grab object "
               "in active controller \n Hold BKSPS + Left Click - on line edges to delete line \n "
               "UP/DOWN keys - switch between controllers \n 's' - save , 'l' - load \n When turn "
               "on 'Debug controller' you can switch between all controlles to solo each and map "
               "easily";
}

void ledMapperApp::setupGui()
{
    m_guiTheme = make_unique<ofxDatGuiThemeLM>();

    /// setup menu gui
    m_guiMenu
        = make_unique<ofxDatGuiFolder>("Menu", ofColor::fromHex(LedMapper::LM_COLOR_GREEN_DARK));
    for (const auto &it : s_menuItems) {
        auto but = m_guiMenu->addButton(it);
        but->onButtonEvent(this, &ledMapperApp::onButtonClick);
        but->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    }
    auto topTheme = make_unique<ofxDatGuiThemeLMTopMenu>();
    m_guiMenu->setTheme(topTheme.get());
    bMenuExpanded ? m_guiMenu->expand() : m_guiMenu->collapse();
    m_guiMenu->horizontal();

    /// setup gui responsible for video input
    m_guiInput->setAssetPath("");
    m_guiInput = make_unique<ofxDatGui>(ofxDatGuiAnchor::TOP_RIGHT);
    m_guiInput->setTheme(m_guiTheme.get());
    m_guiInput->setWidth(LM_GUI_WIDTH);
    m_guiInput->setAutoDraw(false);
    m_guiInput->addHeader("Input");

    m_syphonList = m_guiInput->addDropdown("source", vector<string>());
    m_syphonList->onDropdownEvent(this, &ledMapperApp::onDropdownEvent);
    updateVideoServers();

    m_guiInput->addSlider("width", 100, 1920)->bind(syphonW);
    m_guiInput->addSlider("height", 100, 1920)->bind(syphonH);
    m_guiInput->addSlider("X offset", 0, 1000)->bind(syphonX);
    m_guiInput->addSlider("Y offset", 0, 1000)->bind(syphonY);

    m_guiInput->addSlider("bright", 0, 255)->bind(filterA);
    m_guiInput->addSlider("red", 0, 255)->bind(filterR);
    m_guiInput->addSlider("green", 0, 255)->bind(filterG);
    m_guiInput->addSlider("blue", 0, 255)->bind(filterB);

    m_guiInput->addToggle("test image")->bind(bTestImage);
    m_guiInput->addToggle("animate image")->bind(bTestImageAnimate);

    bSetupGui = true;
    updateGuiPosition();
}

void ledMapperApp::updateGuiPosition()
{
    if (!bSetupGui)
        return;

    m_guiMenu->setPosition(ofGetWidth() - m_guiMenu->getWidth(), 0);
    m_guiInput->setPosition(ofGetWidth() - LM_GUI_WIDTH, LM_GUI_TOP_BAR);
    m_ledMapper->setGuiPosition(ofGetWidth() - LM_GUI_WIDTH, LM_GUI_TOP_BAR);
    m_player->setGuiPosition(ofGetWidth() - LM_GUI_WIDTH, LM_GUI_TOP_BAR);
}

/// on menu item selection focus on needed gui and set needed lambda to draw items gui
void ledMapperApp::selectMenuItem(const string &item)
{
    if (m_menuSelected == "Control") {
        m_ledMapper->setGuiActive();
        m_drawMenuGuiFunc = [this]() { m_ledMapper->drawGui(); };
    }
    else if (m_menuSelected == "Input") {
        m_guiInput->focus();
        m_drawMenuGuiFunc = [this]() {
            m_guiInput->update();
            m_guiInput->draw();
        };
    }
    else if (m_menuSelected == "Player") {
        m_player->getGui()->focus();
        m_drawMenuGuiFunc = [this]() { m_player->drawGui(); };
    }
}

//--------------------------------------------------------------
void ledMapperApp::update()
{
    updateVideoServers();
#ifdef TARGET_WIN32
    // receive Spout texture
    m_spoutIn.updateTexture();
#endif

    m_fbo.begin();

    ofClear(0, 0, 0);

    ofTranslate(syphonX, syphonY);

    ofFill();
    ofSetColor(filterR, filterG, filterB, filterA);

    /// Platform specific video stream input draw
#ifdef TARGET_WIN32
    m_spoutIn.getTexture().draw(-syphonW / 2, -syphonH / 2, syphonW, syphonH);
#elif defined(TARGET_OSX)
    if (Syphon1.getApplicationName() != "") {
        Syphon1.draw(-syphonW / 2, -syphonH / 2, syphonW, syphonH);
    }
#endif

    if (bTestImage) {
        if (bTestImageAnimate) {
            ofColor col;
            col.setHsb(animateHue++, 255, filterA);
            ofSetColor(col);
            if (animateHue > 255)
                animateHue = 0.0;
        }
        ofDrawRectangle(-syphonW / 2, -syphonH / 2, syphonW, syphonH);
    }

    /// update color filter settings and draw playing video if has one
    m_player->setColorize(filterR, filterG, filterB, filterA);
    m_player->draw(-syphonW / 2, -syphonH / 2, syphonW, syphonH);

    m_fbo.end();
    m_fbo.readToPixels(m_pixels);
}

//--------------------------------------------------------------
void ledMapperApp::draw()
{
    ofSetBackgroundColor(0);

    m_fbo.draw(0, 0);

    /// update LM with grabbed pixels to send them
    m_ledMapper->update(m_pixels);
    /// draw LM grab objects
    m_ledMapper->draw();

    /// GUI stuff
    if (bMenuExpanded != m_guiMenu->getIsExpanded()) {
        bMenuExpanded = m_guiMenu->getIsExpanded();
        ofLogVerbose() << "Toggle menu to " << bMenuExpanded;
        updateGuiPosition();
    }
    m_guiMenu->update();
    m_guiMenu->draw();

    if (m_guiMenu->getIsExpanded() && bSetupGui) {
        m_drawMenuGuiFunc();
    }

#ifndef NDEBUG
    ofSetWindowTitle("ledMapper (fps: " + ofToString(static_cast<int>(ofGetFrameRate())) + ")");
#endif

}

void ledMapperApp::updateVideoServers()
{
#ifdef TARGET_WIN32
/*if (bSetupGui && m_syphonList->size() != Spout.getSenderCount()) {
 vector<string> list;
 for (int i = 0; i < Spout.getSenderCount(); i++) {
 list.push_back(Spout.getSenderName(i));
 }
 m_syphonList->setDropdownList(list);
 if (prev_dirIdx == -1 && !list.empty()) {
 dirIdx = prev_dirIdx = 0;
 }

 }*/
#elif defined(TARGET_OSX)

    if (bSetupGui && m_syphonList->size() != SyphonDir.size()) {
        vector<string> list;
        for (auto &serv : SyphonDir.getServerList()) {
            list.push_back(serv.serverName + ": " + serv.appName);
        }
        m_syphonList->setDropdownList(list);
        // -1 initial value
        if (Syphon1.getApplicationName() == "" && !list.empty()) {
            dirIdx = prev_dirIdx = 0;
            Syphon1.set(SyphonDir.getDescription(dirIdx));
        }
    }

#endif
}

void ledMapperApp::save(const string &folderPath)
{
    ofJson conf;

    XML.clear();

    conf["screenPosX"] = ofGetWindowPositionX();
    conf["screenPosY"] = ofGetWindowPositionY();
    conf["screenWidth"] = ofGetWidth();
    conf["screenHeight"] = ofGetHeight();

    conf["bMenuExpanded"] = bMenuExpanded;
    conf["menuSelected"] = m_menuSelected;

    conf["syphonW"] = syphonW;
    conf["syphonH"] = syphonH;
    conf["syphonX"] = syphonX;
    conf["syphonY"] = syphonY;
    conf["brightness"] = filterA;
    conf["filterR"] = filterR;
    conf["filterG"] = filterG;
    conf["filterB"] = filterB;
    conf["bTestImage"] = bTestImage;
    conf["bTestImageAnimate"] = bTestImageAnimate;

    ofSavePrettyJson(folderPath + s_configName, conf);

    m_ledMapper->save(folderPath);
    m_player->save(folderPath);
}

bool ledMapperApp::load(const string &folderPath)
{
    /// if empty set default
    m_configPath = folderPath != "" ? folderPath : LM_CONFIG_PATH;

    auto conf = ofLoadJson(m_configPath + m_configName);

    /// if conf empty fill with defaults
    ofSetWindowPosition(conf.count("screenPosX") ? conf.at("screenPosX").get<int>() : 100,
                        conf.count("screenPosY") ? conf.at("screenPosY").get<int>() : 100);
    ofSetWindowShape(conf.count("screenWidth") ? conf.at("screenWidth").get<int>() : 1024,
                     conf.count("screenHeight") ? conf.at("screenHeight").get<int>() : 768);

    bMenuExpanded = conf.count("bMenuExpanded") ? conf.at("bMenuExpanded").get<bool>() : true;
    m_menuSelected
        = conf.count("menuSelected") ? conf.at("menuSelected").get<string>() : s_menuItems.front();

    syphonW = conf.count("syphonW") ? conf.at("syphonW").get<int>() : 600;
    syphonH = conf.count("syphonH") ? conf.at("syphonH").get<int>() : 400;
    syphonX = conf.count("syphonX") ? conf.at("syphonX").get<int>() : 300;
    syphonY = conf.count("syphonY") ? conf.at("syphonY").get<int>() : 200;
    filterA = conf.count("brightness") ? conf.at("brightness").get<int>() : 150;
    filterR = conf.count("filterR") ? conf.at("filterR").get<int>() : 255;
    filterG = conf.count("filterG") ? conf.at("filterG").get<int>() : 255;
    filterB = conf.count("filterB") ? conf.at("filterB").get<int>() : 255;
    bTestImage = conf.count("bTestImage") ? conf.at("bTestImage").get<bool>() : false;
    bTestImageAnimate
        = conf.count("bTestImageAnimate") ? conf.at("bTestImageAnimate").get<bool>() : false;

    m_ledMapper->load(folderPath);
    m_player->load(folderPath);

    setupGui();
    selectMenuItem(m_menuSelected);

    return true;
}

//--------------------------------------------------------------
void ledMapperApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
#ifdef TARGET_OSX
    if (e.child >= SyphonDir.size())
        return;
    dirIdx = e.child;
    Syphon1.set(SyphonDir.getDescription(dirIdx));
#endif
}

void ledMapperApp::onButtonClick(ofxDatGuiButtonEvent e)
{
    string name = e.target->getName();

    /// check for Menu items click
    if (find_if(begin(s_menuItems), end(s_menuItems),
                [&name](const string &it) { return name == it; })
        != end(s_menuItems)) {
        ofLogVerbose() << "Switch Menu to " << e.target->getName();
        m_menuSelected = e.target->getName();
        selectMenuItem(m_menuSelected);
    }
}

//--------------------------------------------------------------
void ledMapperApp::keyPressed(int key)
{

    switch (key) {
        case 's':
            save(m_configPath);
            break;
        case 'l':
            load(m_configPath);
            break;
        case 'h':
            bHelp = !bHelp;
            break;
#ifdef TARGET_WIN32
        case 'i':
            m_spoutIn.showSenders();
            break;
#endif
        default:
            break;
    }
}

//--------------------------------------------------------------
void ledMapperApp::keyReleased(int key) {}
//--------------------------------------------------------------
void ledMapperApp::mouseMoved(int x, int y) {}
//--------------------------------------------------------------
void ledMapperApp::mouseDragged(int x, int y, int button) {}
//--------------------------------------------------------------
void ledMapperApp::mousePressed(int x, int y, int button) {}
//--------------------------------------------------------------
void ledMapperApp::windowResized(int w, int h) { updateGuiPosition(); }

void ledMapperApp::dragEvent(ofDragInfo info)
{
    if (info.files.empty())
        return;

    auto dragPt = info.position;

    //        draggedImages.assign( info.files.size(), ofImage() );
    for (const auto &filePath : info.files) {

        /// Check for config file in folder, if found load it and folder it contains
        if (filePath.compare(filePath.size() - LM_CONFIG_EXTENSION.size(),
                             LM_CONFIG_EXTENSION.size(), LM_CONFIG_EXTENSION)
            == 0) {
            ofLogVerbose() << "!!!!!!! LedMapper CONFIG Path=" << filePath;
            std::filesystem::path pth(filePath);

            /// Update config name from default and load folder
            m_configName = pth.filename().string();
            string containingFolder = pth.parent_path().string();
            containingFolder.push_back(pth.preferred_separator);
            load(containingFolder);
            continue;
        }

        ofLogVerbose() << "DRAG FILE=" << filePath;
        /// Try import file to Player
        m_player->addContent(filePath);
    }
}

void ledMapperApp::exit()
{
#ifdef TARGET_WIN32
    m_spoutIn.exit();
#endif
}
