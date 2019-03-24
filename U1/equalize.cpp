#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void plotHistogramInImage(Mat image, const Mat &histogram){
  //calcula histograma da image e posiciona no canto superior esquerda da propria image
  int histw = 256, histh = histw/4;
  Mat histImg(histh, histw, CV_8U, Scalar(0));
  //plot do histograma
  for(int i=0; i < histw; i++){
    line(histImg,
         Point(i, histh),
         Point(i, histh-cvRound(histogram.at<float>(i))),
         Scalar(255), 1, 8, 0);
  }

  histImg.copyTo(image(Rect(0, 0, histw, histh)));
}

int main(int argc, char** argv){
  Mat frame;
  Mat hist;
  VideoCapture cap;
  uint8_t histEq[256];

  int histsize = 256;
  int sum;
  float range[] = {0, 256};
  const float *histrange = { range };

  cap.open(0);

  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }

  std::cout << "Pressione qualquer tecla para encerrar o programa." << '\n';
  while(1){
    cap >> frame;

    cvtColor(frame, frame, CV_BGR2GRAY);
    imshow("Original", frame);

    calcHist(&frame, 1, 0, Mat(), hist, 1, &histsize, &histrange);

    sum = 0;
    for(int i = 0; i < histsize; i++)
    {
      sum+= hist.at<float>(i);
      histEq[i] = sum*255.0/frame.total();
    }

    for(int i = 0; i < frame.size().height; i++)
      for(int j = 0; j < frame.size().width; j++)
        frame.at<uint8_t>(i,j) = histEq[frame.at<uint8_t>(i,j)];
    imshow("Equalizado", frame);


    if(waitKey(30) != 255)break;
  }
  return 0;
}
