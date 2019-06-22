#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <system_tools.h>

using namespace cv;
using namespace std;

#define NUM_COLORS 3


struct RANGE{
  RANGE( ){
      inf = cv::Scalar(179, 255,255);
      sup = cv::Scalar(179, 255,255);
  }

  RANGE( cv::Scalar sInf, cv::Scalar sSup){
      inf = sInf;
      sup = sSup;
  }
  cv::Scalar inf;
  cv::Scalar sup;
};

static const cv::Vec3b PxLARANJA(0,127,255);
static const cv::Vec3b PxAZUL(255, 0, 0);
static const cv::Vec3b PxAMARELO(0,255,255);
static const cv::Vec3b PxVERDE(0,255,0);
static const cv::Vec3b PxMAGENTA(255, 0, 255);
static const cv::Vec3b PxCIANO(255,255, 0);
static const cv::Vec3b PxPRETO(0,0,0);
static const cv::Vec3b PxBRANCO(255,255,255);

static const cv::Vec3b COLORS[] = { PxLARANJA, PxAZUL, PxAMARELO, PxVERDE, PxMAGENTA, PxCIANO};

static const RANGE AZUL_range = RANGE( cv::Scalar(91, 233, 77),  cv::Scalar(165, 255, 200));
static const RANGE LARANJA_range = RANGE( cv::Scalar(0, 148, 61),  cv::Scalar(25, 255, 255));
static const RANGE AMARELO_range = RANGE( cv::Scalar(91, 233, 77),  cv::Scalar(165, 255, 200));

static  RANGE COLORS_RANGE[NUM_COLORS];

void readFile(){
  std::ifstream in("../calibracao.cal");
  int colorIndex;
  int Hmin,Smin,Vmin;
  int Hmax,Smax,Vmax;

  if(!in.is_open()){
    std::cerr << "Erro abrir arquivo de calibracao" << '\n';
    exit(1);
  }

  for(int c = 0; c < NUM_COLORS; c++)
  {
    in >> Hmin;
    in >> Smin;
    in >> Vmin;

    in >> Hmax;
    in >> Smax;
    in >> Vmax;

    COLORS_RANGE[c] = RANGE(cv::Scalar(Hmin, Smin, Vmin), cv::Scalar(Hmax, Smax, Vmax));
  }
  in.close();
};

 int main( int argc, char** argv )
 {
    VideoCapture cap(argc > 1 ? atoi(argv[1]) : 0);

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }
    readFile();//carrega os limires de HSV estabelecidos no programa de calibracação

  Mat segment;
  double dt_process;
  double dt_BGR2HSV;

    while (true)
    {
      Mat imgOriginal;

       if (!cap.read(imgOriginal))
      {
           cout << "Erro na captura!" << endl;
           break;
      }

    dt_BGR2HSV = tools::clock();
    Mat imgHSV;
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
    dt_BGR2HSV = tools::clock() - dt_BGR2HSV;

    dt_process = tools::clock();
    for(uint color = 0; color < NUM_COLORS; color++){
      Mat imgThresholded;

      inRange(imgHSV, COLORS_RANGE[color].inf, COLORS_RANGE[color].sup, imgThresholded); //Threshold
      //operacao morfologia de abertura, para remover ruidos de formato
      erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
      dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

      //filtro morfologico, fechamento (para preencher espacos vazios dos objetos)
      dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
      erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

      if(color == 0)segment = imgThresholded.clone();
      else segment = segment + imgThresholded;

      //calcula dos momentos da imagem segmentada
      Moments oMoments = moments(imgThresholded);

      //uso dos momentos espacais para o calculo do centroide
      double dM01 = oMoments.m01; //momento em torno de x
      double dM10 = oMoments.m10; //momento em torno de y
      double dArea = oMoments.m00;//area
      dt_process = tools::clock() - dt_process;

      imshow(to_string(color),  imgThresholded);
      //considera apenas regioes com mais de 1000 pixels
      if (dArea > 1000)
      {
        //Calculo do Centro da regiao
        int posX = dM10 / dArea;
        int posY = dM01 / dArea;

        circle(imgOriginal,
          cv::Point(posX, posY),
          6,
          COLORS[color],  //realiza pequenas mudanças nas tonalidades originais, para dar efeito de erro humano
          -1,
          CV_AA);
        }
      }

    imshow("Thresholded Image", segment); //show the thresholded image
    imshow("Original", imgOriginal); //show the original image

    std::cout << "Tempo de Processsamento Total:" << dt_process + dt_BGR2HSV << 's' <<'\n';

    if (waitKey(30) == 27)//para sair com a tecla  ESC
    {
        cout << "Programa Finalizado" << endl;
        break;
    }
  }
     return 0;
  }
