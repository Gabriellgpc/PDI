#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat frame, grayFrame;
  Mat H1, H2;
  VideoCapture cap;
  double r_correl = 0;

  int histsize = 256;
  float range[] = {0, 256};
  const float *histrange = { range };

  cap.open(0);

  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }

  cap >> frame; //captura um frame
  cvtColor(frame, frame, CV_BGR2GRAY); //converte para escala de cinza
  calcHist(&frame, 1, 0, Mat(), H1, 1, &histsize, &histrange);

  while(true){
    H2 = H1.clone();
    cap >> frame; //captura um frame
    cvtColor(frame, grayFrame, CV_BGR2GRAY); //converte para escala de cinza
    calcHist(&grayFrame, 1, 0, Mat(), H1, 1, &histsize, &histrange);

    r_correl =  compareHist(H1, H2, CV_COMP_CORREL);

    if(r_correl <= 0.9989)//movimento
    {
      circle(frame, Point(frame.cols - 20, 20), 10, Scalar(0, 255, 0), CV_FILLED);
    }else{
      circle(frame, Point(frame.cols - 20, 20), 10, Scalar(255, 255 ,255), CV_FILLED);
    }

    imshow("Live", frame);
    if(waitKey(30) != 255)break;
  }

  return 0;
}
