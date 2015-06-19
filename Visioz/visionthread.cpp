#include "visionthread.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <CLFileSystem.h>
#include <IO/BlueFoxStereo.h>

VisionThread::VisionThread()
{

    exception_caught_ = false;
    odo_=true;
}


void VisionThread::choose_algorithm(bool odo){
    //TODO: enum when all algo
    odo_=odo;
}

void VisionThread::ThreadFuncHelper() {
    MarkThreadReady();


    try{

        io::BlueFoxStereo stereoBench;
        cv::Size size( static_cast<int32_t>(752), static_cast<int32_t>(480) );
        stereoBench.start( ht::ColorSpace::Rgb, 752, 480, 50, 40000, true );

        saladDetector_.set_erode( 3 );
        saladDetector_.set_dilate( 3 );
        saladDetector_.init_blob_detector( true, true, false, true, false );
        saladDetector_.set_blob_detector_threshold( 0.0f, 255.0f );
        saladDetector_.set_blob_detector_area_range( 200.0f, 1000000.0f );
        saladDetector_.set_blob_detector_circularity_range( 0.1f, 1.0f );
        saladDetector_.set_blob_detector_convexity_range( 0.0f, 1.0f );
        saladDetector_.set_blob_detector_inertia_range( 0.1f, 1.0f );
        int x_mem=0;
        int y_mem=0;
        while (!ThreadMustExit()) {

            cm::BitmapPairEntryUniquePtr entry;
            stereoBench.wait_entry( entry );
            stereoBench.clear_entry_buffer();
            cv::Mat matL = cv::Mat( size, CV_8UC3 );
            cv::Mat matR = cv::Mat( size, CV_8UC3 );
            matL.data = entry->bitmap_left().data();
            matR.data = entry->bitmap_right().data();
            cv::cvtColor(matL,matL,CV_BGR2RGB);
            cv::cvtColor(matR,matR,CV_BGR2RGB);

            if(odo_){
                std::unique_ptr<QPixmap> img2 = std::make_unique<QPixmap>(matL.rows,matL.cols);
                std::unique_ptr<QPixmap>  img = std::make_unique<QPixmap>(matR.cols, matR.rows);

                QImage image( matL.data, matL.cols, matL.rows, matL.step, QImage::Format_RGB888 );
                QImage image2( matR.data, matR.cols, matR.rows, matR.step, QImage::Format_RGB888 );
                bool resp = img2->convertFromImage(image2.rgbSwapped());
                img->convertFromImage(image.rgbSwapped());
                std::unique_ptr<int> x_odo = std::make_unique<int>(x_mem++);
                std::unique_ptr<int>  y_odo = std::make_unique<int>(y_mem++);

                cl::print_line(resp);
                pushBothPixmap(std::move(img),std::move(img2));
                pushXY(std::move(x_odo),std::move(y_odo));
            }else{
                saladDetector_.set_src_img(matL);
                saladDetector_.compute();
                saladDetector_.get_output_img();
                cv::Mat imgTiff2 = saladDetector_.get_output_img();

                std::unique_ptr<QPixmap> img2 = std::make_unique<QPixmap>(imgTiff2.rows,imgTiff2.cols);
                std::unique_ptr<QPixmap>  img = std::make_unique<QPixmap>(matL.cols, matL.rows);

                QImage image2( imgTiff2.data, imgTiff2.cols, imgTiff2.rows, imgTiff2.step, QImage::Format_RGB888 );
                QImage image( matL.data, matL.cols, matL.rows, matL.step, QImage::Format_RGB888 );
                img2->convertFromImage(image2.rgbSwapped());
                img->convertFromImage(image.rgbSwapped());

                pushBothPixmap(std::move(img),std::move(img2));
            }

        }
    }
    catch(const std::exception& e){
        exception_caught();
        ex_ptr_ = std::current_exception();
        cl::print_line(e.what());
    }
}

void VisionThread::exception_caught(){
    exception_caught_ = true;
}

bool VisionThread::is_exception_free(){
    return !exception_caught_;
}

void VisionThread::ThreadCleanupHookHelper() {

}

void VisionThread::init(){
    StartThread();
    WaitForThreadReady();
}
void VisionThread::cleanup(){
    ex_ptr_ = NULL;
    TerminateThread();
}


void VisionThread::pushBothPixmap(std::unique_ptr<QPixmap> qpixmap, std::unique_ptr<QPixmap> qpixmap2){

    std::unique_ptr<QPixmap> item ;
    while(pixmap_clqueue_.size()>=1)
        pixmap_clqueue_.try_pop_front(item);
    while(pixmap_clqueue_result_.size()>=1)
        pixmap_clqueue_result_.try_pop_front(item);
    pixmap_clqueue_.push_back(std::move(qpixmap));
    pixmap_clqueue_result_.push_back(std::move(qpixmap2));
}

void VisionThread::pushXY(std::unique_ptr<int> x, std::unique_ptr<int> y){

    std::unique_ptr<int> item ;
    while(x_clqueue.size()>=1)
        x_clqueue.try_pop_front(item);
    while(y_clqueue.size()>=1)
        y_clqueue.try_pop_front(item);
    x_clqueue.push_back(std::move(x));
    y_clqueue.push_back(std::move(y));
}


std::vector<std::unique_ptr<QPixmap>> VisionThread::getBothPixmap(){
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

std::vector<std::unique_ptr<int>> VisionThread::getXY(){
    if(ex_ptr_)
        std::rethrow_exception(ex_ptr_);

    std::unique_ptr<int> x ;
    bool resp1= x_clqueue.try_pop_front(x);
    std::unique_ptr<int> y ;
    bool resp2= y_clqueue.try_pop_front(y);
    std::vector<std::unique_ptr<int>> vec;
    if(resp1)
        vec.push_back(std::move(x));
    if(resp2)
        vec.push_back(std::move(y));
    return vec;
}


