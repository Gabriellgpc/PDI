#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <thread>
#include <QTimer>

enum MODES{
    REAL,
    COR_ATUAL,
    SEGMENTADO,
    IDENTIFICACAO
};

#define NUM_COLORS 6
#define WIDTH 640
#define HEIGHT 480

enum COLOR
{
    LARANJA,
    AZUL,
    AMARELO,
    VERDE,
    MAGENTA,
    CIANO,
    UNDEF
};


struct HSVthresholds
{
//    HSVthresholds():Hmin(0), Hmax(179), Smin(0), Smax(255), Vmin(0), Vmax(255){}
//    int Hmin,Hmax;
//    int Smin,Smax;
//    int Vmin,Vmax;
    HSVthresholds():HSV_lower(0,0,0), HSV_upper(175,255,255){}
    cv::Vec3b HSV_lower;
    cv::Vec3b HSV_upper;
};



void func_run(void* X);

namespace Ui
{
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
     void press(int x, int y);
     void move(int x, int y);

private slots:
     void on_pause_pushButton_clicked();
     void on_Hmax_horizontalSlider_valueChanged(int value);

     void on_Hmax_spinBox_valueChanged(int arg1);

     void on_Hmin_horizontalSlider_valueChanged(int value);

     void on_Hmin_spinBox_valueChanged(int arg1);

     void on_Smax_horizontalSlider_valueChanged(int value);

     void on_Smax_spinBox_valueChanged(int arg1);

     void on_Smin_horizontalSlider_valueChanged(int value);

     void on_Smin_spinBox_valueChanged(int arg1);

     void on_Vmax_horizontalSlider_valueChanged(int value);

     void on_Vmax_spinBox_valueChanged(int arg1);

     void on_Vmin_horizontalSlider_valueChanged(int value);

     void on_Vmin_spinBox_valueChanged(int arg1);

     void on_comboBox_modeView_activated(int index);


     void on_comboBox_CurrentColor_activated(int index);

     void on_finish_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    cv::VideoCapture video;
    cv::Mat frame;
    cv::Mat imgProcessada;
    std::thread thr_capture;
    bool on;
    bool capturing;
    MODES mode;
    HSVthresholds thresholdsColor[NUM_COLORS];

    COLOR getColor(int x, int y);

    void run();
    void processImage();

    friend void func_run(void* X);
    friend void func_process(void* X);

    void updateImage();
};

#endif // MAINWINDOW_H
