/*

l'idée est d'avoir une appli de tracking simple : track + OSC
à laquelle on puisse ajouter via des objet voteMode est actionMode les fonction de vote et/ou click .
*/




#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){


    movies[0] = "linea";
    movies[1] = "deliver";

    wichMovie  = 0;
    //OF related
    ofSetVerticalSync(true);
	ofEnableSmoothing();


	//initailize vars

	//image realated

	settingUp = true;
    treatImg = 1;
    doHSV = true;
    doBlur = false;
    dilateFirst = true;
	doInv = false;
    wichImg = 2;
    doTrack = false;

    chan1Min =22.0;
	chan1Max = 255.0;

	chan2Min = 0;
	chan2Max = 106.0;

	chan3Min = 0;
	chan3Max = 255.0;

    dilateNtimes = 6;
    blurNtimes = 1;
    minBlobArea = 1;
    maxBlobArea = (camW*camH)*0.5;
    maxPlayers = 10;
    //warp related
    xVidPos  = 0;
    yVidPos = 0;
    wichPoint = 0;


    //camera related
	vidGrabber.setDeviceID(1);
    vidGrabber.initGrabber(camW, camH);


	//OSC realted
	sender.setup( HOST, PORT );
    sendOsc= false;


    //GUI realted
    gui = new ofxUICanvas(20, 540, ofGetWidth(), 200);
    gui->loadSettings("settings.xml");

    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);


	gui->addWidgetDown(new ofxUIRangeSlider(255,10, 0.0, 255.0, chan1Min, chan1Max, "CHAN1"));
    gui->addWidgetRight(new ofxUISlider(255,10, 1.0, 110.0,  maxPlayers, "PLAYERS"));


    gui->addWidgetDown(new ofxUIRangeSlider(255,10, 0.0, 255.0, chan2Min, chan2Max, "CHAN2"));
    gui->addWidgetRight(new ofxUIRangeSlider(255,10, 1,  1500, minBlobArea, maxBlobArea, "AREA"));
    gui->addWidgetDown(new ofxUIRangeSlider(255,10, 0.0, 255.0, chan3Min, chan3Max, "CHAN3"));
    gui->addWidgetRight(new ofxUISlider(255,10, 2, 10, 2, "GHOST"));
    gui->addWidgetDown(new ofxUISlider(255,10, 1, 50, blurNtimes, "BLR"));
    gui->addWidgetRight(new ofxUISlider(255,10, 2, 100, 2, "DISPL"));
    gui->addWidgetDown(new ofxUISlider(255,10, 1, 50, dilateNtimes, "DILT"));


    chanName[0] = "HUE :";
    chanName[1] = "SAT :";
    chanName[2] = "VAL :";


    //init gui + keystone

    gui->loadSettings("settings.xml");

    if( XML.loadFile("corners.xml") ){
                        cout<<"loaded"<<endl;
            }else{cout<<"unable to load"<<endl;
            }


    //image allocate
    colorImg.allocate(camW, camH);					// Live Cam as input source
    colorImgTrans.allocate(camW, camH);

    channels[0].allocate(camW, camH);					// Hue Image
    channels[1].allocate(camW, camH);					// Saturation Image
    channels[2].allocate(camW, camH);					// value Image

    ttImg.allocate(camW, camH);
    warpedImg.allocate(camW, camH);
    trackedTexture.allocate(camW, camH, OF_IMAGE_GRAYSCALE);

    colorTrackedPixels = new unsigned char [camW * camH];	// Tracked Image


    //keyStone
    srcPositions  = new ofPoint[4];
	srcPositions[0].set(XML.getValue("SRC:X1", xVidPos), XML.getValue("SRC:Y1", yVidPos), 0);
	srcPositions[1].set(XML.getValue("SRC:X2", camW + xVidPos), XML.getValue("SRC:Y2", yVidPos), 0);
	srcPositions[2].set(XML.getValue("SRC:X3", camW + xVidPos), XML.getValue("SRC:Y3", camH + yVidPos), 0);
	srcPositions[3].set(XML.getValue("SRC:X4", xVidPos), XML.getValue("SRC:Y4", camH + yVidPos), 0);

	dstPositions  = new ofPoint[4];
	dstPositions[0].set(xVidPos, yVidPos, 0);
	dstPositions[1].set(camW + xVidPos, yVidPos, 0);
	dstPositions[2].set(camW + xVidPos, camH + yVidPos, 0);
	dstPositions[3].set(xVidPos, camH + yVidPos, 0);

    //clicked = false;

    loadMovieGuiSettings(wichMovie);

}

//--------------------------------------------------------------
void testApp::update(){

	ofBackground(0);

	vidGrabber.grabFrame();

	if (vidGrabber.isFrameNew()){


        colorImg.setFromPixels(vidGrabber.getPixels(), vidGrabber.width, vidGrabber.height);

        colorImgTrans = colorImg;

		if(doHSV){
            colorImgTrans.convertRgbToHsv();
		}

		colorImgTrans.convertToGrayscalePlanarImages(channels[0], channels[1], channels[2]);

        for(int i = 0; i<3;i++){
            channels[i].flagImageChanged();
        }

        // Perform tracking calculations
		unsigned char * chan1Pixels = channels[0].getPixels();
		unsigned char * chan2Pixels = channels[1].getPixels();
		unsigned char * chan3Pixels = channels[2].getPixels();

		int nPixels = vidGrabber.width * vidGrabber.height;

        switch(treatImg){

            case 4:

                for (int i = 0; i < nPixels; i++){
			if ((chan1Pixels[i]>=chan1Min && chan1Pixels[i]<= chan1Max)&&(chan2Pixels[i]>=chan2Min&&chan2Pixels[i]<=chan2Max)&&(chan3Pixels[i]>=chan3Min && chan3Pixels[i]<=chan3Max)){
				colorTrackedPixels[i] = 255;
			} else {
				colorTrackedPixels[i] = 0;
			}
		}
            break;

            case 1:

            for (int i = 0; i < nPixels; i++){
			if (chan1Pixels[i]>=chan1Min && chan1Pixels[i]<= chan1Max){
				colorTrackedPixels[i] = 255;
			} else {
				colorTrackedPixels[i] = 0;
			}
		}
            break;

            case 2:
                for (int i = 0; i < nPixels; i++){
			if (chan2Pixels[i]>=chan2Min && chan2Pixels[i]<= chan2Max){
				colorTrackedPixels[i] = 255;
			} else {
				colorTrackedPixels[i] = 0;
			}
		}
            break;

            case 3:

            for (int i = 0; i < nPixels; i++){
			if (chan3Pixels[i]>=chan3Min && chan3Pixels[i]<= chan3Max){
				colorTrackedPixels[i] = 255;
			} else {
				colorTrackedPixels[i] = 0;
			}
		}
            break;
    }


        trackedTexture.setFromPixels(colorTrackedPixels, vidGrabber.width, vidGrabber.height, OF_IMAGE_GRAYSCALE);

        ttImg.setFromPixels(trackedTexture.getPixels(), vidGrabber.width, vidGrabber.height);

        warpedImg.warpIntoMe(ttImg, srcPositions, dstPositions);

        if(dilateFirst){

                for(int i = 0; i<dilateNtimes;i++){
                warpedImg.dilate_3x3();

            }

            if(doBlur){
                for(int i = 0; i<blurNtimes; i++){
                    warpedImg.blur();

                }
            }

        }else{

                if(doBlur){
                    for(int i = 0; i<blurNtimes; i++){
                        warpedImg.blur();

                    }
                }

                for(int i = 0; i<dilateNtimes;i++){
                warpedImg.dilate_3x3();

            }
        }

        if(doInv){
            warpedImg.invert();

        }

        if(!settingUp){
            blobFinder.findContours(warpedImg, minBlobArea, maxBlobArea, maxPlayers, false);

            if(doTrack && blobFinder.nBlobs > 0){

                tracker.ghost_frames = ghost;
                tracker.trackBlobs(blobFinder.blobs);


                if(resultTime){

                    //winner = maxLasersLocation(&buttons, &tracker.blobs);

                }





        }else{
            blobFinder.findContours(ttImg, minBlobArea, maxBlobArea, maxPlayers, false);
            }
        }
    }
}
//--------------------------------------------------------------
void testApp::draw(){

	ofSetHexColor(0xffffff);

    if (!settingUp){

        ofDrawBitmapString("Tracked", 680, 20);


            //ofDrawBitmapString(ofToString(clicked), 680, 40);

        warpedImg.draw(0,0, 640,480);

        ofFill();
        blobFinder.draw(0, 0, 640, 480);
        tracker.draw(0, 0);
        ofNoFill();
        ofSetColor(255);
        for(int i = 0; i<buttons.size(); i++){

            ofRect(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height);
        }
        gui->disable();

    }else{
    gui->enable();
    ofDrawBitmapString("Tracked:", 20, 20);
	ttImg.draw(0, 0, 640, 480);  // was 25,25,640,480

    ofDrawBitmapString("warped Image", 680, 20);
    warpedImg.draw(685, 25, 320, 240);

    ofFill();

    // Draw keyStone
	ofSetColor(255);
	ofNoFill();
	ofBeginShape();
	for (int j = 0; j < 4; j++){
		ofVertex(srcPositions[j].x, srcPositions[j].y);
	}
	ofEndShape(true);

	// Draw points
	ofFill();
	ofSetColor(255);
	for (int j = 0; j < 4; j++){
		ofCircle(srcPositions[j].x, srcPositions[j].y, 3);
	}

    ofSetColor(0, 255, 0);
    ofNoFill();

    ofDrawBitmapString(chanName[wichImg],  680, 280);
    ofSetColor(255, 255, 255);

    if(treatImg > 3){
        colorImgTrans.draw(685, 290, 320, 240);
    }else{

        channels[treatImg - 1].draw(685, 290, 320, 240);
    }

        ofDrawBitmapString("HSV: "+ofToString(doHSV)+" "+"TreatChannel: "+ofToString(treatImg)+" " + "BLUR :" + ofToString(doBlur)+" "+"Dilate1st: "+ofToString(dilateFirst)+" "+"Inv: "+ofToString(doInv), 25, 520);

    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


    switch(key){

        case('a'):
            treatImg = 1;
        break;
        case('z'):
            treatImg = 2;
        break;
        case('e'):
            treatImg = 3;
        break;
        case('r'):
            treatImg = 4;
        break;
        case('b'):
            doBlur = !doBlur;
        break;
        case('d'):
            dilateFirst = !dilateFirst;
        break;
        //case('h'):
            //doHSV = !doHSV;
        //break;
        case('i'):
            doInv = !doInv;
        break;

        case('q'):

                if(settingUp){
                    srcPositions[0].x = blobFinder.blobs[0].centroid.x;
                    srcPositions[0].y = blobFinder.blobs[0].centroid.y;
                }
                break;

        case('s'):
                if(settingUp){
                    srcPositions[1].x = blobFinder.blobs[0].centroid.x;
                    srcPositions[1].y = blobFinder.blobs[0].centroid.y;
                }
                break;

        case('w'):
               if(settingUp){
                    srcPositions[3].x = blobFinder.blobs[0].centroid.x;
                    srcPositions[3].y = blobFinder.blobs[0].centroid.y;
                }
                break;

        case('x'):
               if(settingUp){
                    srcPositions[2].x = blobFinder.blobs[0].centroid.x;
                    srcPositions[2].y = blobFinder.blobs[0].centroid.y;
                }
                break;
        case('m'):
            gui->saveSettings("settings.xml");
            XML.setValue("SRC:X1", srcPositions[0].x);

            XML.setValue("SRC:Y1", srcPositions[0].y);
            XML.setValue("SRC:X2", srcPositions[1].x);
            XML.setValue("SRC:Y2", srcPositions[1].y);
            XML.setValue("SRC:X3", srcPositions[2].x);
            XML.setValue("SRC:Y3", srcPositions[2].y);
            XML.setValue("SRC:X4", srcPositions[3].x);
            XML.setValue("SRC:Y4", srcPositions[3].y);

            XML.saveFile("corners.xml");
        break;
        case('l'):
            gui->loadSettings("settings.xml");
             if( XML.loadFile("corners.xml") ){
                        cout<<"loaded"<<endl;
            }
              else{
                        cout<<"unable to load"<<endl;
            }

        break;
        case('v'):
            srcPositions[0].set(xVidPos, yVidPos, 0);
            srcPositions[1].set(vidGrabber.width + xVidPos, yVidPos, 0);
            srcPositions[2].set(vidGrabber.width + xVidPos, vidGrabber.height + yVidPos, 0);
            srcPositions[3].set(xVidPos, vidGrabber.height + yVidPos, 0);
        break;
        case('k'):
            settingUp = !settingUp;
            doTrack = !doTrack;
        break;
    }
}



//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

void testApp::guiEvent(ofxUIEventArgs &e){
	string name = e.widget->getName();

    if(name == "CHAN1"){

        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan1Min = vslider->getScaledValueLow();
        chan1Max = vslider->getScaledValueHigh();

    }else if(name == "CHAN2"){

        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan2Min = vslider->getScaledValueLow();
        chan2Max = vslider->getScaledValueHigh();

    }else if(name == "CHAN3"){

        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan3Min = vslider->getScaledValueLow();
        chan3Max = vslider->getScaledValueHigh();

    }else if(name == "GHOST"){

        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ghost = (int)slider->getScaledValue();


    }
    else if(name == "DILT"){

        ofxUISlider *slider = (ofxUISlider *) e.widget;
        dilateNtimes = slider->getScaledValue();
    }
    else if(name == "BLR"){

        ofxUISlider *slider = (ofxUISlider *) e.widget;
        blurNtimes = slider->getScaledValue();
    }

    else if(name == "AREA"){

        ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        minBlobArea = slider->getScaledValueLow();
        maxBlobArea = slider->getScaledValueHigh();
    }
}

void testApp::loadMovieGuiSettings(int wich){

    ///everything movie + GUI related

    buttons.clear();

    if( movieGuiSettings.loadFile(movies[wich]+".xml") ){
                        cout<<"loaded linea settings"<<endl;
            }else{cout<<"unable to load linea settings"<<endl;
        }
    movieGuiSettings.pushTag("BUTTONS");
    int numButts = movieGuiSettings.getNumTags("BT");

    for(int i = 0; i<numButts; i++){

        int tmpX = movieGuiSettings.getValue("BT:X", 0, i);
        int tmpY = movieGuiSettings.getValue("BT:Y", 0, i);
        int tmpW = movieGuiSettings.getValue("BT:W", 0, i);
        int tmpH = movieGuiSettings.getValue("BT:H", 0, i);

        cout<<tmpX<<" "<<tmpY<<" "<<tmpW<<" "<<tmpH<<endl;

        buttons.push_back(ofRectangle(tmpX, tmpY, tmpW, tmpH));


    }

    cout<<"done loading movie settings"<<endl;
};





