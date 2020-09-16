#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
      //ofSleepMillis(200000);
    ofSetupOpenGL(1280,720,OF_FULLSCREEN);			// <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}



// Make sure bird is random,
// Make sure it starts in passive
// Make sure widths and heigths and timers are right
