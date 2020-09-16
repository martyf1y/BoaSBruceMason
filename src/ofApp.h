#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "BirdBox.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void drawGrid();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void changeColor();
    void setupGrid();
    void savePlayerImages();
    void loadBirdText();
    void displayDebugText();
    void displayFrozenImage();
    
    Boolean finalFrameComp(ofImage firstImgCheck, ofImage secondImgCheck);
    Boolean gameColorComp(ofImage firstImgCheck, ofImage secondImgCheck);
    // Bird content
    void loadImages();
    void loadSounds();
    void loadNewBirdImage();
    
    void displayTimeIndicator();
    void setupColorPalette();
    
    
    // Jenna UI
    void launchGUI();
    void guiEvent(ofxUIEventArgs &e);
    
    enum CurrentState{
        Passive,
        Playing
    };
    
    CurrentState state;
    
    // Kinect detection
    ofxOpenNI openNIDevice;
    
    ////////////////////////////////////// Adjustable varaibles //////////////////////////////////////
    // Variables to include
    /*
     Screen Size
     Flipping birds number
     Depth minimum and maximum
     Sounds?
     Bird image size
     Depth blocker size
     For loop colour checking amount (changes percent and framerate)
     */
    Boolean debugMode = false;              // Allow setting to change
    
   
    
    //GUI for debug / config mode
    ofxUISuperCanvas *gui1;
    
    //see if height / width of screen has changed (debug mode special)
    int oldDimWidth = 0;
    int oldDimHeight = 0;
    
    // Timer
    int absolutePlayTime = 7000;
    // int guiPlayTime;
    
    // Kinect depth
    int minDepthSetter = 2300;
    int maxDepthSetter = 3300;
    // int guiDepthMin, guiDepthMax;
    
    
    // Basic dimensions
    // float screenAdjust = 0.75;
    
    float silhoetteAdjust = 0.75; // this changes the image size
    
    int showBackEnd = 1;
    int dimWidth = 1280;                    // The screen size
    int dimHeight = 720;
    int debugPanelW = 420;
    int debugPanelH = dimHeight;
    
    // Grid stuff
    int numRows = 3;
    int numCols = 3;
    int numImagesToKeep = numRows * numCols;
    int boxW = dimWidth/numCols;
    int boxH = dimHeight/numRows;
    
    // Roof things
    Boolean roofHax = false;                 // Stop roof interferance
    int roofRectHeight = 0;
    
    int triggerPerAmount = 25;              // The amount to trigger the game
    int finalFrameTriggerPerAmount = 3;    // The amount to trigger the game
    
    int randBird = 1;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    
    ofRectangle kinectRect;
    // IMPORTANT THIS NEEDS TO CHANGE IF YOU CHANGE THE RESOLUTION
    int kinectWidth = 640;
    int kinectHeight = 480;
    
    // Bird stuff
    int birdCatalogSize = 12; // The amount of animals we have available
    
    ofColor colourBoxArray[12];
    ofImage birdImage[12];
    ofImage birdCatalog[12];
    string birdName[12];
    ofSoundPlayer birdSoundCatalog[13];
   // int gameBirdW;                          // The bird sizes on screen
   // int gameBirdH;
    
    vector<BirdBox*> birdBoxes;
    
    // Current variables
    ofImage currentBirdImage;
    ofImage resizedBirdImage;
    ofSoundPlayer currentBirdSound;
    int totalPlayerImages = 0;
    int currentlyShowing  = 0;
    int currentImageNumber = 0;
    ofImage savedShape; // The pixels used
    
    // Bird images Saved
    vector <ofImage> previousPlayImages;
    vector<vector <ofImage> > previousPlayBird;
    Boolean saveImage = false;
    int imgCounter = 0;
    
    // Ready timer
    Boolean readyToPlay=false; // This makes the playing timer go down
    int readyTimer = 1000;
    int readyTimerElapsed = 0;
    
    // Color detection
    ofImage beforeDepthImg; // Screengrab before the trigger capture
    ofImage afterDepthImg; // Screengrab of both depth and silhouette
    ofImage actualOnScreenGrab; // This is the full image of the everything seen on screen
    ofImage greyOnScreenGrab;
    
    // Changes the background colour
    ofColor randomCol;
    ofColor randomColDepth;
    
    // Colour palette - safest way to generate background colour
    int colorPaletteSize = 10;
    ofColor colorPalette[10];
    ofColor colorPaletteDepth[10];
    
    // Colour percentages
    int perCount; // gets percent amount for image
    int totCount; // gets total percent amount for image
    int percentage; // Overall percent
    
    //timer
    float triggerTimer = 0; // This is the actual timer that can be reset or changed
    float triggerStartTime; // Resets and changes the trigger timer
    Boolean playTrigger = false;
    
    ofRectangle birdRect;
    
    // Text
    int scrollingTextX = 0;
    ofxTrueTypeFontUC descriptionFont; // font with macrons
    
    
    // Grid stuff
    Boolean flipStatus = false;
    Boolean gridOut = true;
};
