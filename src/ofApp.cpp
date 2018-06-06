#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

#ifndef NDEBUG
    ofSetLogLevel(OF_LOG_VERBOSE);
#else
    ofSetLogLevel(OF_LOG_NOTICE);
#endif

    ofSetEscapeQuitsApp(false);
    ofSetVerticalSync(true);

    ofEnableAntiAliasing();
    ofSetCircleResolution(60);

    syphonW = 600;
    syphonH = 400;
    syphonX = 300;
    syphonY = 200;

    bShowGui = true;
    bSetupGui = false;
    bTestImage = false;
    bTestImageAnimate = false;
    animateHue = 0.0;

    loadFromFile("GUI.xml");
    setupGui();

#ifdef TARGET_WIN32
    ofxSpout::init("", tex, syphonW, syphonH, false);
#elif defined(TARGET_OSX)
    SyphonDir.setup();
    Syphon1.setup();
#endif

    prev_dirIdx = -1;
    dirIdx = 0;

    fbo1.allocate(SYPHON_W, SYPHON_H, GL_RGB);
    fbo1.begin();
    ofClear(0, 0, 0);
    fbo1.end();

    ledMapper = make_unique<LedMapper::ofxLedMapper>();
    if (bSetupGui) {
        m_guiInput->update();
        ledMapper->setGuiPosition(m_guiInput->getPosition().x, m_guiInput->getPosition().y + m_guiInput->getHeight());
    }
    ledMapper->load();

    textHelp = " Hold '1' / '2' / '3' + Left Click - add 'line' / 'circle' / 'region' grab object "
               "in active controller \n Hold BKSPS + Left Click - on line edges to delete line \n "
               "UP/DOWN keys - switch between controllers \n 's' - save , 'l' - load \n When turn "
               "on 'Debug controller' you can switch between all controlles to solo each and map "
               "easily";
}

void ofApp::setupGui()
{
    /// setup gui responsible for video input
    m_guiInput->setAssetPath("");
    m_guiInput = make_unique<ofxDatGui>(ofxDatGuiAnchor::TOP_RIGHT);
    guiTheme = make_unique<LedMapper::ofxDatGuiThemeLM>();
    m_guiInput->setTheme(guiTheme.get());
    m_guiInput->setWidth(LM_GUI_WIDTH);

    m_guiInput->addHeader("Input");

    syphonList = m_guiInput->addDropdown("source", vector<string>());
    syphonList->onDropdownEvent(this, &ofApp::onDropdownEvent);
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

    /// Mouse Grab style buttons
    m_guiMenu = make_unique<ofxDatGui>(ofxDatGuiAnchor::TOP_LEFT);
    m_guiMenu->setTheme(guiTheme.get());
    m_guiMenu->setWidth(LM_GUI_WIDTH);


    auto button = m_guiMenu->addButtonImage(LMGUIMouseSelect, "gui/mouse_select.png",
                                            "gui/mouse_select_over.png");
    button->setPosition(10, 0);
    m_guiMenu->addButtonImage(LMGUIMouseGrabLine, "gui/mouse_grab_line.png",
                                            "gui/mouse_grab_line_over.png")->setPosition(10 + button->getWidth(), 0);

    m_guiMenu->addButtonImage(LMGUIMouseGrabCircle, "gui/mouse_grab_line.png", "gui/mouse_grab_line.png");
    m_guiMenu->addButtonImage(LMGUIMouseGrabCircle, "gui/mouse_grab_line.png", "gui/mouse_grab_line.png");

    m_guiMenu->onButtonEvent(this, &ofApp::onButtonClick);
    bSetupGui = true;
}

void ofApp::updateGuiPosition()
{
    m_guiInput->setPosition(ofxDatGuiAnchor::TOP_RIGHT);
    ledMapper->setGuiPosition(m_guiInput->getPosition().x, m_guiInput->getPosition().y + m_guiInput->getHeight());
}

//--------------------------------------------------------------
void ofApp::update()
{

    updateVideoServers();
    if (bSetupGui)
        m_guiInput->update();

#ifdef TARGET_WIN32
    // init receiver if it's not already initialized
    ofxSpout::initReceiver(tex);
    // receive Spout texture
    ofxSpout::receiveTexture(tex);
#endif

    fbo1.begin();

    ofClear(0, 0, 0);

    ofTranslate(LM_GUI_TOP_BAR+syphonX, LM_GUI_TOP_BAR+syphonY);

    ofFill();
    ofSetColor(ofColor(filterR, filterG, filterB, filterA));

//    if (m_menuSelected == "Input") {

#ifdef TARGET_WIN32
        tex.draw(-syphonW / 2, -syphonH / 2, syphonW, syphonH);
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

//    } else if (m_menuSelected == "Controllers") {
//
//    }


    fbo1.end();
    fbo1.readToPixels(pix);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetBackgroundColor(0);

    fbo1.draw(0, 0);
    ledMapper->update(pix);

    if (bShowGui && bSetupGui) {
        m_guiInput->draw();
        ledMapper->draw();
        m_guiMenu->update();
        m_guiMenu->draw();
    }

    ofSetWindowTitle("ledMapper (fps: " + ofToString(static_cast<int>(ofGetFrameRate())) + ")");
    ofSetColor(255, 255, 255, 255);
    bHelp ? ofDrawBitmapString(textHelp, 10, 730) : ofDrawBitmapString("'h' - help", 10, 790);
}

void ofApp::updateVideoServers()
{
#ifdef TARGET_WIN32
/*if (syphonList->size() != Spout.getSenderCount()) {
 vector<string> list;
 for (int i = 0; i < Spout.getSenderCount(); i++) {
 list.push_back(Spout.getSenderName(i));
 }
 syphonList->setDropdownList(list);
 if (prev_dirIdx == -1 && !list.empty()) {
 dirIdx = prev_dirIdx = 0;
 }

 }*/
#elif defined(TARGET_OSX)
    if (syphonList->size() != SyphonDir.size()) {
        vector<string> list;
        for (auto &serv : SyphonDir.getServerList()) {
            list.push_back(serv.serverName + ": " + serv.appName);
        }
        syphonList->setDropdownList(list);
        // -1 initial value
        if (Syphon1.getApplicationName() == "" && !list.empty()) {
            dirIdx = prev_dirIdx = 0;
            Syphon1.set(SyphonDir.getDescription(dirIdx));
        }
    }
#endif
}

void ofApp::saveToFile(const string &path)
{
    XML.clear();
    XML.addTag("syphon");
    XML.pushTag("syphon");
    XML.addValue("syphonW", syphonW);
    XML.addValue("syphonH", syphonH);
    XML.addValue("syphonX", syphonX);
    XML.addValue("syphonY", syphonY);
    XML.addValue("brightness", filterA);
    XML.addValue("filterR", filterR);
    XML.addValue("filterG", filterG);
    XML.addValue("filterB", filterB);
    XML.addValue("bTestImage", bTestImage);
    XML.addValue("bTestImageAnimate", bTestImageAnimate);

    XML.popTag();
    XML.save(path);
}

void ofApp::loadFromFile(const string &path)
{
    if (!XML.loadFile(path))
        return;
    
    XML.pushTag("syphon");
    syphonW = XML.getValue("syphonW", 600, 0);
    syphonH = XML.getValue("syphonH", 400, 0);
    syphonX = XML.getValue("syphonX", 300, 0);
    syphonY = XML.getValue("syphonY", 200, 0);
    filterA = XML.getValue("brightness", 150, 0);
    filterR = XML.getValue("filterR", 255, 0);
    filterG = XML.getValue("filterG", 255, 0);
    filterB = XML.getValue("filterB", 255, 0);
    bTestImage = XML.getValue("bTestImage", false, 0);
    bTestImageAnimate = XML.getValue("bTestImageAnimate", false, 0);

    XML.popTag();
}

//--------------------------------------------------------------
void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
#ifdef TARGET_OSX
    if (e.child >= SyphonDir.size())
        return;
    dirIdx = e.child;
    Syphon1.set(SyphonDir.getDescription(dirIdx));
#endif
}

void ofApp::onButtonClick(ofxDatGuiButtonEvent e)
{
    if (e.target->getName() == LMGUIMouseSelect) {
        ofLogWarning() << "Clicked " << LMGUIMouseSelect;
    }
    else if (e.target->getName() == LMGUIMouseGrabLine){
        ofLogWarning() << "Clicked " << LMGUIMouseGrabLine;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    switch (key) {
        case 's':
            ledMapper->save();
            saveToFile("GUI.xml");
            break;
        case 'l':
            ledMapper->load();
            loadFromFile("GUI.xml");
            break;
        case 'h':
            bHelp = !bHelp;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) { updateGuiPosition(); }

void ofApp::exit() {}