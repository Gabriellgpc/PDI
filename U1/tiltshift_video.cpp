#include <iostream>
#include <math.h>       /* tanh, log */
#include <opencv2/opencv.hpp>

using namespace cv;

bool tiltShift(Mat &image_src,
               Mat &image_dest,
               float delta_l, //0 to 1
               float decay,   //0 to 1
               float center,  //0 to 1
               float blurring);//0 to 1

int main(int argc, char** argv){
  VideoCapture video_src;
  Mat frame;

  video_src.open(argv[1]);
  if(video_src.isOpened() == false){
    std::cerr << "Erro ao carregar arquivo de video!" << '\n';
    return 1;
  }

  int frame_width = static_cast<int>(video_src.get(CAP_PROP_FRAME_WIDTH));
  int frame_height = static_cast<int>(video_src.get(CAP_PROP_FRAME_HEIGHT));
  Size frame_size(frame_width, frame_height);
  int frames_per_second = 8;
  int steps = 2;

  //Create and initialize the VideoWriter object
  VideoWriter video_result("resultados/tiltshift_video.avi", VideoWriter::fourcc('X','V','I','D'),
                                                             frames_per_second, frame_size, true);
  if(video_result.isOpened() == false){
   std::cerr << "Erro ao carregar arquivo de video!" << '\n';
   return 1;
  }

  std::cout << "Processando..." << '\n';

  while(true){
    video_src >> frame;
    if(frame.data == NULL)break;

    tiltShift(frame, frame, 0.2, 0.0001, 0.45, 0.1);
    video_result.write(frame);

    int i;
    for(i = 0; i < steps; i++)
    {
      if(frame.data == NULL)break; //final do arquivo
      video_src >> frame;
    }
    if(i != steps)break;
  }

  video_src.release();
  video_result.release();

  std::cout << "Terminou!" << '\n';

  system("vlc resultados/tiltshift_video.avi");

  return 0;
}


double alfa_func(int x, int l1, int l2, double d){
  return 0.5*( tanh((x-l1)/d) - tanh((x-l2)/d) );
}

bool tiltShift(Mat &image_src,
               Mat &image_dest,
               float delta_l, //0 to 1
               float decay,   //0 to 1
               float center,  //0 to 1
               float blurring)
{
  if(delta_l > 1 || center > 1 || decay > 1 || blurring > 1)return true;
  if(delta_l < 0 || center < 0 || decay < 0 || blurring < 0)return true;
  if(image_src.data == NULL)return true;

  const static int intensity_max = 100;
  Mat blurry_img;
  const int x_max = image_src.rows;
  Mat mask, image_32f;
  float weigth = 1/9.0;
  float average[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};


  mask = Mat(3, 3, CV_32F, average);
  scaleAdd(mask, weigth, Mat::zeros(3,3,CV_32F), mask);
  image_src.convertTo(image_32f, CV_32F);
  for(int i = 0; i < intensity_max*blurring; i++)
  {
    filter2D(image_32f, image_32f, image_32f.depth(), mask, Point(1,1));
    image_32f = abs(image_32f);
  }
  image_32f.convertTo(blurry_img, CV_8U);

  #define L1 (center - delta_l/2.0)*x_max
  #define L2 (center + delta_l/2.0)*x_max
  image_dest = image_src.clone();
  double alfa;
  for(int x = 0; x < x_max; x++)
  {
      alfa = alfa_func(x, L1, L2, decay);
      addWeighted(  image_src.row(x),
                    alfa,
                    blurry_img.row(x),
                    1-alfa,
                    0.0,
                    image_dest.row(x));
  }

  return false;
};
