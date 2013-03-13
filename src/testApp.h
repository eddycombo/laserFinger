#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "voteFunctions.h"
#include "ofxCvBlobTracker.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"


#define HOST "169.254.239.247"
#define PORT 12345

#define camW 640
#define camH 480
#define screenW 1024
#define screenH 768

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


        void loadMovieGuiSettings(int wich);

        ofTrueTypeFont          lcdFont;

        ofxXmlSettings          movieGuiSettings;
        ofxXmlSettings          XML;//to be changed by corners.xml


        ofRectangle             winner;

        string movies[3];
        int wichMovie;

        ofVideoPlayer           vidPlayer;
        ofVideoGrabber          vidGrabber;


        ofxCvColorImage		    colorImg;
        ofxCvColorImage		    colorImgTrans;


        ofxCvGrayscaleImage		chan1Img;
        ofxCvGrayscaleImage		chan2Img;
        ofxCvGrayscaleImage		chan3Img;


        ofxCvGrayscaleImage   channels[3];

        string                  chanName[3];
        ofxCvGrayscaleImage     ttImg, warpedImg;

        ///warp stuff
        ofRectangle             warpGui;
        ofPoint *				srcPositions;
        ofPoint *				dstPositions;
        int                     activePoint;
        int                     wichPoint, xVidPos, yVidPos;
        /////
        bool                    doHSV, doBlur, dilateFirst, doInv, settingUp, sendOsc, doTrack, clicked;

        unsigned char *         colorTrackedPixels;
        ofImage               trackedTexture;

        int wichImg, chan1Min, chan1Max, chan2Min, chan2Max, chan3Min, chan3Max, blurNtimes, dilateNtimes;
        float chan1, chan1Width, chan2, chan2Width, chan3, chan3Width, minDispl, ghost, minBlobArea, maxBlobArea;
        int treatImg, thresh;

        ofxUICanvas *gui;

        void guiEvent(ofxUIEventArgs &e);

        ofxCvContourFinder blobFinder;
        ofxCvBlobTracker  tracker;

        ofxOscSender sender;

        /////about those voting things
        vector <ofRectangle> buttons;
        int maxPlayers;
        int voteMode;

        ofxCvTrackedBlob randomPicked;
        bool isPicked;


};

