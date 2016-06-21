#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetLogLevel(OF_LOG_VERBOSE);
	ofEnableAntiAliasing();
	ofSetEscapeQuitsApp(false);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

    SyphonDir.setup();
    Syphon1.setup();
    syphonW = 600;
    syphonH = 400;
    syphonX = 300;
    syphonY = 200;
    loadFromFile("GUI.xml");
    
	bShowGui= true;
    bSetupGui = false;
    setupGui();

	prev_dirIdx = -1;
	dirIdx = 0;
	rotatePos = 0;
	fbo1.allocate(SYPHON_W, SYPHON_H, GL_RGB);
	fbo1.begin(); ofClear(0, 0, 0); fbo1.end();

    ledMapper = make_unique<ofxLedMapper>(0);
    if (bSetupGui) {
        ledMapper->setGuiPosition(gui->getPosition().x, gui->getPosition().y+gui->getHeight());
    }
	ledMapper->load();

	textHelp = "CMD+Click - add line points in active controller / SHIFT+Click add circle of points \n BKSPS+Click - on line edges to delete line \n 's' - save , 'l' - load \n When turn on 'Debug controller' you can switch between all controlles to show individual maps"; //using LEFT/RIGHT keys

}

void ofApp::setupGui() {

    gui->setAssetPath("");
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->setWidth(SYPHON_GUI_WIDTH);

#ifdef TARGET_WIN32
    gui->addHeader("Spout");
#elif defined(TARGET_OSX)
    gui->addHeader("Syphon");
#endif

    syphonList = gui->addDropdown("source", vector<string>());
    syphonList->onDropdownEvent(this, &ofApp::onDropdownEvent);
    updateVideoServers();

    ofxDatGuiSlider* sInput;
    sInput = gui->addSlider("width", 100, 1920);
    sInput->bind(syphonW);
    
    sInput = gui->addSlider("height", 100, 1920);
    sInput->bind(syphonH);
    
    sInput = gui->addSlider("X offset", 0, 1000);
    sInput->bind(syphonX);
    
    sInput = gui->addSlider("Y offset", 0, 1000);
    sInput->bind(syphonY);
//    sInput->setWidth(SYPHON_GUI_WIDTH, .5f);
    sInput = gui->addSlider("bright", 0, 255);
    sInput->bind(filterA);
    sInput = gui->addSlider("red", 0, 255);
    sInput->bind(filterR);
    sInput = gui->addSlider("green", 0, 255);
    sInput->bind(filterG);
    sInput = gui->addSlider("blue", 0, 255);
    sInput->bind(filterB);
    
    bSetupGui = true;
}


//--------------------------------------------------------------
void ofApp::update(){

    updateVideoServers();
    if (bSetupGui) gui->update();
	fbo1.begin();

	ofClear(0,0,0);

	ofTranslate(syphonX, syphonY);

	if(!bRotate) {
		rotatePos = 0;
	} else {
		if (abs(rotatePos%360) < 3) {
			rotatePos = 0;
		}
		rotatePos += (360.f/ofGetFrameRate())*rotate;
//        ofLogVerbose("COEFF: "+ofToString((360.f/ofGetFrameRate())*rotate));
//        ofLogVerbose(ofToString(rotatePos));
		ofRotateZ(rotatePos);
	}

	ofSetColor(ofColor(filterR, filterG, filterB, filterA));
	Syphon1.draw(-syphonW/2, -syphonH/2, syphonW,syphonH );
	//        Syphon2.draw(SYPHON_W, 0, SYPHON_W, SYPHON_H);
	fbo1.end();
    fbo1.readToPixels(pix);
//	fboReader.readToPixels(fbo1, pix, OF_IMAGE_COLOR);

	ledMapper->update(pix);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(0);

	fbo1.draw(0, 0);

//	ofSetColor(255);
//	ofDrawCircle(syphonX, syphonY, 3);

	if (bShowGui && bSetupGui) {
		gui->draw();
		ledMapper->draw();
	}
	ofSetWindowTitle("ledMapper (fps: "+ofToString(ofGetFrameRate())+")");
	ofSetColor(255,255,255,255);
	bHelp ? ofDrawBitmapString(textHelp, 10, 730) : ofDrawBitmapString("'h' - help", 10, 790);;
}

void ofApp::updateVideoServers() {
#ifdef TARGET_WIN32
    
#elif defined(TARGET_OSX)
    if (syphonList->size() != SyphonDir.size()) {
        vector<string> list;
        for (auto &serv : SyphonDir.getServerList() ) {
            list.push_back(serv.serverName+": "+serv.appName);
        }
        syphonList->setDropdownList(list);
        // -1 initial value
        if (prev_dirIdx == -1 && !list.empty()) {
            dirIdx = prev_dirIdx = 0;
            Syphon1.set(SyphonDir.getDescription(dirIdx));
        }
    }
#endif
    
}

void ofApp::saveToFile(const string & path) {
    XML.clear();
    XML.addTag("syphon");
    XML.pushTag("syphon");
    XML.addValue("syphonW", syphonW);
    XML.addValue("syphonH", syphonH);
    XML.addValue("syphonX", syphonX);
    XML.addValue("syphonY", syphonY);
    XML.popTag();
    XML.save(path);
}

void ofApp::loadFromFile(const string & path) {
    if(!XML.loadFile(path)) return;
    XML.pushTag("syphon");
    syphonW = XML.getValue("syphonW", 600, 0);
    syphonH = XML.getValue("syphonH", 400, 0);
    syphonX = XML.getValue("syphonX", 300, 0);
    syphonY = XML.getValue("syphonY", 200);
    XML.popTag();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e) {
    if (e.child >= SyphonDir.size()) return;
    dirIdx = e.child;
    Syphon1.set(SyphonDir.getDescription(dirIdx));
    e.target->collapse();
    gui->collapse();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key) {
//        case OF_KEY_SHIFT:
//            bRecordCircles = true;
//            break;
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
	case OF_KEY_ESC:
		break;
//            bSelected = false;
	default:
		break;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    gui->setPosition(ofxDatGuiAnchor::TOP_RIGHT);
    ledMapper->setGuiPosition(gui->getPosition().x, gui->getPosition().y+gui->getHeight());
}

void ofApp::exit(){

}