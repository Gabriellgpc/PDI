#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <fstream>
#include <QDebug>

void func_run(void* X)
{
    ((MainWindow*)X)->run();
}

//BGR
static const cv::Vec3b PxLARANJA(0,127,255);
static const cv::Vec3b PxAZUL(255, 0, 0);
static const cv::Vec3b PxAMARELO(0,255,255);
static const cv::Vec3b PxVERDE(0,255,0);
static const cv::Vec3b PxMAGENTA(255, 0, 255);
static const cv::Vec3b PxCIANO(255,255, 0);
static const cv::Vec3b PxPRETO(0,0,0);
static const cv::Vec3b PxBRANCO(255,255,255);

static const cv::Vec3b COLORS[NUM_COLORS] = { PxLARANJA, PxAZUL, PxAMARELO, PxVERDE, PxMAGENTA, PxCIANO};
static const cv::Vec3b REVERSE_COLORS[NUM_COLORS] = { PxAZUL, PxLARANJA, PxAZUL, PxMAGENTA, PxVERDE, cv::Vec3b(0,0,255)};



MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow),
    video(),
    frame(),
    on(true),
    capturing(false),
    mode(MODES::REAL)
{
    ui->setupUi(this);


    for(uint i = 0; i < NUM_COLORS; i++)
    {
        this->thresholdsColor[i] = HSVthresholds();
    }

    this->video.open(1);
    if (!this->video.isOpened())
    {
        QApplication::quit();
        exit(1);
    }

    video.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    video.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    video.set(cv::CAP_PROP_FPS, 30);

    this->thr_capture = std::thread(func_run,this);
    this->capturing = true;

    QObject::connect(ui->myLabel, &QLabelImage::MouseMove, this, &MainWindow::move);
    QObject::connect(ui->myLabel, &QLabelImage::MousePress, this, &MainWindow::press);

}

MainWindow::~MainWindow()
{
    delete ui;
    this->on = false;
    this->thr_capture.join();
}

void MainWindow::press(int x, int y)
{
    if(mode != MODES::REAL && mode != MODES::COR_ATUAL)return;

    cv::Mat BGR = frame(cv::Rect(x,y,1,1)).clone();
    cv::Mat HSV;
    cv::cvtColor(BGR, HSV, CV_BGR2HSV);
    cv::Vec3b hsv =HSV.at<cv::Vec3b>(0,0);

    ui->Hmax_horizontalSlider->setValue( ((hsv[0]+20) > 175)?175: (hsv[0]+20));
    ui->Hmin_horizontalSlider->setValue( ((hsv[0]-20) > 175)?175: (hsv[0]-20) );
}

void MainWindow::move(int x, int y)
{
    if(x < 0 || x >= 640)return;
    if(y < 0 || y >= 480)return;

    cv::Mat image = imgProcessada.clone();
    cv::Mat HSV;
    cv::Mat BGR = image(cv::Rect(x,y,1,1));

    ui->lcdNumber_x->display(x);
    ui->lcdNumber_y->display(y);


    cv::cvtColor(BGR, HSV, CV_BGR2HSV);
    cv::Vec3b hsv =HSV.at<cv::Vec3b>(0,0);

    ui->lcdNumber_H->display(hsv[0]);
    ui->lcdNumber_S->display(hsv[1]);
    ui->lcdNumber_V->display(hsv[2]);
}

void MainWindow::run()
{
    while(this->on)
    {
       if(!this->capturing)
       {
        std::this_thread::yield();
        continue;
       }
       video >> frame;
       cv::resize(frame, frame, cv::Size(640, 480));
       processImage();
    }
}

void MainWindow::processImage()
{
    COLOR color;
    int corAtual;
    cv::Mat frame_threshold;
    cv::Mat imgHSV;
    cv::Mat segment;

    switch (mode) {
    case MODES::REAL:
        imgProcessada = frame.clone();
        updateImage();
        break;
    case MODES::COR_ATUAL:
         //exibi apenas objetos identificados com a cor atual
        for(int y = 0; y < HEIGHT; y++)
        {
            for(int x = 0; x < WIDTH; x++)
            {
                color = getColor(x,y);
                if(color == ui->comboBox_CurrentColor->currentIndex())
                {
                    imgProcessada.at<cv::Vec3b>(y,x) = frame.at<cv::Vec3b>(y,x);
                }else
                {
                    imgProcessada.at<cv::Vec3b>(y,x) = REVERSE_COLORS[ui->comboBox_CurrentColor->currentIndex()];
                }
            }
        }
        updateImage();
        break;
    case MODES::SEGMENTADO:
        //exibi todos os objetos nas cores identificaveis
        for(int y = 0; y < HEIGHT; y++)
        {
            for(int x = 0; x < WIDTH; x++){
                color = getColor(x,y);
                if(color != UNDEF){
                    imgProcessada.at<cv::Vec3b>(y,x) = COLORS[color];
                }else{
                    imgProcessada.at<cv::Vec3b>(y,x) = cv::Vec3b(0,0,0);
                }
            }
        }
        updateImage();
        break;
    case MODES::IDENTIFICACAO:
        //identifica as tags, calculando e destacando seus centros e sua orientacao
        imgProcessada = frame.clone();

        corAtual = ui->comboBox_CurrentColor->currentIndex();
        cv::cvtColor(frame, imgHSV, CV_BGR2HSV);
        cv::inRange(imgHSV,
                    cv::Scalar(thresholdsColor[corAtual].HSV_lower[0], thresholdsColor[corAtual].HSV_lower[1], thresholdsColor[corAtual].HSV_lower[2]),
                    cv::Scalar(thresholdsColor[corAtual].HSV_upper[0], thresholdsColor[corAtual].HSV_upper[1], thresholdsColor[corAtual].HSV_upper[2]),
                    frame_threshold);
        cv::imshow("Resultado", frame_threshold);

        break;
    default:
        break;
    }
}

void MainWindow::updateImage()
{
    if(imgProcessada.data == nullptr)return;
    cv::cvtColor(imgProcessada, imgProcessada, CV_BGR2RGB);
    ui->myLabel->setPixmap(QPixmap::fromImage(QImage(imgProcessada.data, imgProcessada.cols, imgProcessada.rows, imgProcessada.step, QImage::Format_RGB888)));
}


COLOR MainWindow::getColor(int x, int y)
{

    cv::Mat BGR = frame(cv::Rect(x,y,1,1)).clone();
    cv::Mat HSV;
    cv::cvtColor(BGR, HSV, CV_BGR2HSV);
    cv::Vec3b hsv =HSV.at<cv::Vec3b>(0,0);
    struct HSVthresholds hsvThr;

    for(uint c = 0; c < NUM_COLORS; c++){
        hsvThr = this->thresholdsColor[c];
        if(hsv[0] <= hsvThr.HSV_upper[0] && hsv[0] >= hsvThr.HSV_lower[0])
        {
            if(hsv[1] <= hsvThr.HSV_upper[1] && hsv[1] >= hsvThr.HSV_lower[1])
            {
                if(hsv[2] <= hsvThr.HSV_upper[2] && hsv[2] >= hsvThr.HSV_lower[2])
                {
                    return static_cast<COLOR>(c);
                }
            }
        }
    }

    return UNDEF;
}

void MainWindow::on_pause_pushButton_clicked()
{
    this->capturing = !this->capturing;
    if(this->capturing){
        ui->pause_pushButton->setText("Pause");
    }else{
        ui->pause_pushButton->setText("Continue");
    }
}

void MainWindow::on_Hmax_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Hmax_spinBox->value()){
        if(value <= ui->Hmin_horizontalSlider->value())
        {
            ui->Hmin_horizontalSlider->setValue(value-1);
        }
        ui->Hmax_spinBox->setValue(value);
    }
//    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].Hmax = value;
    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_upper[0] = value;
}

void MainWindow::on_Hmax_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Hmax_horizontalSlider->value()){
        if(arg1 <= ui->Hmin_spinBox->value())
        {
            ui->Hmin_spinBox->setValue(arg1-1);
        }
        ui->Hmax_horizontalSlider->setValue(arg1);
    }
}

void MainWindow::on_Hmin_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Hmin_spinBox->value()){
        if(value >= ui->Hmax_horizontalSlider->value())
        {
            ui->Hmax_horizontalSlider->setValue(value+1);
        }
        ui->Hmin_spinBox->setValue(value);
    }

    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_lower[0] = value;
}

void MainWindow::on_Hmin_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Hmin_horizontalSlider->value()){
        if(arg1 >= ui->Hmax_spinBox->value())
        {
            ui->Hmax_spinBox->setValue(arg1-1);
        }
        ui->Hmin_horizontalSlider->setValue(arg1);
    }
}

void MainWindow::on_Smax_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Smax_spinBox->value()){
        if(value <= ui->Smin_horizontalSlider->value())
        {
            ui->Smin_horizontalSlider->setValue(value-1);
        }
        ui->Smax_spinBox->setValue(value);
    }

//    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].Smax = value;
    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_upper[1] = value;
}

void MainWindow::on_Smax_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Smax_horizontalSlider->value()){
        if(arg1 <= ui->Smin_spinBox->value())
        {
            ui->Smin_spinBox->setValue(arg1-1);
        }
        ui->Smax_horizontalSlider->setValue(arg1);
    }
}

void MainWindow::on_Smin_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Smin_spinBox->value()){
        if(value >= ui->Smax_horizontalSlider->value())
        {
            ui->Smax_horizontalSlider->setValue(value+1);
        }
        ui->Smin_spinBox->setValue(value);
    }

//    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].Smin = value;
    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_lower[1] = value;
}

void MainWindow::on_Smin_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Smin_horizontalSlider->value()){
        if(arg1 >= ui->Smax_spinBox->value())
        {
            ui->Smax_spinBox->setValue(arg1-1);
        }
        ui->Smin_horizontalSlider->setValue(arg1);
    }
}

void MainWindow::on_Vmax_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Vmax_spinBox->value()){
        if(value <= ui->Vmin_horizontalSlider->value())
        {
            ui->Vmin_horizontalSlider->setValue(value-1);
        }
        ui->Vmax_spinBox->setValue(value);
    }

//    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].Vmax = value;
    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_upper[2] = value;
}

void MainWindow::on_Vmax_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Vmax_horizontalSlider->value()){
        if(arg1 <= ui->Vmin_spinBox->value())
        {
            ui->Vmin_spinBox->setValue(arg1-1);
        }
        ui->Vmax_horizontalSlider->setValue(arg1);
    }
}

void MainWindow::on_Vmin_horizontalSlider_valueChanged(int value)
{
    if(value != ui->Vmin_spinBox->value()){
        if(value >= ui->Vmax_horizontalSlider->value())
        {
            ui->Vmax_horizontalSlider->setValue(value+1);
        }
        ui->Vmin_spinBox->setValue(value);
    }

//    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].Vmin = value;
    this->thresholdsColor[ui->comboBox_CurrentColor->currentIndex()].HSV_lower[2] = value;
}

void MainWindow::on_Vmin_spinBox_valueChanged(int arg1)
{
    if(arg1 != ui->Vmin_horizontalSlider->value()){
        if(arg1 >= ui->Vmax_spinBox->value())
        {
            ui->Vmax_spinBox->setValue(arg1-1);
        }
        ui->Vmin_horizontalSlider->setValue(arg1);
    }

}

void MainWindow::on_comboBox_modeView_activated(int index)
{
    switch (index) {
    case 0:
        this->mode = MODES::REAL;
        break;
    case 1:
        this->mode = MODES::COR_ATUAL;
        break;
    case 2:
        this->mode = MODES::SEGMENTADO;
        break;
    case 3:
        this->mode = MODES::IDENTIFICACAO;
        break;
    default:
        break;
    }
}

void MainWindow::on_comboBox_CurrentColor_activated(int index)
{
    struct HSVthresholds hsvRange = this->thresholdsColor[index];

    ui->Hmin_horizontalSlider->setValue(hsvRange.HSV_lower[0]);
    ui->Hmax_horizontalSlider->setValue(hsvRange.HSV_upper[0]);
    ui->Smin_horizontalSlider->setValue(hsvRange.HSV_lower[1]);
    ui->Smax_horizontalSlider->setValue(hsvRange.HSV_upper[1]);
    ui->Vmin_horizontalSlider->setValue(hsvRange.HSV_lower[2]);
    ui->Vmax_horizontalSlider->setValue(hsvRange.HSV_upper[2]);
}

void MainWindow::on_finish_pushButton_clicked()
{
    std::ofstream out;
    out.open("../calibracao.cal");
    for(uint c = 0; c < 3; c++){
        out << (int)this->thresholdsColor[c].HSV_lower[0] << '\n';
        out << (int)this->thresholdsColor[c].HSV_lower[1] << '\n';
        out << (int)this->thresholdsColor[c].HSV_lower[2] << '\n';

        out << (int)this->thresholdsColor[c].HSV_upper[0] << '\n';
        out << (int)this->thresholdsColor[c].HSV_upper[1] << '\n';
        out << (int)this->thresholdsColor[c].HSV_upper[2] << '\n';
    }

    out.close();
}
