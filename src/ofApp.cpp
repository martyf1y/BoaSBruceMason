#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetDataPathRoot("../Resources/data/");
    
    dimWidth = 1280;                    // The screen size
    dimHeight = 720;


    //Setup Kinect
    ofSetLogLevel(OF_LOG_VERBOSE);
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    //openNIDevice.setRegister(true);                                           // Some sort of register but it makes things look worse
    openNIDevice.setMirror(true);
    openNIDevice.start();
    
    descriptionFont.loadFont(ofToDataPath("Verdana Bold.ttf"), 18);         // Load font
    //DEBUG
    if (debugMode) {
        launchGUI();
    } else {
        //hide cursor
        ofHideCursor();
    }

    setupColorPalette();                                                        // Set array of background colours
    changeColor();
    
    for (int i = 0; i < birdCatalogSize; i++) {                                 // Set up image array for previous plays
        previousPlayBird.push_back( vector<ofImage>());
    }

    loadBirdText();                                                             // Set text names for birds
    loadImages();                                                               // Load bird images

    loadSounds();                                                               // Load sounds
    // loadNewBirdImage();
    currentImageNumber = 0;
    currentBirdImage=birdCatalog[1];
    currentBirdSound=birdSoundCatalog[1];
    setupGrid();                                                                // Setup the 12 grid for Display/Passive mode
    
   // openNIDevice.globalMinDepth = minDepthSetter;                               // Set initial depths
   // openNIDevice.globalMaxDepth = maxDepthSetter;
  //  gameBirdW = currentBirdImage.getWidth();
  //  gameBirdH = currentBirdImage.getHeight();
    // birdRect = ofRectangle(0, 0, gameBirdW*(imgScale*0.1), gameBirdH*(imgScale*0.1));       // This is the rectangle for the main screen
  //  birdRect = ofRectangle();
  //  birdRect.setFromCenter(dimWidth/2, dimHeight-kinectHeight/2, gameBirdW/((float)gameBirdW/(float)kinectWidth), gameBirdH/((float)gameBirdH/(float)kinectHeight));       // This is the rectangle for the main screen
    kinectRect = ofRectangle();
    kinectRect.setFromCenter(dimWidth/2, dimHeight-kinectHeight/2, kinectWidth, kinectHeight);
    state = Passive;

}


//--------------------------------------------------------------
void ofApp::update(){
    openNIDevice.update();
    
    if (debugMode) {
        
        //UPDATE VALUES TO-FROM GUI SLIDERS
        
        // Round Kinect Min-Max depth to nearest 100:
        // minDepthSetter = (guiDepthMin + 50) / 100 * 100;
        // maxDepthSetter = (guiDepthMax + 50) / 100 * 100;
        minDepthSetter = (minDepthSetter + 50) / 100 * 100;
        maxDepthSetter = (maxDepthSetter + 50) / 100 * 100;
        kinectRect.setFromCenter(dimWidth/2, dimHeight-kinectHeight/2, kinectWidth, kinectHeight);

        //round Play Time to nearest 1000: (second)
        // absolutePlayTime = (guiPlayTime + 500) / 1000 * 1000;
        absolutePlayTime = (absolutePlayTime + 500) / 1000 * 1000;
        
        //round Dimensions to nearest 10 pixels: (second)
        dimWidth = (dimWidth + 5) / 10 * 10;
        dimHeight = (dimHeight + 5) / 10 * 10;
        
        //round image scale to nearest 10 pixels: (second)
        // imgScale = (imgScale + 5) / 10 * 10;
        // birdRect = ofRectangle(0, 0, kinectWidth, kinectHeight);       // This is the rectangle for the main screen
        
        //update Kinect with new depths
       // openNIDevice.setMinDepth(minDepthSetter);
       // openNIDevice.setMaxDepth(maxDepthSetter);
        
        // update grid with new x,y,w,h .. IF dim Width / Height has changed
        if (dimWidth != oldDimWidth || dimHeight != oldDimHeight) {
            // grid:
            
            boxW = dimWidth/numCols;
            boxH = dimHeight/numRows;
            
            int num=0;
            for (int i=0; i<numRows; i++) {
                for (int j=0; j<numCols; j++) {
                    
                    
                    //Set x and y
                    int tX = boxW*j;
                    int tY = boxH*i;
                    
                    birdBoxes[num]->updateDimensions(tX,tY,boxW,boxH);
                    
                    num++;
                }
            }
        }
        oldDimWidth = dimWidth;
        oldDimHeight = dimHeight;
        
    }
    
    switch(state){
            
            /////////////////////////// Passive ///////////////////////////////
        case Passive:{
            
            int totalBoxes = numRows*numCols;
            
            //Update objects in grid
            for (int i=0; i<totalBoxes; i++) {
                birdBoxes[i]->update(flipStatus);
            }
            
        }break;
            
            /////////////////////////// Playing ///////////////////////////////
        case Playing:{
            
        }break;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch(state){
            
            /////////////////////////// Passive ///////////////////////////////
        case Passive:{
            if(saveImage){
                savePlayerImages();
                saveImage = false;
                gridOut = true; // Just put it here to make sure it works
            }
            displayFrozenImage();
            // Timer
            triggerTimer =  ofGetElapsedTimeMillis() - triggerStartTime;
            
            
            //////////// Grid time ////////////
            if(triggerTimer >= 13000 && gridOut){ // Give players some time to view all of the grid
                if (currentBirdSound.isPlaying()) {
                    currentBirdSound.stop();
                }
                loadNewBirdImage();
                changeColor();
                // This part flips through between grid and new silhoette image
                ofSetColor(randomCol);
                ofSetRectMode(OF_RECTMODE_CENTER);
                ofDrawRectangle(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The grid background
                
                ofSetColor(1, 1, 1, 254); // DONT GO FULL BLACK IN CASE IMAGE USES IT!!!
                currentBirdImage.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // Silhouette
                ofSetColor(255);
                ofSetRectMode(OF_RECTMODE_CORNER);
                beforeDepthImg.grabScreen(0,0, dimWidth, dimHeight); //check back
                
                /// Lets do it again!!!
                int num=0;
                //update the grid
                for (int i=0; i<numRows; i++) {
                    for (int j=0; j<numCols; j++) {
                        //Calculate x and y
                        int tX = boxW*j;
                        int tY = boxH*i;
                        
                        // Chop up the current image on screen.
                        // Into 12 separate pieces so we can turn them over
                        ofImage choppedBird = greyOnScreenGrab;
                        
                        choppedBird.crop(tX,tY,boxW,boxH);
                        
                        ofImage shrunkBird = beforeDepthImg;
                        shrunkBird.crop(tX,tY,boxW,boxH);
                        
                        birdBoxes[num]->setNewImages(shrunkBird, choppedBird);
                        
                        num++;
                    }
                }
                gridOut = false;
            }
            
            ////////// TIME: Grid! Start flippin'
            if(triggerTimer >= 3000){
                drawGrid();
                if(triggerTimer >= 11500 && gridOut){
                    greyOnScreenGrab.grabScreen(0, 0, dimWidth, dimHeight); // This is the image of the 12 screens
                }
            }
            
            if (triggerTimer < 7500){
                int textLength = birdName[currentImageNumber].length();
                ofSetColor(0, 0, 0);
               //  descriptionFont.drawString(birdName[currentImageNumber], 30, dimHeight - 25);
                ofSetColor(255, 255, 255);
            }
            
            //////////// TIME: Done! Switch to PLAY mode
            if(triggerTimer >= 21000){ // Give players some time to view all of the grid
                //Stop the sounds
                
                state = Playing;
                triggerStartTime = ofGetElapsedTimeMillis();
                playTrigger=false;
            }
            
        } break;
            
            ///////////////////////////Playing///////////////////////////////
        case Playing:{
            
            ofSetColor(randomCol);
           // ofSetRectMode(OF_RECTMODE_CENTER);
            //cout << "main Space" << endl;
            ofDrawRectangle(kinectRect); // The main space
            ofSetRectMode(OF_RECTMODE_CENTER); // This is here because the calculation needs to be centered below.
            ofSetColor(1, 1, 1, 254); // DONT GO FULL BLACK IN CASE IMAGE USES IT!!!
            currentBirdImage.draw(dimWidth/2, dimHeight -kinectHeight/2, kinectRect.width, kinectRect.height); // Silhouette
            ofSetColor(255);
            ofSetRectMode(OF_RECTMODE_CORNER);

            beforeDepthImg.grabScreen(kinectRect.x, kinectRect.y, kinectRect.width, kinectRect.height);
            
            ofSetColor(randomColDepth);
            openNIDevice.drawDepth(kinectRect.x-7, kinectRect.y, kinectRect.width+7, kinectRect.height);
            afterDepthImg.grabScreen(kinectRect.x, kinectRect.y, kinectRect.width, kinectRect.height); // This is the image with depth
            
            
           
            Boolean setReadySwitch = gameColorComp(beforeDepthImg, // Checking this image
                                                   currentBirdImage); // Against this image
                                                   //We compare the two images together and see if they are the same
            
            
            if (!setReadySwitch) {
                //reset the ready to play timer...
                readyTimerElapsed=ofGetElapsedTimeMillis();
            }
            
            if (readyTimer < ofGetElapsedTimeMillis() - readyTimerElapsed) {
                readyToPlay=true;
            }
            
            /////////////////////////// THE ACTUAL IMAGE ///////////////////////////
            // ofSetColor(50, 50, 50);
            // ofRect(0,0, dimWidth, dimHeight); // The main space
            ofSetColor(255, 255, 255);
            if(showBackEnd == 1){
       
                ofSetColor(randomCol);
                ofSetRectMode(OF_RECTMODE_CENTER);
                ofDrawRectangle(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The main space
                
                ofSetColor(randomColDepth);
                openNIDevice.drawDepth(dimWidth/2-7,dimHeight/2, dimWidth+14, dimHeight);
                ofSetColor(1, 1, 1, 254); // DONT GO FULL BLACK IN CASE IMAGE USES IT!!!
                currentBirdImage.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // Silhouette
                ofSetRectMode(OF_RECTMODE_CORNER);
                //  afterDepthImg.draw(0, 0, dimWidth, dimHeight); // The after depth image
            }
            // 1/3 top rectangle cover up (Aotea-specific roof fix hack)
            if (roofHax) {
                //    ofSetColor(randomCol);
                //    ofRect(0,0,dimWidth, roofRectHeight);
            }
            
            // Wait until triggered
            if (!readyToPlay)  {
                triggerStartTime = ofGetElapsedTimeMillis();
            }
            
            // Timer
            triggerTimer =  absolutePlayTime - (ofGetElapsedTimeMillis() - triggerStartTime);
            
            //Indicate time left to play
            if(triggerTimer >= 0){ // Don't want dimmer to be there
                displayTimeIndicator();
                ofSetColor(255, 255, 255);
                ofSetRectMode(OF_RECTMODE_CENTER);
                savedShape.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The image created by the depth
                ofSetRectMode(OF_RECTMODE_CORNER);
                ofSetColor(0, 0, 0);
                descriptionFont.drawString(birdName[currentImageNumber], 30, dimHeight - 20);
                
                ofSetColor(255, 255, 255);
                greyOnScreenGrab.grabScreen(0,0, dimWidth, dimHeight); //

            }
            else{
                ofSetColor(255, 255, 255);
                ofSetRectMode(OF_RECTMODE_CENTER);
                savedShape.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The image created by the depth
                ofSetRectMode(OF_RECTMODE_CORNER);

            }
            // Time up!
            /////////////////////////// Capture images, switch to Passive/Display mode ///////////////////////////
            if(triggerTimer <= 0){
                readyToPlay=false;
                triggerStartTime = ofGetElapsedTimeMillis();
                //      cout << "BAM!" << endl;
                Boolean isThereSomething = finalFrameComp(beforeDepthImg, // Checking this image
                                                          afterDepthImg // Against this image
                                                          );//We compare the two images together and see if they are the same
                
                
                // displayFrozenImage();
                //     cout << percentage << endl;
                if(!isThereSomething) // If we do not think there is enough to trigger a saved image
                {
                    playTrigger=false;
                    if (currentBirdSound.isPlaying()) {
                        currentBirdSound.stop();
                        displayFrozenImage();
                    }
                }
                else{ // SAVE SAVE SAVE!!!
                    //  cout << "TRUE? " << endl;
                    
                    //////// I PUT THIS IN AGAIN TO RESET SAVED SHAPE ////////////
                    Boolean setReadySwitch = gameColorComp(beforeDepthImg, // before depth
                                                           currentBirdImage // The bird image
                                                           );//We compare the two images together and see if they are the same
                    
                    state = Passive;
                    
                    //Display bird description over top
                    int textLength = birdName[currentImageNumber].length();
                    ofSetColor(0);
                    actualOnScreenGrab.grabScreen(0, 0, dimWidth, dimHeight); // This is the proper image
                    
                    displayFrozenImage();
                    ofSetColor(0, 0, 0);
                    
                    ofSetColor(255, 255, 255);
                    saveImage = true; // This allows the saved images to load without freexing the program
                }
            }
            
        } break;
    }
    if (debugMode) {
        //Display debug stats
        displayDebugText();
    }
    
    
    // Outside Space
    ofSetColor(255, 0, 0);
    ofDrawLine(dimWidth+1, 0, dimWidth+1, dimHeight+1);
    ofDrawLine(0, dimHeight+1, dimWidth+1, dimHeight+1);

}

//--------------------------------------------------------------
void ofApp::launchGUI() {
    
    // guiDepthMin = minDepthSetter;
    // guiDepthMax = maxDepthSetter;
    // guiPlayTime = absolutePlayTime;
    
    gui1 = new ofxUISuperCanvas("Dynamic Config.", 10,10,debugPanelW,debugPanelH, OFX_UI_FONT_MEDIUM);
    //string _label, float x, float y, float w, float h, int _size = OFX_UI_FONT_MEDIUM);
    
    // KINECT DEPTH MIN
    gui1->addIntSlider("Depth Min", 0, 4000, &minDepthSetter)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();

    // KINECT DEPTH MAX
    gui1->addIntSlider("Depth Max", 500, 8000, &maxDepthSetter)->setTriggerType(OFX_UI_TRIGGER_BEGIN|OFX_UI_TRIGGER_CHANGE|OFX_UI_TRIGGER_END);
    gui1->addSpacer();
    
    // KINECT Dimension Width
    gui1->addIntSlider("Scrren Width", 200, 4000, &dimWidth)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();

    // KINECT Dimension Height
    gui1->addIntSlider("Screen Height", 200, 4000, &dimHeight)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();
    
    // Bird Size
    gui1->addRotarySlider("Bird Size", 0, 1.0, &silhoetteAdjust, 150, 50, 50)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();
    
    // Image Scale
    gui1->addIntSlider("Show Back End ", 0, 1, &showBackEnd)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();
    
    // PLAYING TIME
    gui1->addIntSlider("Play Time", 2000, 300000, &absolutePlayTime)->setTriggerType(OFX_UI_TRIGGER_ALL);
    
    gui1->addSpacer();
    gui1->addIntSlider("Percentage Silhouette Trigger", 5, 60, &triggerPerAmount)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();

    gui1->addIntSlider("Percentage Frame Trigger", 5, 30, &finalFrameTriggerPerAmount)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();
    
    gui1->addIntSlider("Roof Height", 0, dimHeight, &roofRectHeight)->setTriggerType(OFX_UI_TRIGGER_ALL);
    
    gui1->addSpacer();
    
    gui1->addIntSlider("Random Bird", 0, 1, &randBird)->setTriggerType(OFX_UI_TRIGGER_ALL);
    gui1->addSpacer();

    
    ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
    
    
}

//--------------------------------------------------------------
void ofApp::displayDebugText() {
    
    //DEBUG STATS
    
    int startY=dimHeight;
    
    ofSetColor(255);
    
    // ofDrawBitmapString("Min Depth: " + ofToString(minDepthSetter), 10, startY+20);
    // ofDrawBitmapString("Max Depth: " + ofToString(maxDepthSetter), 10, startY+40);
    
    ofDrawBitmapString("FRACTION MADE: " + ofToString(perCount) + " / " + ofToString(totCount), 15, startY-100);
    ofDrawBitmapString("PERCENT MADE: " + ofToString(percentage) + "%", 15, startY-80);
    
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, startY-60);
    ofDrawBitmapString("Mode: " + ofToString(state), 15, startY-40);
    ofDrawBitmapString("Total Game Time: " + ofToString(triggerTimer), 15, startY-20);
    
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    string name = e.getName();
    int kind = e.getKind();
    
}


Boolean ofApp::gameColorComp(ofImage firstImgCheck, ofImage secondImgCheck){
    
    savedShape.allocate(secondImgCheck.getWidth(), secondImgCheck.getHeight(), OF_IMAGE_COLOR_ALPHA); // This should always be whatever the image we are comparing to
    
    totCount = 0;
    perCount = 0;
    
    ofSetColor(255,255, 255);
    
    float kWidthToImgWidth = (float)secondImgCheck.getWidth() / (float)(firstImgCheck.getWidth());
    float kHeightToImgHeight = (float)secondImgCheck.getHeight() / (float)firstImgCheck.getHeight();
    //  cout << kWidthToImgWidth << endl;
    //  cout << kHeightToImgHeight << endl;
    //IMPORTANT!! BECAUSE WE ARE CHECKING THE COLOUR TO THE LEFT, RIGHT AND 3 ABOVE WE NEED TO NOT START AT 0 OTHERWISE IT MIGHT FREEZE
    for(int x = 1; x < firstImgCheck.getWidth(); x++){
        float imgColPosX = x*kWidthToImgWidth; // Colour position of actual image at X
        
        for(int y = 1; y < firstImgCheck.getHeight(); y++){// Took out top thrid because it is blocked by roof
            float imgColPosY = y*kHeightToImgHeight; // Colour position of actual image at Y
            
            ofColor birdCol = secondImgCheck.getColor(imgColPosX, imgColPosY);// This is sourcing the image colour at a certain location
            
            ofColor transCol = ofColor(255, 255, 255, 0); // transparency
            // This is needed 4 times because the conversion between image and kinect means there is a gap between checks
            savedShape.setColor(imgColPosX -1, imgColPosY -1, transCol); // -x, -y
            savedShape.setColor(imgColPosX, imgColPosY -1, transCol); // x, -y
            // savedShape.setColor(imgColPosX +1, imgColPosY -1, transCol); // +x, -y
            savedShape.setColor(imgColPosX -1, imgColPosY, transCol); // -x, y
            savedShape.setColor(imgColPosX, imgColPosY, transCol); // x, y
            //  savedShape.setColor(imgColPosX +1, imgColPosY, transCol); // +x, y
            
            
            if(birdCol.a != 0){ // We only need to check the alpha dots
                ofColor orgCol = firstImgCheck.getColor(x, y);
                float orgColR = orgCol.r;
                float orgColG = orgCol.g;
                float orgColB = orgCol.b;
                float orgColA = orgCol.a;
                
                
                ofColor capCol = afterDepthImg.getColor(x, y);
                float capColR = capCol.r;
                float capColG = capCol.g;
                float capColB = capCol.b;
                float capColA = capCol.a;
                
                if(orgColR != capColR || orgColG != capColG || orgColB != capColB || orgColA != capColA){
                    perCount ++;
                    
                    savedShape.setColor(imgColPosX, imgColPosY, birdCol);
                   
                    
                    birdCol = secondImgCheck.getColor(imgColPosX -1, imgColPosY -1);
                    savedShape.setColor(imgColPosX -1, imgColPosY -1, birdCol);
                    birdCol = secondImgCheck.getColor(imgColPosX, imgColPosY-1);
                    savedShape.setColor(imgColPosX, imgColPosY-1, birdCol);
                    //    birdCol = secondImgCheck.getColor(imgColPosX +1, imgColPosY -1);
                    //    savedShape.setColor(imgColPosX +1, imgColPosY -1, birdCol);
                    birdCol = secondImgCheck.getColor(imgColPosX -1, imgColPosY);
                    savedShape.setColor(imgColPosX -1, imgColPosY, birdCol);
                    //    birdCol = secondImgCheck.getColor(imgColPosX+1, imgColPosY);
                    //    savedShape.setColor(imgColPosX+1, imgColPosY, birdCol);
                    
                }
                totCount ++;
            }
        }
    }
    savedShape.update();
    
    percentage = ((float)perCount / (float)totCount)*100;
    //trying out a different trigger for play
    // if we haven't already triggered the game, then look at the center point of the image and wait...
    if (!playTrigger ) {
        int area = 2;
        if (percentage >= triggerPerAmount) {
            //cout << "TRIGGERED PLAY! Let's go! " << x << " and " << y << endl;
            playTrigger=true;
            readyTimerElapsed=ofGetElapsedTimeMillis();
            currentBirdSound.play();
        }
    }
    return playTrigger;
}



Boolean ofApp::finalFrameComp(ofImage firstImgCheck, ofImage secondImgCheck){
    
    totCount = 0;
    perCount = 0;
    
    ofSetColor(255,255, 255);
    int maxValueCheck = 0; // It were originally halfing the screen check but we need to check all otherwise it is too small
    
    for(int x = 2; x < firstImgCheck.getWidth() - 2; x++){
        for(int y = 2; y < firstImgCheck.getHeight(); y++){// Top third is still there
            ofColor capCol = secondImgCheck.getColor(x, y);// This will be the after depth
            
            if(capCol.a != 0){ // We only need to check the alpha dots
                ofColor orgCol = firstImgCheck.getColor(x, y);
                float orgColR = orgCol.r;
                float orgColG = orgCol.g;
                float orgColB = orgCol.b;
                float orgColA = orgCol.a;
                
                float capColR = capCol.r;
                float capColG = capCol.g;
                float capColB = capCol.b;
                float capColA = capCol.a;
                
                if(orgColR != capColR || orgColG != capColG || orgColB != capColB || orgColA != capColA){
                    perCount ++;
                }
                totCount ++;
            }
        }
    }
    
    percentage = ((float)perCount / (float)totCount)*100;
    cout << "PERCENTAGE AGAIN: " << percentage << endl;
    cout << "finalFrameTriggerPerAmount AGAIN: " << finalFrameTriggerPerAmount << endl;

    // Checks the trigger for the final frame and says if it is true or not
    if(percentage > finalFrameTriggerPerAmount){
        //cout << "TRIGGERED PLAY! Let's go! " << x << " and " << y << endl;
        playTrigger=true;
        readyTimerElapsed=ofGetElapsedTimeMillis();
        //  currentBirdSound.play();
    }
    else{
        playTrigger=false;
    }
    return playTrigger;
}


void ofApp:: displayFrozenImage() {
    
    //Display frozen image
    ofSetColor(255);
    greyOnScreenGrab.draw(0,0);
}

void ofApp::changeColor() {
    
    // Pick at random one of the colours in the array
    int rCol = ofRandom(0,colorPaletteSize);
    
    // Change bg color
    randomCol = colorPalette[rCol];
    // Change depth color
    randomColDepth = colorPaletteDepth[rCol];
}

void ofApp::drawGrid() {
    
    //Set border edge for each box in grid
    int num=0;
    
    //Draw grid
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {
            
            //Draw individual box
            birdBoxes[num]->display();
            
            num++;
        }
    }
}


void ofApp::savePlayerImages() {
    
    // remove the first image from the vector (decrease capacity, so we can put a new image in)
    // numImagesToKeep is set to 12?? at the moment, per type of bird
    
    if (previousPlayBird[currentImageNumber].size()>=numImagesToKeep) {
        previousPlayBird[currentImageNumber].erase(previousPlayBird[currentImageNumber].begin());
    }
    string time = ofGetTimestampString();
    // This is the cut out bird shape
  //  savedShape.saveImage("images/saved/cutbird_"+time+".png");
    previousPlayBird[currentImageNumber].push_back(actualOnScreenGrab);
    
    // This is the entire screen grab image
   // actualOnScreenGrab.saveImage("images/saved/shape_"+time+".png");
    
    // Total count for all saved images, just goes up and up...
    imgCounter++;
    
    //To show previous player images on screen, count how many there are now
    totalPlayerImages = previousPlayBird[currentImageNumber].size();
    currentlyShowing  = totalPlayerImages;
    
    // shrink the previous played images...
    // ofImage shrunkDepth;
    ofImage shrunkBird;
    
    int num=0;
    //update the grid
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {
            
            // display previously played image in this box
            if (num < totalPlayerImages) {
                shrunkBird = previousPlayBird[currentImageNumber][num];
            }
            // otherwise display the current played image in this box
            else {
                shrunkBird= beforeDepthImg;
                
            }
            
            shrunkBird.resize(boxW,boxH);
            
            //Calculate x and y
            int tX = boxW*j;
            int tY = boxH*i;
            
            // Chop up the current image on screen.
            // Into 12 separate pieces so we can turn them over
            ofImage choppedBird = greyOnScreenGrab;
            
            choppedBird.crop(tX,tY,boxW,boxH);
            
            birdBoxes[num]->setNewImages(shrunkBird, choppedBird);
            
            num++;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::loadImages() {
    
    
    birdCatalog[0].load("images/fantail.png");
    birdCatalog[1].load("images/dotterel.png");
    birdCatalog[2].load("images/kingfisher.png");
    birdCatalog[3].load("images/magpie.png");
    birdCatalog[4].load("images/pigeon.png");
    birdCatalog[5].load("images/quail.png");
    birdCatalog[6].load("images/kakapo.png");
    birdCatalog[7].load("images/penguin.png");
    birdCatalog[8].load("images/kokako.png");
    birdCatalog[9].load("images/oystercatcher.png");
    birdCatalog[10].load("images/whio.png");
    birdCatalog[11].load("images/seagull.png");
}

//--------------------------------------------------------------
void ofApp::loadSounds() {
    //Load Sounds * * * * * * * * * * * * * *
    
    //replace with for loop above

    
   // birdSoundCatalog[0].load("sounds/fantail.mp3");
    birdSoundCatalog[1].load("sounds/dotterel.mp3");
    
    birdSoundCatalog[2].load("sounds/kingfisher.mp3");
    birdSoundCatalog[3].load("sounds/magpie.mp3");
    birdSoundCatalog[4].load("sounds/pigeon.mp3");
    
    birdSoundCatalog[5].load("sounds/quail.mp3");
    birdSoundCatalog[6].load("sounds/kakapo.mp3");
    
    birdSoundCatalog[7].load("sounds/penguin.mp3");
    birdSoundCatalog[8].load("sounds/kokako.mp3");
    birdSoundCatalog[9].load("sounds/oystercatcher.mp3");
    birdSoundCatalog[10].load("sounds/whio.mp3");
    birdSoundCatalog[11].load("sounds/seagull.mp3");
    birdSoundCatalog[12].load("sounds/fantail.mp3"); // Yeah Jenna says this is bullshit but for some reason array 0 does not work!!

   // birdSoundCatalog[0].load("sounds/fantail.mp3");

    
}

//--------------------------------------------------------------
void ofApp::loadNewBirdImage() {
    
    //rand = the type of bird we will generate.
    
    if(randBird == 1){
        int rand = int(ofRandom(birdCatalogSize));
        
        currentImageNumber = rand;//Change back to rand
    }
    else{
        if(currentImageNumber<birdCatalogSize)
        {
            currentImageNumber++;
        }
        else{
            currentImageNumber=0;
        }
    }
    switch (currentImageNumber) {
            
        case 0: // Alligator
            currentBirdImage=birdCatalog[0];
            currentBirdSound=birdSoundCatalog[12];
            break;
        case 1: // Bear
            currentBirdImage=birdCatalog[1];
            currentBirdSound=birdSoundCatalog[1];
            break;
        case 2: // Coyote
            currentBirdImage=birdCatalog[2];
            currentBirdSound=birdSoundCatalog[2];
            break;
        case 3: // Deer
            currentBirdImage=birdCatalog[3];
            currentBirdSound=birdSoundCatalog[3];
            break;
        case 4: // Jaguar
            currentBirdImage=birdCatalog[4];
            currentBirdSound=birdSoundCatalog[4];
            break;
        case 5: // Orangutan
            currentBirdImage=birdCatalog[5];
            currentBirdSound=birdSoundCatalog[5];
            break;
        case 6: // Peacock
            currentBirdImage=birdCatalog[6];
            currentBirdSound=birdSoundCatalog[6];
            break;
        case 7: // Pig
            currentBirdImage=birdCatalog[7];
            currentBirdSound=birdSoundCatalog[7];
            break;
        case 8: // Squirrel
            currentBirdImage=birdCatalog[8];
            currentBirdSound=birdSoundCatalog[8];
            break;
        case 9: // Toucan
            currentBirdImage=birdCatalog[9];
            currentBirdSound=birdSoundCatalog[9];
            break;
            
        case 10: // Fantail
            currentBirdImage=birdCatalog[10];
            currentBirdSound=birdSoundCatalog[10];
            break;
        case 11: // Dotterel
            currentBirdImage=birdCatalog[11];
            currentBirdSound=birdSoundCatalog[11];
            break;
        default:
            currentBirdImage=birdCatalog[1];
            currentBirdSound=birdSoundCatalog[1];
    }
    
    //originalBirdImage = currentBirdImage; //This makes sure we have the original size if we need to resize;
    
    
        // YOU NEED TO ADD HERE WHERE THE IMAGE GETS BIGGER
        //currentBirdImage = originalBirdImage;
        currentBirdImage.resize(currentBirdImage.getWidth()*silhoetteAdjust, currentBirdImage.getHeight()*silhoetteAdjust);
        resizedBirdImage.allocate(dimWidth, dimHeight, OF_IMAGE_COLOR_ALPHA);
    
    int beginHereX = resizedBirdImage.getWidth()/2 - currentBirdImage.getWidth()/2;
    int endHereX = resizedBirdImage.getWidth()/2 + currentBirdImage.getWidth()/2;
    int beginhereY = resizedBirdImage.getHeight() - currentBirdImage.getHeight();
    for(int x = 1; x < resizedBirdImage.getWidth(); x++){
        for(int y = 1; y < resizedBirdImage.getHeight(); y++){// Took out top thrid because it is blocked by roof
            if(x > beginHereX &&
               x < endHereX &&
               y > beginhereY){ // This puts the resized image in the center bottom
                
                
                ofColor birdCol = currentBirdImage.getColor(x - beginHereX, y - beginhereY);// This is sourcing the image colour at a certain location
                resizedBirdImage.setColor(x, y, birdCol);
            }
            else{
                ofColor transCol = ofColor(255, 255, 255, 0); // transparency
                resizedBirdImage.setColor(x, y, transCol);
            }
        }
    }
    resizedBirdImage.update();
    currentBirdImage = resizedBirdImage;
    
}

void ofApp::displayTimeIndicator() {
    
    //sides of screen
    int timeHeight = ofMap(triggerTimer,0,absolutePlayTime,0,dimHeight + 20);
    ofSetColor(50, 50, 50, 80);
    
    //ofRect(0,0,timeWidth,dimHeight-timeHeight);
    ofDrawRectangle(0, 0, dimWidth, dimHeight - timeHeight);
    ofSetColor(255, 255, 255, 255);
}

void ofApp::setupColorPalette() {
    
    //MmmMmManual Settings!
    
    //pink maroon
    colorPalette[0] = ofColor(152,44,84);
    colorPaletteDepth[0] = ofColor(224,126,186);
    
    // dark blue
    colorPalette[1] = ofColor(64,50,170);
    colorPaletteDepth[1] = ofColor(21,108,253);
    
    // blue green
    colorPalette[2] = ofColor(11,151,153);
    colorPaletteDepth[2] = ofColor(105,198,199);
    
    //green
    colorPalette[3] = ofColor(7,167,90);
    colorPaletteDepth[3] = ofColor(67,255,186);
    
    //yellow
    colorPalette[4] = ofColor(240,198,28);
    colorPaletteDepth[4] = ofColor(252,236,63);
    
    //orange
    colorPalette[5] = ofColor(244,103,0);
    colorPaletteDepth[5] = ofColor(216,180,152);
    
    //red
    colorPalette[6] = ofColor(255,47,47);
    colorPaletteDepth[6] = ofColor(252,167,167);
    
    //purple
    colorPalette[7] = ofColor(150,100,252);
    colorPaletteDepth[7] = ofColor(202,181,244);
    
    //blue
    colorPalette[8] = ofColor(49,82,110);
    colorPaletteDepth[8] = ofColor(49,186,237);
    
    // pinker
    colorPalette[9] = ofColor(255,34,133);
    colorPaletteDepth[9] = ofColor(244,168,255);
    
}

//--------------------------------------------------------------
void ofApp::loadBirdText() {
    
    std::ostringstream localOSS;
    std::string fileName;
    localOSS << "birdnames.txt";
    fileName = localOSS.str();
    
    ofBuffer buffer = ofBufferFromFile(fileName);
    
    int count = 0;
    
    if(buffer.size()) {
        
        // we now keep grabbing the next line
        // until we reach the end of the file
        while(buffer.isLastLine() == false) {
            
            // move on to the next line
            std::string line = buffer.getNextLine();
            
            // seussLines.push_back(line);
            birdName[count] = line;
            cout << birdName[count] << endl;
            count++;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == 'd') {
        debugMode = !debugMode;
        
        if(gui1 != NULL) {
            gui1->toggleVisible();
        }
        
        if (debugMode==true) {
            if (gui1==NULL) {
                //once only.
                launchGUI();
            }
            ofShowCursor();
        } else {
            ofHideCursor();
        }
    }
}


//--------------------------------------------------------------
void ofApp::setupGrid(){
    
    boxW = dimWidth/numCols;
    boxH = dimHeight/numRows;
    
    //Set up grid and objects
    int num = 0;
    
    //images
    ofImage imgShadow;
    ofImage imgColour;
    
    //Grid:
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {
            
            //Set colour
            colourBoxArray[num]= ofColor(ofRandom(100,254),ofRandom(50),ofRandom(50,154));
            
            //Set x and y
            int tX = boxW*j;
            int tY = boxH*i;
            
            //Retrieve images from file
            string pngName;
            
            int imgNum=0;
            if (num<(numRows*numCols)) {
                imgNum = num;
            } else {
                imgNum = ofRandom(0,5);
            }
            
            //dummy pix, are shown at start
            imgShadow = birdCatalog[num];
            imgColour = birdCatalog[num];
            imgShadow.setColor(0,0,0);
            imgShadow.resize(boxW,boxH);
            imgColour.resize(boxW,boxH);
            //Create Bird Box object
            BirdBox *b = new BirdBox(tX, tY, boxW, boxH, num, imgShadow, imgColour);
            
            birdBoxes.push_back(b);
            
            //next bird
            num++;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(button == 2){randomColDepth = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //   if(button == 0){state = Playing;}
    //   if(button == 1){state = Passive;}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
