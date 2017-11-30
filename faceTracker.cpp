//
// Created by hsw on 17-11-27.
//

#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

#include <dirent.h>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

    if (argc > 5) return -1;

    bool HOG = true;
    bool FIXEDWINDOW = false;
    bool MULTISCALE = true;
    bool SILENT = false;
    bool LAB = false;
    bool track=false;
    for(int i = 0; i < argc; i++){
        if ( strcmp (argv[i], "hog") == 0 )
            HOG = true;
        if ( strcmp (argv[i], "fixed_window") == 0 )
            FIXEDWINDOW = true;
        if ( strcmp (argv[i], "singlescale") == 0 )
            MULTISCALE = false;
        if ( strcmp (argv[i], "show") == 0 )
            SILENT = false;
        if ( strcmp (argv[i], "lab") == 0 ){
            LAB = true;
            HOG = true;
        }
        if ( strcmp (argv[i], "gray") == 0 )
            HOG = false;
    }

    // Create KCFTracker object
    KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

    // Frame readed
    Mat frame;

    // Tracker results
    Rect result;

    //Read video
    VideoCapture cap("/home/hsw/faceTracker/1.avi");

    // Frame counter
    int nFrames = 0;
    double apceValue = 0;
    double resMax =0;
    vector<double> preApce;
    vector<double> preResMax;


    while (cap.read(frame)){

        // First frame, give the groundtruth to the tracker
        if (nFrames == 0) {
            
	        Rect roi = selectROI("Image",frame,false,false);
            tracker.init(roi,frame);
            rectangle(frame , Point(roi.x , roi.y) , Point(roi.x + roi.width , roi.y + roi.height),Scalar(0,0,255),2);
            
 	    }// Update
        else{

            result = tracker.update(frame ,apceValue,resMax);
            rectangle( frame, Point(result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 2, 8 );

            //preApce.push_back(apceValue);
            //preResMax.push_back(resMax);
            double addApce = 0;
            double addResMax = 0;
            double comApce = 0;
            double comResMax = 0;
            cout<<"apceValue = "<<apceValue<<endl;
            cout<<"resMax = "<<resMax<<endl;
            preApce.push_back(apceValue);
            preResMax.push_back(resMax);
            //cout<<"preResMax.size() = "<<preResMax.size()<<endl;

        /*    for (int j = 0; j < sz; j++) {

                *//* cout<<preApce[j]<<endl;
                       cout<<preResMax[j]<<endl;*//*
                    addApce += preApce[j];
                    addResMax += preResMax[j];
            }*/

            //addApce = accumulate(preApce.begin(), preApce.end(),0);
            //addResMax = accumulate(preResMax.begin(), preResMax.end(),0);
            int sz =  preApce.size()-1;
            for(int i=0;i<sz;i++){

                addApce +=preApce[i];
                addResMax +=preResMax[i];
            }
            //cout<<"addResMax = "<<addResMax<<endl;


            if(sz>0) {

                addApce = addApce / sz;
                addResMax = addResMax / sz;

                //cout<<"sz ="<<sz<<endl;
                comApce = 0.5 * addApce;
                comResMax = 0.5 * addResMax;
               // cout<<"comApce = "<<comApce<<endl;
                //cout<<"comResMax = "<<comResMax<<endl;

                if (apceValue > comApce && resMax > comResMax) {

                    std::cout << "tracking was successed!" << std::endl;
                    track = true;

                } else {

                    std::cout << "tracking was failed!" << std::endl;
                    track = false;
                }

            }

	    }

        nFrames++;

        if (!SILENT){
            imshow("Image", frame);
            waitKey(100);
        }
    }
  

}
