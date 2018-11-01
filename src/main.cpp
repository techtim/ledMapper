#include "ofMain.h"

#include "ledMapperApp.h"

#ifdef WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//========================================================================
int main()
{
    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2);    // <--- GL Programmable Renderer
    settings.setSize(1024, 768);
    ofCreateWindow(settings);       // <-------- setup the GL context

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ledMapperApp());
}
