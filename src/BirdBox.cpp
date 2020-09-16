//
//  BirdBox.cpp
//  
//
//  Created by Jenna Gavin on 17/02/15.
//
//

#include "BirdBox.h"

BirdBox::BirdBox(int tX, int tY, int tW, int tH, int myID, ofImage myPic, ofImage myColourPic) {
    
    iID = myID;
    
    pic = myPic;
    picBird = myColourPic;
    
    x = tX;
    y = tY;
    
    
    
    myBGColour = ofColor(ofRandom(50,255),50,ofRandom(100,200));
    
    picSwitchTime = ofRandom(minTime,maxTime);
    
    flipboard.load("sounds/page.mp3");
    flip = false;
    wave = 1;
}

void BirdBox::update(Boolean flipChange) {
    // cout << "FLIP " << flip <<  endl;
    if (flip ==false && picSwitchTime < ofGetElapsedTimeMillis() - picSwitchTimeElapsed ) {
       
        pictureSwitch=!pictureSwitch;
        
        flip = true;
       
        flipboard.play();

          //flip = !flip;

    }
    
    
    if (flipChange){
      //  flip = !flip;
       // flipboard.play();
        //cout << "flip "<< endl;
    }
}


void BirdBox::display() {
    
    //ofPushMatrix();
    //ofEnableAlphaBlending();

    //Draw Rect
    ofSetColor(myBGColour);
    int border = 2;
    //ofRect(x+border,y+border,w-border*2,h-border*2);
    
    //Draw Bird
    ofImage drawPic;
    
   // ofSetColor(255,255,255);
    if (pictureSwitch) {
        drawPic = pic;
    } else {
        drawPic = pic;
    }
    
    //drawPic.draw(x+(w*0.5)-((pic.width*picScale)*0.5),y+20,picBird.width*picScale, picBird.height*picScale);
    
    flipImage();
    
   // ofDisableAlphaBlending();
    //ofPopMatrix();
}

void BirdBox::setNewImages(ofImage birdShape, ofImage realBird) {
    pic = realBird;
    
    picBirdShape = birdShape;
    
    picSwitchTime = ofRandom(minTime,maxTime);
    picSwitchTimeElapsed = ofGetElapsedTimeMillis();
    flip = false;
}

void BirdBox::setTimer() {
    //flip = false;
}


void BirdBox::flipImage() {
    
 
    // float wave = sin(ofGetElapsedTimef());
    const float friction = 1.45;
    
    if(flip == true){
        wave *= friction;
        if(wave >= 181){
            wave = 181;
        }
    }
    if(flip == false){
        wave = 1;
        //wave /= friction;
        //if(wave <= 1){
         //   wave = 1;
        //}
    }
    
    
    // Well there it is... all the imagery in the boxes ... ....
    
    ofPushMatrix();
    ofTranslate(picBird.getWidth()/2, pic.getHeight()/2+pic.getHeight()/4);
    
    ofSetColor(0,0,0);
    ofDrawRectangle(x-pic.getWidth()/2,y-pic.getHeight()/4, pic.getWidth(), pic.getHeight()/2);
    ofSetColor(255,255,255);
    
    pic.drawSubsection(x-pic.getWidth()/2,y-pic.getHeight()/4, pic.getWidth(), pic.getHeight()/2, 0 , pic.getHeight()/2); /// This is the bottom first
   
    
    
    //Background
     picBirdShape.drawSubsection(x-picBird.getWidth()/2,y-picBird.getHeight()/2-picBird.getHeight()/4, picBird.getWidth(), picBird.getHeight()/2, 0, 0);
    ofSetColor(255,255,255);
    

    ofPushMatrix();
    
    ofTranslate(x-picBird.getWidth()/2, y-picBird.getHeight()/4);
    ofRotate(181 - wave, 1.0, 0, 0);
    
    picBirdShape.drawSubsection(0, 0, picBird.getWidth(), picBird.getHeight()/2, 0, picBird.getHeight()/2); // bottom second
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(x-pic.getWidth()/2, y-pic.getHeight()/4);
    ofRotate(1 - wave, 1.0, 0, 0);
    
    ofSetColor(255, 255, 255);
    if(wave <= 90){ // This stops it being drawn after the halfway mark
            ofSetColor(255,255,255);
        pic.drawSubsection(0, -pic.getHeight()/2, pic.getWidth(), pic.getHeight()/2, 0, 0); // This is the top first
        
    }
    ofPopMatrix();
    
    ofPopMatrix();
}


void BirdBox::updateDimensions(int tX, int tY, int tW, int tH) {
    
    x = tX;
    y = tY;
    
   
    
    pic.resize(tW,tH);
    picBird.resize(tW,tH);
    
}
