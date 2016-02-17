#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofEnableAntiAliasing();
    ofSetEscapeQuitsApp(false);
    ofSetFrameRate(30);
    bShowGui= true;
    
    SyphonDir.setup();
    Syphon1.setup();
    Syphon2.setup();
    prev_dirIdx = -1;
    dirIdx = 0;
    rotatePos = 0;
    fbo1.allocate(SYPHON_W, SYPHON_H, GL_RGBA);
    fbo1.begin(); ofClear(0, 0, 0); fbo1.end();
    fbo2.allocate(SYPHON_W, SYPHON_H, GL_RGBA);
    fbo2.begin(); ofClear(0, 0, 0); fbo2.end();
    
    ledMapper = new ofxLedMapper(0);
    
    gui.setup("Syphon1"); // most of the time you don't need a name but don't forget to call setup
    gui.add(idSyphonServer.set("server num", 0, SyphonDir.size() - 1, SyphonDir.size()));
    gui.add(syphonW.set("width", 500, 100, SYPHON_W));
    gui.add(syphonH.set("height", 500, 200, SYPHON_H));
    gui.add(syphonX.set("x", 0, 0, ofGetWidth()));
    gui.add(syphonY.set("y", 0, 0,ofGetHeight()));
    gui.add(bRotate.set("rotate", false));
    gui.add(rotate.set("rotate speed", 0, -30,30));
    gui.add(colorFilter.set("color",ofColor(255,255,255,200),ofColor(0,0,0,0),ofColor(255,255,255,255)));
    gui.minimizeAll();
    //    gui.add(Alpha.set("ALPHA", 125.f, 0.f, 255.f));
    gui.loadFromFile("GUI.xml");
    
    ledMapper->load();
    textHelp = " Click on controller to activate / ESC to deactivate all \n CMD+Click - add line points in active controller / SHIFT+Click add circle of points \n BKSPS+Click - delete clicked point \n 's' - save , 'l' - load \n When turn off 'Show Controlles' you can switch between all controlles to show individual maps using LEFT/RIGHT keys";

}

//--------------------------------------------------------------
void ofApp::update(){
    if(idSyphonServer > SyphonDir.size() - 1) {
        dirIdx = 0;
    } else {
        dirIdx = idSyphonServer;
    }
    if (SyphonDir.size() > 0 && prev_dirIdx != dirIdx && dirIdx >=0) {
        Syphon1.set(SyphonDir.getDescription(dirIdx));
        syphonW.set("syphon Width", Syphon1.getWidth(), 100, 1920);
        syphonH.set("syphon Heigth", Syphon1.getHeight(), 100, 1920);
        prev_dirIdx = dirIdx;
        if (SyphonDir.size() > 1) Syphon2.set(SyphonDir.getDescription(dirIdx+1));
    }
    if(!bRotate) rotatePos = 0;
    idSyphonServer.setMax(SyphonDir.size() - 1);
    ledMapper->update(pix);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(0);    
    fbo1.begin();

    ofClear(0,0,0);
    
//    ofSet
    
//    ofTranslate(syphonW-syphonW/4, syphonH-syphonH/4);
    ofTranslate(syphonX, syphonY);
    ofRotateZ(rotatePos+=rotate);
//    ofTranslate(syphonW/2, syphonH/2);
    if (rotatePos>360) rotatePos = 0;

    ofSetColor(colorFilter);
    Syphon1.draw(-syphonW/2, -syphonH/2, syphonW,syphonH );
    
    
    
//        Syphon2.draw(SYPHON_W, 0, SYPHON_W, SYPHON_H);
    fbo1.end();
    
    fboReader.readToPixels(fbo1, pix, OF_IMAGE_COLOR_ALPHA);
    fbo1.draw(0, 0);

    ofSetColor(255);
    ofDrawCircle(syphonX, syphonY, 10);

    if (bShowGui) {
        gui.draw();
        ledMapper->draw();
    }
    ofSetWindowTitle("ledMapper (fps: "+ofToString(ofGetFrameRate())+")");
    ofSetColor(255,255,255,255);
    bHelp ? ofDrawBitmapString(textHelp, 10, 730) : ofDrawBitmapString("'h' - help", 10, 790);;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
//        case OF_KEY_SHIFT:
//            bRecordCircles = true;
//            break;
        case 's':
            ledMapper->save();
            gui.saveToFile("GUI.xml");
            break;
        case 'l':
            ledMapper->load();
            gui.loadFromFile("GUI.xml");
            break;
        case 'h':
            bHelp = !bHelp;
            break;
        case OF_KEY_ESC :
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
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
