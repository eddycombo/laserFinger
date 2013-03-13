/*
* ofCvBlobTracker.h
* by stefanix
* Thanks to touchlib for the best fit algorithm!
*
* This class tracks blobs between frames.
* Most importantly it assignes persistent id to new blobs, correlates
* them between frames and removes them as blobs dissappear. It also
* compensates for ghost frames in which blobs momentarily dissappear.
*
* Based on the tracking it fires events when blobs come into existence,
* move around, and disappear. The object which receives the callbacks
* can be specified with setListener().
*
*/


#ifndef OF_CV_BLOBTRACKER_H
#define OF_CV_BLOBTRACKER_H

#include <map>
#include <vector>
#include "ofMain.h"
#include "ofxCvMain.h"
#include "ofxCvTrackedBlob.h"


class ofxCvBlobTracker {


  public:

    vector<ofxCvTrackedBlob>  blobs;


    ofxCvBlobTracker();
    //void setListener( ofxCvBlobListener* _listener );
    void trackBlobs( const vector<ofxCvBlob>& blobs );
    int findOrder( int id );  // order by which the present
                              // blobs came into existence
    ofxCvTrackedBlob& getById( int id );  // returns a reference to the
                                         // corresponding blob in blobs vector
    void draw( float x, float y );  // draws all blobs

    int reject_distance_threshold;
    int ghost_frames;
    float minimumDisplacementThreshold;

  protected:

    int currentID;
    int extraIDs;
    int numcheck;

    //ofCvBlobListener* listener;



    vector<vector<int> > matrix;
    vector<int> ids;
    vector<vector<ofxCvTrackedBlob> > history;


    void doBlobOn( const ofxCvTrackedBlob& b );
    void doBlobMoved( const ofxCvTrackedBlob& b );
    void doBlobOff( const ofxCvTrackedBlob& b );

    inline void permute( int k );
    inline bool checkValid( int start );
    inline bool checkValidNew( int start );
};



#endif

