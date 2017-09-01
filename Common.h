//
//  Common.h
//  ledMapper
//
//  Created by Timofey Tavlintsev on 8/9/17.
//
//
#pragma once

#ifndef LED_MAPPER_NO_GUI

#include "ofxDatGui.h"
#define LM_GUI_WIDTH 200
#define LMGUIListControllers "Controllers"
#define LMGUIToggleDebug "Debug controller"
#define LMGUITogglePlay "Play"
#define LMGUISliderFps "FPS"
#define LMGUIButtonAdd "Add Controller"


#define LCGUIButtonSend "Send"
#define LCGUIButtonDoubleLine "Double Line"
#define LCGUITextIP "IP"
#define LCGUITextPort "Port"
#define LCGUISliderPix "Pix in led"
#define LCGUIDropColorType "Color Type"
#define LCGUIButtonDmx "DMX"
#define LCFileName "Ctrl-"

#endif

//typedef unique_ptr<ofxLedController> ofxLedController_ptr;

#define LMCtrlsFolderPath "Ctrls"

#define RPI_IP "192.168.2.10"
#define RPI_PORT 3000

namespace LedMapper {
    
static const std::string APP_NAME = "LedMapper";

#ifdef WIN32
static const std::string CONFIG_PATH = "C:\\Program Files (x86)\\Resolume\\";
#elif defined(__APPLE__)
static const std::string CONFIG_PATH = "/Users/Shared/";
#endif

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Point {
    uint16_t x;
    uint16_t y;
};

static void to_json(ofJson& j, const Point& p) {
    j = ofJson{{"x", p.x}, {"y", p.y}};
}

static void from_json(const ofJson& j, Point& p) {
    p.x = j.at("x").get<uint16_t>();
    p.y = j.at("y").get<uint16_t>();
}

class ofxDatGuiThemeLedMapper : public ofxDatGuiTheme{
public:
    
    ofxDatGuiThemeLedMapper()
    {
        stripe.visible = false;
        color.label = hex(0x9C9DA1);
        color.icons = hex(0x9C9DA1);
        color.background = hex(0x28292E);
        color.guiBackground = hex(0x1E1F24);
        color.inputAreaBackground = hex(0x42424A);
        color.slider.fill = hex(0x6BE6B4);    // 107 230 180
        color.slider.text = hex(0x9C9DA1);
        color.textInput.text = hex(0x9C9DA1);
        color.textInput.highlight = hex(0x28292E);
        color.colorPicker.border = hex(0xEEEEEE);
        color.textInput.backgroundOnActive = hex(0x1D1E22);
        color.backgroundOnMouseOver = hex(0x42424A);
        color.backgroundOnMouseDown = hex(0x1D1E22);
        color.matrix.hover.button = hex(0x9C9DA1);
        color.graph.fills = hex(0x9C9DA1);
        stripe.button = hex(0x64ffda);
        stripe.toggle = hex(0x64ffda);
        init();
    }
};

} // LedMapper