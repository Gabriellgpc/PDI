#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

#define OBJ_COLOR 255
#define BACK_COLOR 0
#define NEW_BACK_COLOR ((OBJ_COLOR - BACK_COLOR)/2)

int main(int argc, char** argv){
  Mat image;
  int width, height;
  int nbolhas_com_buracos = 0, nbolhas_sem_buracos = 0;

  image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);

  if(!image.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }
  imshow("Original", image);
  width = image.size().width;
  height= image.size().height;

  ///////////////////remove bolhas/bolas das laterais///////////////////
  //remove da borda superior e inferior
  for(int i = 0; i < width; i++){
    if(image.at<uint8_t>(0, i) == OBJ_COLOR)
      floodFill(image, CvPoint(i, 0), BACK_COLOR);
    if(image.at<uint8_t>(height-1,i) == OBJ_COLOR)
      floodFill(image, CvPoint(i, height-1), BACK_COLOR);
  }

  //remove das laterais
  for(int i = 0; i < height; i++){
    //lateral esquerda
    if(image.at<uint8_t>(i, 0) == OBJ_COLOR)
      floodFill(image, CvPoint(0, i), BACK_COLOR);
    //lateral direita
    if(image.at<uint8_t>(i, width-1) == OBJ_COLOR)
      floodFill(image, CvPoint(width-1, i), BACK_COLOR);
  }
  imshow("Passo 1", image);
  /////////////////////conta bolhas/////////////////
  //troca o background, para facilitar a identificar os buracos das bolhas
  floodFill(image, CvPoint(0,0), NEW_BACK_COLOR);
  imshow("Passo 2", image);
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
    {
      //identifica uma bolha com buraco
      if(image.at<uint8_t>(i,j) == BACK_COLOR && image.at<uint8_t>(i,j-1) == OBJ_COLOR){
        //soma um no numero de bolhas e "apaga" a bolha encontrada
        nbolhas_com_buracos++;
        floodFill(image, CvPoint(j-1, i), NEW_BACK_COLOR);
      }
    }
  imshow("Passo 3", image);
  //conta bolhas sem buracos
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
    {
      //identifica uma bola
      if(image.at<uint8_t>(i,j) == OBJ_COLOR){
        //soma um no numero de bolhas e "apaga" a bolha encontrada
        nbolhas_sem_buracos++;
        floodFill(image, CvPoint(j, i), NEW_BACK_COLOR);
      }
    }

  std::cout << "Bolhas sem buracos:" << nbolhas_sem_buracos << '\n';
  std::cout << "Bolhas com buracos:" << nbolhas_com_buracos << '\n';

  imshow("Resultado final", image);
  waitKey();
  return 0;
}
