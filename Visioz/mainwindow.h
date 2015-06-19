#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <visionthread.h>
#include <QString>
#include <QPainter>
#include <QRgb>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <QGraphicsDropShadowEffect>
#include <memory>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void change_displayed_images();
    void on_odometry_button_clicked();
    void on_color_segment_button_clicked();
    void on_stereo_button_clicked();
    void on_quit_button_clicked();
    void on_both_button_clicked();
    void on_real_button_clicked();

    void on_error_continue_clicked();

    void on_error_run_thread_clicked();

private:
    Ui::MainWindow * ui;

    bool original_view_;
    bool result_only_;
    bool odometry_view_;

    bool error_ = false;

    int ALGO_USE = 0;
    static const int ODOMETRY = 1;
    static const int COLOR_SEGMENT = 2;
    static const int STEREO = 3;
    static const int REAL = 4;
    static const int ERROR = 9;
    int ANCIEN_ALGO = 0;

    static const int INTERVAL_TIMER = 1;

    QString stereo_stylesheet_;
    QString color_segment_stylesheet_ ;
    QString odometry_stylesheet_;
    QString real_stylesheet_;
    QString clicked_stylesheet_;
    QString error_stylesheet_;

    VisionThread  vision_thread_;

    std::unique_ptr<QTimer> timer_;

    void odometry();
    void color_segment();
    void stereo();
    void real();

    void display_images();
    void display_log_box(std::string);

    void  set_alpha_channel(QPixmap& pixmap, int const alpha);
};

#endif // MAINWINDOW_H
