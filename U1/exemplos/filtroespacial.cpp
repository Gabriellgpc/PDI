#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


#define MAX_FILTER 2

void printmask(Mat &m){
  for(int i=0; i<m.size().height; i++){
    for(int j=0; j<m.size().width; j++){
      cout << m.at<float>(i,j) << ",";
    }
    cout << endl;
  }
}



void menu(){
  cout << "\npressione a tecla para ativar o filtro: \n"
	         "a - calcular modulo\n"
           "m - media\n"
           "g - gauss\n"
           "v - vertical\n"
	         "h - horizontal\n"
           "l - laplaciano\n"
           "i - identidade\n"
           "e - laplaciano do Gaussiano\n"
	         "esc - sair\n";
}

int main(int argvc, char** argv){
  VideoCapture video;
  float identidade[] = {0, 0, 0,
                        0, 1, 0,
                        0, 0, 0};
  float media[] = {1,1,1,
				           1,1,1,
				           1,1,1};
  float gauss[] = {1,2,1,
				           2,4,2,
				           1,2,1};
  float horizontal[]={-1,0,1,
					            -2,0,2,
					            -1,0,1};
  float vertical[]={-1,-2,-1,
					           0, 0, 0,
					           1, 2, 1};
  float laplacian[]={0,-1,0,
					          -1,4,-1,
					           0,-1,0};

  Mat cap, frame, frame32f, frameFiltered;
  Mat mask[] = { Mat(3,3, CV_32F), Mat(3,3, CV_32F) };
  Mat maskAux;
  Mat result;
  char* text = "Media";
  double width, height;
  int absolut;
  char key;

  video.open(0);
  if(!video.isOpened())
    return -1;
  width=video.get(CV_CAP_PROP_FRAME_WIDTH);
  height=video.get(CV_CAP_PROP_FRAME_HEIGHT);
  std::cout << "largura=" << width << "\n";;
  std::cout << "altura =" << height<< "\n";;

  namedWindow("filtroespacial",1);

  mask[0] = Mat(3, 3, CV_32F, media);
  scaleAdd(mask[0], 1/9.0, Mat::zeros(3,3,CV_32F), maskAux);
  mask[0] = maskAux.clone();
  absolut = 1; // calcs abs of the image
  mask[1] = Mat(3, 3, CV_32F, identidade);

  menu();
  for(;;){
    video >> cap;
    cvtColor(cap, frame, CV_BGR2GRAY);//converte a imagem para tons de cinza
    flip(frame, frame, 1); //espelha a imagem

    imshow("original", frame);//exibe a imagem

    frame.convertTo(frameFiltered, CV_32F); //cria uma imagem tipo float
    for(int i = 0; i < MAX_FILTER; i++)
    {
      filter2D(frameFiltered, frameFiltered, frameFiltered.depth(), mask[i], Point(1,1), 0); //aplica o filtro 1
      if(absolut)frameFiltered = abs(frameFiltered);
    }


    frameFiltered.convertTo(result, CV_8U); //converte a imagem filtrada de float para byte em tons de cinza
    putText(result, text, Point(5,50), FONT_HERSHEY_DUPLEX, 1, Scalar(255), 2);

    imshow("filtroespacial", result);//exibe a imagem filtrada


    key = (char) waitKey(10);
    if( key == 27 ) break; // esc pressed!
    switch(key){
    case 'a':
	  menu();
      absolut=!absolut;
      break;
    case 'm':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, media);
      scaleAdd(mask[0], 1/9.0, Mat::zeros(3,3,CV_32F), maskAux);
      mask[0] = maskAux.clone();
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Media";

      break;
    case 'g':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask[0], 1/16.0, Mat::zeros(3,3,CV_32F), maskAux);
      mask[0] = maskAux.clone();
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Gaussiano";
      break;
    case 'h':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, horizontal);
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Horizontal";
      break;
    case 'v':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, vertical);
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Vertical";
      break;
    case 'l':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, laplacian);
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Laplaciano";
      break;
    case 'i':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, identidade);
      mask[1] = Mat(3, 3, CV_32F, identidade);
      text = "Identidade";
      break;
    case 'e':
	  menu();
      mask[0] = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask[0], 1/16.0, Mat::zeros(3,3,CV_32F), maskAux);
      mask[0] = maskAux.clone();
      mask[1] = Mat(3, 3, CV_32F, laplacian);
      text = "Laplaciano do Gaussiano";
      break;
    default:
      break;
    }
  }
  return 0;
}
