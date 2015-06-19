#ifndef ODOTHREAD_H
#define ODOTHREAD_H


#include <QPixmap>
#include <CLThreadsafeDeque.hpp>
#include <memory>
#include <HTThreadHelper.h>
#include </home/bodereau/ObjectTracker/include/SaladDetector.h>

class VisionThread : private HTThreadHelper
{

public:
    explicit VisionThread();
    std::vector<std::unique_ptr<QPixmap>> getBothPixmap();
    void pushBothPixmap(std::unique_ptr<QPixmap>,std::unique_ptr<QPixmap>);
    void pushXY(std::unique_ptr<int>,std::unique_ptr<int>);
    std::vector<std::unique_ptr<int>> getXY();
    void init();
    void cleanup();
    bool is_exception_free();
    void choose_algorithm(bool);

private:
    virtual void ThreadFuncHelper() final;
    virtual void ThreadCleanupHookHelper() final;

    void exception_caught();

    SaladDetector saladDetector_;
    bool odo_;
    cl::ThreadsafeDeque<std::unique_ptr<QPixmap>> pixmap_clqueue_;
    cl::ThreadsafeDeque<std::unique_ptr<QPixmap>> pixmap_clqueue_result_;
    cl::ThreadsafeDeque<std::unique_ptr<int>> x_clqueue;
    cl::ThreadsafeDeque<std::unique_ptr<int>> y_clqueue;
    std::string path_original_;
    std::string path_result_;

    std::exception_ptr ex_ptr_;
    bool exception_caught_;
};

#endif // ODOTHREAD_H
