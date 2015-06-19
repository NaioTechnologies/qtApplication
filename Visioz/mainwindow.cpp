#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <CLPrint.hpp>
#include <QDebug>
#include <QFile>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showFullScreen();

    //new Q_DebugStream(std::cout, ui->textEdit); //Redirect Console output to QTextEdit
   // Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit

    result_only_ = false;
    original_view_ = true;
    odometry_view_ = false;

    ALGO_USE = ODOMETRY;

    stereo_stylesheet_ = QString("color:#333; border:2px solid #4DAF7C;"
                                "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #4DAF7C stop: 1 #4DAF7C);"
                                "min-width: 80px;");
    color_segment_stylesheet_ = QString("color:#333; border: 2px solid #26C281;"
                                       "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #26C281 stop: 1 #26C281);"
                                       "min-width: 80px;");
    odometry_stylesheet_= QString( "color: #333;border: 2px solid #2ECC71;"
                                  "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #2ECC71 stop: 1 #2ECC71);"
                                  "min-width: 80px;");

    real_stylesheet_= QString( "color: #333;border: 2px solid #03A678;"
                                  "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #03A678 stop: 1 #03A678);"
                                  "min-width: 80px;");

    clicked_stylesheet_= QString( "color: #333;border: 2px solid #C8F7C5;"
                                 "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #C8F7C5 stop: 1 #C8F7C5);"
                                 "min-width: 80px;");

    error_stylesheet_= QString( "color: #333;border: 2px solid #95A5A6;"
                                 "background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #95A5A6 stop: 1 #95A5A6);"
                                 "min-width: 80px;");

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect( );
    effect->setBlurRadius( 10);
    ui->both_button->setGraphicsEffect( effect );
    effect = new QGraphicsDropShadowEffect( );
    effect->setBlurRadius( 10 );
    effect->setXOffset(1);
    effect->setYOffset(1);
    ui->label_odo_1->setGraphicsEffect( effect );
    effect = new QGraphicsDropShadowEffect( );
    effect->setBlurRadius( 10 );
    effect->setXOffset(1);
    effect->setYOffset(1);
    ui->label_odo_2->setGraphicsEffect( effect );
    effect = new QGraphicsDropShadowEffect( );
    effect->setBlurRadius( 10 );
    effect->setXOffset(1);
    effect->setYOffset(1);
    ui->label_odo_3->setGraphicsEffect( effect );

    ui->log_box->setVisible(false);
    timer_ = std::make_unique<QTimer>(this);
    timer_->setInterval(INTERVAL_TIMER);
    connect(timer_.get(), SIGNAL(timeout()), this, SLOT(change_displayed_images()));
    timer_->start();

    vision_thread_.init();



}

MainWindow::~MainWindow()
{
    disconnect(timer_.get(),0,0,0);
    vision_thread_.cleanup();
}


void MainWindow::change_displayed_images(){


    //function used by the timer
    try{

        switch (ALGO_USE) {
        case ODOMETRY:
            vision_thread_.choose_algorithm(true);
            odometry();
            break;
        case COLOR_SEGMENT:
            vision_thread_.choose_algorithm(false);
            color_segment();
            break;
        case STEREO:
            stereo();
            break;
        case REAL:
            vision_thread_.choose_algorithm(true);
            real();
            break;
        case ERROR:
            break;
        default:
            break;
        }
    }
    catch(const std::exception& e){

        cl::print_line(e.what());

        //disconnect(timer_.get(),0,0,0);
        std::string threadError{};
        if( !vision_thread_.is_exception_free()){
            vision_thread_.cleanup();
            threadError = "Odo";
        }

        std::string box_content = std::string(e.what()) + " from thread "+threadError;
        display_log_box(box_content);

        ALGO_USE = ERROR;
    }

}

void MainWindow::set_alpha_channel(QPixmap& pixmap, int const alpha)
{
  QImage image(pixmap.toImage().convertToFormat(QImage::Format_ARGB32));
  for (int x(0); x != image.width(); ++x)
  {
    for (int y(0); y != image.height(); ++y)
    {
      QColor color(image.pixel(x,y));
      color.setAlpha(alpha);
      image.setPixel(x, y, color.rgba());
    }
  }
  pixmap = QPixmap::fromImage(image);
}

void MainWindow::odometry(){

    std::vector<std::unique_ptr<int>> vec = vision_thread_.getXY();
    ui->info_odo->setVisible(true);
    ui->both_button->setVisible(true);
    ui->info_odo->setGeometry(10,ui->widget->height()-200,350,190);
    ui->label_odo_1->setFixedSize(ui->info_odo->width(),ui->info_odo->height()/3);
    ui->label_odo_2->setFixedSize(ui->info_odo->width(),ui->info_odo->height()/3);
    ui->label_odo_3->setFixedSize(ui->info_odo->width(),ui->info_odo->height()/3);
    if(vec.size() > 1){
        ui->label_odo_2->setText("Y : "+ QString::number(*(vec.at(1)))+".0 m");
        ui->label_odo_3->setText("X : "+ QString::number(*(vec.at(0)))+".0 m");
    }
    ui->stereo_button->setStyleSheet(stereo_stylesheet_);
    ui->odometry_button->setStyleSheet(clicked_stylesheet_);
    ui->real_button->setStyleSheet(real_stylesheet_);
    ui->color_segment_button->setStyleSheet(color_segment_stylesheet_);
    display_images();
}

void MainWindow::stereo(){
    ui->info_odo->setVisible(false);
    ui->both_button->setVisible(true);
    ui->stereo_button->setStyleSheet(clicked_stylesheet_);
    ui->odometry_button->setStyleSheet(odometry_stylesheet_);
    ui->real_button->setStyleSheet(real_stylesheet_);
    ui->color_segment_button->setStyleSheet(color_segment_stylesheet_);
    display_images();

}

void MainWindow::color_segment(){
    ui->info_odo->setVisible(false);
    ui->both_button->setVisible(true);
    ui->stereo_button->setStyleSheet(stereo_stylesheet_);
    ui->odometry_button->setStyleSheet(odometry_stylesheet_);
    ui->real_button->setStyleSheet(real_stylesheet_);
    ui->color_segment_button->setStyleSheet(clicked_stylesheet_);
    display_images();
}

void MainWindow::real(){
    ui->info_odo->setVisible(false);
    ui->both_button->setVisible(false);
    ui->stereo_button->setStyleSheet(stereo_stylesheet_);
    ui->odometry_button->setStyleSheet(odometry_stylesheet_);
    ui->real_button->setStyleSheet(clicked_stylesheet_);
    ui->color_segment_button->setStyleSheet(color_segment_stylesheet_);

    std::vector<std::unique_ptr<QPixmap>> vec = vision_thread_.getBothPixmap();
    if(vec.size() <2)
        return;
    QPixmap image2 = *(vec.at(1)) ;
    QPixmap image3 = *(vec.at(0)) ;
    int parent_width = ui->widget->width()/2;
    int parent_height = ui->widget->height();
    QPixmap original_image = image2.scaled(parent_width,parent_height,Qt::KeepAspectRatio);
    QPixmap algo_image = image3.scaled(parent_width,parent_height,Qt::KeepAspectRatio);
    ui->image2->setVisible(true);
    ui->slider_alpha->setVisible(false);
    ui->image1->setPixmap(original_image);
    ui->image1->setFixedSize(parent_width,parent_height);
    ui->image1->setGeometry(0,0,parent_width,parent_height);
    ui->image2->setPixmap(algo_image);
    ui->image2->setFixedSize(parent_width,parent_height);
    ui->image2->setGeometry(parent_width,0,parent_width,parent_height);

}

void MainWindow::display_images(){
    //get the last pixtures captured ( could be the same as before )

    std::vector<std::unique_ptr<QPixmap>> vec;
    vec = vision_thread_.getBothPixmap();
    if(vec.size() <2)
        return;
    std::cout<<"vectors"<<std::endl;
    int parent_width = ui->widget->width();
    int parent_height = ui->widget->height();
    QPixmap original_image = (*(vec.at(1))).scaled(parent_width,parent_height,Qt::KeepAspectRatio);
    QPixmap algo_image = (*(vec.at(0))).scaled(parent_width,parent_height,Qt::KeepAspectRatio);
    //set_alpha_channel(algo_image,ui->slider_alpha->value());
    std::cout<<"alpha channel done"<<std::endl;
    if(original_view_ && result_only_){
         ui->image2->setVisible(true);
         ui->slider_alpha->setVisible(true);
         ui->image1->setPixmap(original_image);
         ui->image1->setFixedSize(parent_width,parent_height);
         ui->image1->setGeometry((parent_width-original_image.width())/2,0,parent_width,parent_height);
         ui->image2->setPixmap(algo_image);
         ui->image2->setFixedSize(parent_width,parent_height);
         ui->image2->setGeometry((parent_width-algo_image.width())/2,0,parent_width,parent_height);
         ui->slider_alpha->setGeometry(parent_width-100,150,ui->slider_alpha->width(),parent_height-300);
         std::cout<<"both images displayed"<<std::endl;

    }else if(original_view_){
        ui->slider_alpha->setVisible(false);
        ui->image1->setPixmap(original_image);
        ui->image1->setFixedSize(ui->widget->size());
        ui->image1->setGeometry((parent_width-original_image.width())/2,0,parent_width,parent_height);
        ui->image2->setVisible(false);
        std::cout<<"1 image displayed"<<std::endl;
    }else {
        ui->slider_alpha->setVisible(false);
        ui->image1->setPixmap(algo_image);
        ui->image1->setFixedSize(ui->widget->size());
        ui->image2->setVisible(false);
    }
}

void MainWindow::display_log_box(std::string error){
    ui->log_box->setVisible(true);
    ui->log_box->setGeometry(ui->widget->width()/2,ui->widget->height()-200,ui->widget->width()/2 - 50,190);
    ui->log_title->setFixedSize(ui->log_box->width(),ui->log_box->height()/3);
    ui->log_content->setFixedSize(ui->log_box->width(),(ui->log_box->height()/3));
    ui->error_run_thread->setFixedSize(ui->log_box->width()/2,(ui->log_box->height()/3));
    ui->error_continue->setFixedSize(ui->log_box->width()/2,(ui->log_box->height()/3));
    ui->log_content->setText(QString(error.c_str()));
    ui->log_content->setWordWrap(true);

    switch (ALGO_USE) {
    case ODOMETRY:
        ANCIEN_ALGO = ODOMETRY;
        ui->odometry_button->setStyleSheet(error_stylesheet_);
        break;
    case COLOR_SEGMENT:
        ANCIEN_ALGO = COLOR_SEGMENT;
        ui->color_segment_button->setStyleSheet(error_stylesheet_);
        break;
    case STEREO:
        ANCIEN_ALGO = STEREO;
        ui->stereo_button->setStyleSheet(error_stylesheet_);
        break;
    case REAL:
        ANCIEN_ALGO = REAL;
        ui->real_button->setStyleSheet(error_stylesheet_);
        break;
    default:
        break;
    }

}

void MainWindow::on_quit_button_clicked()
{

    vision_thread_.cleanup();

    this->close();
}


void MainWindow::on_odometry_button_clicked()
{
    ALGO_USE =ODOMETRY;
}

void MainWindow::on_color_segment_button_clicked()
{
    ALGO_USE = COLOR_SEGMENT;
}

void MainWindow::on_stereo_button_clicked()
{
    ALGO_USE = STEREO;
}

void MainWindow::on_real_button_clicked()
{
    ALGO_USE = REAL;
}

void MainWindow::on_both_button_clicked()
{
    if(result_only_)
        ui->both_button->setText("SHOW RESULT:\nNo");
    else
        ui->both_button->setText("SHOW RESULT:\nYes");
    original_view_ = true;
    result_only_ = !result_only_;
}



void MainWindow::on_error_continue_clicked()
{
     ui->log_box->setVisible(false);
}

void MainWindow::on_error_run_thread_clicked()
{

    ui->log_box->setVisible(false);
    vision_thread_.init();
    if(ALGO_USE == ERROR)
        ALGO_USE = ANCIEN_ALGO;
}
