#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat frame, frame_eq;
  Mat hist;
  Mat hist_img, hist_img_eq;
  VideoCapture cap;
  uint8_t histEq[256];

  int histW, histH;
  int histsize = 256;
  int sum;
  float range[] = {0, 256};
  const float *histrange = { range };

  cap.open(0);

  if(!cap.isOpened()){
    cout << "cameras indisponiveis\n";
    return -1;
  }

  std::cout << "Pressione qualquer tecla para encerrar o programa." << '\n';
  while(1){
    cap >> frame;

    histW = histsize;
    histH = histsize/4;

    hist_img    = Mat::zeros(histH, histW, CV_8UC1);
    hist_img_eq = Mat::zeros(histH, histW, CV_8UC1);

    cvtColor(frame, frame, CV_BGR2GRAY);

    //Calcula e desenha o histograma original
    calcHist(&frame, 1, 0, Mat(), hist, 1, &histsize, &histrange);
    for (int i = 0; i < histsize; i++) {
			line(hist_img, Point(i, histH), Point(i, cvRound(hist.at<float>(i))), Scalar(255), 1, 8, 0);
		}

    //calculo do histograma acumulado
    //e normalizacao do histograma acumulado
    sum = 0;
    for(int i = 0; i < histsize; i++)
    {
      sum+= hist.at<float>(i);
      histEq[i] = sum*255.0/frame.total();
    }

    frame_eq = frame.clone();
    //substituicao dos valores dos pixels
    for(int i = 0; i < frame.size().height; i++)
      for(int j = 0; j < frame.size().width; j++)
        frame_eq.at<uint8_t>(i,j) = histEq[frame.at<uint8_t>(i,j)];

    //Calcula e desenha o histograma equalizado
    calcHist(&frame_eq, 1, 0, Mat(), hist, 1, &histsize, &histrange);
    for (int i = 0; i<histsize; i++)
			line(hist_img_eq, Point(i, histH), Point(i, cvRound(hist.at<float>(i))), Scalar(255), 1, 8, 0);

    hist_img.copyTo( frame(Rect(10, 10, histW, histH)) );
    hist_img_eq.copyTo( frame_eq(Rect(10, 10, histW, histH)) );

    imshow("Original", frame);
    imshow("Equalizado", frame_eq);
    // imwrite("resultados/equalize_output.png", frame);

    if(waitKey(30) != 255)break;
  }

  return 0;
}
