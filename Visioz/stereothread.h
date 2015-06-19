#ifndef STEREOTHREAD_H
#define STEREOTHREAD_H

#include <QPixmap>
#include <CLThreadsafeDeque.hpp>
#include <memory>
#include <HTThreadHelper.h>

class StereoThread : private HTThreadHelper
{

public:
    explicit StereoThread();
    std::vector<std::unique_ptr<QPixmap>> getBothPixmap();
    void pushBothPixmap(std::unique_ptr<QPixmap>,std::unique_ptr<QPixmap>);
    void init();
    void cleanup();
    bool is_exception_free();

private:
    virtual void ThreadFuncHelper() final;
    virtual void ThreadCleanupHookHelper() final;

    void exception_caught();

    cl::ThreadsafeDeque<std::unique_ptr<QPixmap>> pixmap_clqueue_;
    cl::ThreadsafeDeque<std::unique_ptr<QPixmap>> pixmap_clqueue_result_;
    std::string path_original_;
    std::string path_result_;

    std::exception_ptr ex_ptr_;
    bool exception_caught_;
};

#endif // STEREOTHREAD_H
