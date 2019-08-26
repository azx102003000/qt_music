
/**************************************************
*           音乐播放器
*       使用QMediaPlay播放,需要LAVFilters-0.65.exe *.mp3\*.wav
*
*       QSoundEffect是播放wav格式的文件
*
***************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QTimer>
#include <QModelIndex>

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
    void click_open();              //打开文件添加到list里
    void click_stop();              //设置停止播放
    void click_suspend();           //设置暂停与播放
    void click_mute(bool value);    //是否设置静音
    void click_playmode();          //选择播放模式
    void click_next();              //下一首
    void click_last();              //上一首
    void slider_move(int);          //移动进度条

    void changevulume(int value);   //音量大小调节0-100
    void time_out();                //超时
    void click_double(QModelIndex index);
private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QTimer * timer;
    bool flag;
    int hh,mm,ss;
    long long temp,temp1;
    int hh1,mm1,ss1;
};

#endif // MAINWINDOW_H
