#include "stereothread.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <CLFileSystem.h>

StereoThread::StereoThread()
{
    std::string tmp = cl::filesystem::folder_current_full();
    tmp.append("/tmp3.png");
    std::string tmp2 = cl::filesystem::folder_current_full();
    tmp2.append("/tmp4.png");
    path_original_ = tmp;
    path_result_ = tmp2;
    exception_caught_ = false;
}



void StereoThread::ThreadFuncHelper() {
    MarkThreadReady();
    int index_image_test = 10;
    try{
        while (!ThreadMustExit()) {
            /* std::string text = "/home/bodereau/Bureau/tiff2/"+std::to_string(index_image_test)+"_l.tiff";
             std::string text2 = "/home/bodereau/Bureau/tiff2/"+ std::to_string(index_image_test++)+"_r.tiff";*/
              std::string text = "/home/dev/tiff2/"+std::to_string(index_image_test)+"_l.tiff";
              std::string text2 = "/home/dev/tiff2/"+ std::to_string(index_image_test++)+"_r.tiff";
            cv::Mat imgTiff = cv::imread(text);

            std::cout<<"odo : write img"<<std::endl;
            cv::Mat imgTiff2 = cv::imread(text2);
            std::cout<<"odo : read img1"<<std::endl;
            std::cout<<"odo : write img2"<<std::endl;

            std::unique_ptr<QPixmap> img2 = std::make_unique<QPixmap>(imgTiff2.rows,imgTiff2.cols);
            std::unique_ptr<QPixmap>  img = std::make_unique<QPixmap>(imgTiff.cols, imgTiff.rows);

            QImage image2( imgTiff2.data, imgTiff2.cols, imgTiff2.rows, imgTiff2.step, QImage::Format_RGB888 );
            QImage image( imgTiff.data, imgTiff.cols, imgTiff.rows, imgTiff.step, QImage::Format_RGB888 );
            bool resp = img2->convertFromImage(image2.rgbSwapped());
            img->convertFromImage(image.rgbSwapped());

            cl::print_line(resp);
            pushBothPixmap(std::move(img),std::move(img2));
            std::cout<<"odo : pushed"<<std::endl;

            if(index_image_test>200){
                throw std::out_of_range("std::out_of_range");
                index_image_test=10;
            }

        }
    }
    catch(const std::exception& e){
        exception_caught();
        ex_ptr_ = std::current_exception();
        cl::print_line(e.what());
    }
}

void StereoThread::exception_caught(){
    exception_caught_ = true;
}

bool StereoThread::is_exception_free(){
    return !exception_caught_;
}

void StereoThread::ThreadCleanupHookHelper() {

}

void StereoThread::init(){
    cl::print_line("odo : init");
    StartThread();
    WaitForThreadReady();
    cl::print_line("odo : init done");
}
void StereoThread::cleanup(){
    ex_ptr_ = NULL;
    TerminateThread();
}


void StereoThread::pushBothPixmap(std::unique_ptr<QPixmap> qpixmap, std::unique_ptr<QPixmap> qpixmap2){

    std::unique_ptr<QPixmap> item ;
    while(pixmap_clqueue_.size()>=1)
        pixmap_clqueue_.try_pop_front(item);
    while(pixmap_clqueue_result_.size()>=1)
        pixmap_clqueue_result_.try_pop_front(item);
    pixmap_clqueue_.push_back(std::move(qpixmap));
    pixmap_clqueue_result_.push_back(std::move(qpixmap2));
}


std::vector<std::unique_ptr<QPixmap>> StereoThread::getBothPixmap(){
    if(ex_ptr_)
        std::rethrow_exception(ex_ptr_);

    std::unique_ptr<QPixmap> item ;
    bool resp1= pixmap_clqueue_result_.try_pop_front(item);
    std::unique_ptr<QPixmap> item2 ;
    bool resp2= pixmap_clqueue_.try_pop_front(item2);
    std::vector<std::unique_ptr<QPixmap>> vec;
    if(resp1)
        vec.push_back(std::move(item));
    if(resp2)
        vec.push_back(std::move(item2));
    return vec;
}
