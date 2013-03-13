#include "ofxCvBlobTracker.h"

static ofRectangle maxLasersLocation(vector <ofRectangle> &choicesLocations, vector <ofxCvTrackedBlob> &onScreenLasers){

    int tmpCandidates[choicesLocations.size()];


    ///check where's everybody.
    for(int i = 0; i < onScreenLasers.size(); i++){

            for(int j = 0; j < choicesLocations.size(); j++){

                    if(choicesLocations[j].inside(onScreenLasers[i].centroid)){

                        tmpCandidates[j]++;

                    };
            };
    };

    /// get me the fucker !!!
    int tmpMaxIndex = 0;

    for (int i=1; i<sizeof(tmpCandidates)/sizeof(tmpCandidates[0]); i++) {
        if (tmpCandidates[i] > tmpCandidates[tmpMaxIndex]) {
            tmpMaxIndex = i;
        }
    }
    return choicesLocations[tmpMaxIndex];
};


static ofRectangle oneAtRandomChooser(vector <ofRectangle> &choicesLocations, ofxCvTrackedBlob &randomChooser){

        ///check where's the fucker.


            for(int j = 0; j < choicesLocations.size(); j++){

                    if(choicesLocations[j].inside(randomChooser.centroid)){

                        return choicesLocations[j];

                    };
            };
};
