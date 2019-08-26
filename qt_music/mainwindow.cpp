#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 480);
    flag = false;
    player = new QMediaPlayer;
    player->setVolume(80);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(time_out()));//超时
    connect(ui->mute_box, SIGNAL(clicked(bool)), this,SLOT(click_mute(bool)));
    connect(ui->next_btn, SIGNAL(clicked(bool)), this, SLOT(click_next()));//下一首
    connect(ui->last_btn, SIGNAL(clicked(bool)), this, SLOT(click_last()));//上一首
    connect(ui->mode_btn, SIGNAL(clicked(bool)), this, SLOT(click_playmode()));//播放模式
    connect(ui->open_btn, SIGNAL(clicked(bool)), this, SLOT(click_open()));
    connect(ui->stop_btn, SIGNAL(clicked(bool)), this, SLOT(click_stop()));
    connect(ui->supend_btn, SIGNAL(clicked(bool)), this, SLOT(click_suspend()));
    connect(ui->volume_sli, SIGNAL(valueChanged(int)), this, SLOT(changevulume(int)));//音量
    connect(ui->speed_sli, SIGNAL(valueChanged(int)), this, SLOT(slider_move(int)));//进度条
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(click_double(QModelIndex)));
    ui->volume_sli->setValue(player->volume());
}

MainWindow::~MainWindow()
{
    if(timer->isActive())
    {
        player->stop();
        timer->stop();
    }

    delete timer;
    delete player;
    delete ui;
}
//移动了音乐的进度条
void MainWindow::slider_move(int value)
{
   // if(!ui->speed_sli->isSliderDown())
    if(ui->speed_sli->isSliderDown())
    {
        qDebug()<< value;
        player->setPosition(value*1000);
    }


}
void MainWindow::click_double(QModelIndex index)//双击item后 需要播放的歌曲
{
    qDebug()<<index.row();
    QString string = ui->listWidget->item(index.row())->text();
    player->setMedia(QUrl::fromLocalFile(string));
    ui->link_lab->setText(tr("链接 ") + string.section('/', -1, -1));
    player->play();
    ui->supend_btn->setText("暂停");
    //flag = true;
    timer->start(500);

}
void MainWindow::time_out()
{
    //得到的是ms时间,转换成小时需要 秒 *3600 = 小时

    temp = player->duration() / 1000;   //获得毫秒/1000的时间
    hh = temp / 3600;                   //获得小时的时间
    mm = (temp - hh * 3600) / 60;       //获得分钟的时间
    ss = temp - hh * 3600 - mm * 60;    //获得秒的时间

    temp1 = player->position()/1000;     //获得 毫秒/1000 的时间
    hh1 = temp1 / 3600;                  //获得 小时 的时间
    mm1 = (temp1 - hh1 * 3600) / 60;     //获得 分钟 的时间  player->position()/1000 - player->position()/1000 /60
    ss1 = temp1 - hh1 * 3600 - mm1 * 60; //获得 秒 的时间  (player->position()/1000) - (player->position()/1000/ 3600)*3600 -
//                                                                      (player->position()/1000 - (player->position()/1000/3600) * 3600) / 60 * 60
    ui->speed_lab->setText("进度条:"+QString("%1:%2:%3").arg(hh1).arg(mm1).arg(ss1)+" - " + QString("%1:%2:%3").arg(hh).arg(mm).arg(ss));
    //qDebug()<<player->position() << " --- "<<player->duration()/1000;
    if(ui->speed_sli->maximum() != player->duration()/1000)
        ui->speed_sli->setMaximum(player->duration()/1000);
    if(ui->speed_sli->isSliderDown() == false)
        ui->speed_sli->setValue(player->position()/1000);
    //qDebug()<<QString("%1:%2:%3").arg(hh1).arg(mm1).arg(ss1)<<" *** "<<QString("%1:%2:%3").arg(hh).arg(mm).arg(ss);

    if(temp == temp1)//播放的时间与歌曲总时间相等就退出
    {
        //ui->speed_sli->setValue(0);
        //flag = false;

        ui->speed_lab->setText("进度条:0:0:0 - " + QString("%1:%2:%3").arg(hh).arg(mm).arg(ss));
        if(ui->mode_btn->text() == tr("顺序播放") || ui->mode_btn->text() == tr("列表循环"))//循环播放下一首
        {
            if(ui->mode_btn->text() == tr("列表循环"))
            {
                if(ui->listWidget->currentRow() + 1 == ui->listWidget->count())//最后一首从头开始
                {
//                    int num = ui->listWidget->currentRow();
                    QString string = ui->listWidget->item(0)->text();
                    player->setMedia(QUrl::fromLocalFile(string));
                    player->play();
                    ui->link_lab->setText("链接:" + string.section('/', -1, -1));
                    ui->listWidget->setCurrentRow(0);
                    return;
                }
            }
            else//顺序播放
            {
                if(ui->listWidget->currentRow() + 1 == ui->listWidget->count())//到最后一首就停止
                {
                    ui->supend_btn->setText("播放");
                    timer->stop();
                    return;
                }
            }
            int num = ui->listWidget->currentRow();
            QString string = ui->listWidget->item(num +1)->text();
            player->setMedia(QUrl::fromLocalFile(string));
            player->play();
            ui->link_lab->setText("链接:" + string.section('/', -1, -1));
            ui->listWidget->setCurrentRow(num + 1);
        }

        else if(ui->mode_btn->text() == tr("单曲循环"))
        {
            player->play();
        }
        else if(ui->mode_btn->text() == tr("随机播放"))
        {
            srand(ss);
            int num = rand()%ui->listWidget->count();
            QString string = ui->listWidget->item(num)->text();
            player->setMedia(QUrl::fromLocalFile(string));
            player->play();
            ui->listWidget->setCurrentRow(num);
            ui->link_lab->setText("链接:" + string.section('/', -1, -1));
        }
        //timer->stop();
    }

}
void MainWindow::click_open()//打开
{
    qDebug()<<ui->listWidget->count();
    QString filePath = QFileDialog::getOpenFileName(this,tr("打开音乐文件"),"",tr("*.mp3 *.wav"));
    qDebug()<<filePath;
    if(!filePath.isEmpty())
    {
        for(int i = 0; i < ui->listWidget->count(); i++)
        {
            if(ui->listWidget->item(i)->text() == filePath)
            {
                QMessageBox::warning(this, "提示", QString("这首歌 %1 已存在").arg(filePath));
                return;
            }
        }
        ui->listWidget->insertItem(ui->listWidget->currentRow() + 1, filePath);
    }
//    ui->listWidget->insertItem(ui->listWidget->currentRow()
//    ui->link_lab->setText(tr("链接 ") + filePath);
//    player->setMedia(QUrl::fromLocalFile(filePath));

//    player->play();
//    ui->supend_btn->setText("暂停");
//    flag = true;
//    timer->start(500);



}
void MainWindow::click_stop()//停止
{

    qDebug()<<"isplaying";
    if(player->state() == QMediaPlayer::PlayingState)
    {
        player->stop();
        ui->supend_btn->setText("播放");
        if(timer->isActive())
            timer->stop();
        ui->speed_sli->setValue(0);
        ui->speed_lab->setText("进度条:0:0:0 - " + QString("%1:%2:%3").arg(hh)
                                                                     .arg(mm)
                                                                     .arg(ss));
    }

}

//播放与暂停
void MainWindow::click_suspend()
{
    qDebug()<<player->state()<<player->media().canonicalUrl().toString();

    if(!ui->listWidget->count())
    {
        QMessageBox::warning(this, "提示", "请添加音乐文件");
        return;
    }
    if(player->state() == QMediaPlayer::StoppedState)//停止状态
    {
        if(player->media().canonicalUrl().toString().isEmpty())//链接地址为空
        {
            player->setMedia(QUrl::fromLocalFile( ui->listWidget->item(0)->text()));

        }
       // else
        {
            player->play();
            if(!timer->isActive())
            timer->start(500);
            ui->link_lab->setText("链接:"+ui->listWidget->item(0)->text().section('/', -1, -1));
            ui->supend_btn->setText("暂停");
            ui->listWidget->setCurrentRow(0);
        }
    }
    else if(player->state() == QMediaPlayer::PausedState)//暂停状态
    {
        player->play();
        if(!timer->isActive())
        timer->start(500);
        ui->supend_btn->setText("暂停");
    }
    else if(player->state() == QMediaPlayer::PlayingState)//播放状态
    {
        player->pause();
        if(timer->isActive())
            timer->stop();
        ui->supend_btn->setText("播放");
    }
#if 0
    if(flag)
    {
        player->pause();
        ui->supend_btn->setText("播放");
        flag = false;
        if(timer->isActive())
            timer->stop();
    }
    else
    {
        player->play();
        ui->supend_btn->setText("暂停");
        flag = true;
        if(!timer->isActive())
            timer->start(500);
    }
#endif
}
//播放模式
void MainWindow::click_playmode()
{
    static unsigned char i=0;
    switch(i++)
    {
        case 0:
            ui->mode_btn->setText("列表循环");
            break;
        case 1:
            ui->mode_btn->setText("单曲循环");
            break;
        case 2:
            ui->mode_btn->setText("随机播放");
            break;
        case 3:
            ui->mode_btn->setText("顺序播放");
            i = 0;
            break;
    }
}

/************************************ 上一首 ******************************************/

void MainWindow::click_last()//上一首
{


    if(ui->mode_btn->text() == tr("随机播放"))
    {
        qDebug()<<"随机播放";
        if(ui->listWidget->count() > 0)
        {
            int num = rand()%ui->listWidget->count();
            QString string = ui->listWidget->item(num)->text();
            player->setMedia(QUrl::fromLocalFile(string));
            player->play();
            ui->link_lab->setText("链接:" + string.section('/', -1, -1));
            ui->supend_btn->setText("暂停");
            ui->listWidget->setCurrentRow(num);
            if(!timer->isActive())
                timer->start(500);

        }
        else
            QMessageBox::warning(this, "提示", "请添加歌曲");
    }
    else if(ui->mode_btn->text() == tr("列表循环"))
    {
        if(ui->listWidget->count() > 0)
        {
            if(ui->listWidget->currentRow() < 0)//从最后首开始播放
            {
                player->setMedia(QUrl::fromLocalFile(ui->listWidget->item(ui->listWidget->count()-1)->text()));
                ui->link_lab->setText("链接:"+ui->listWidget->item(ui->listWidget->count()-1)->text().section('/', -1, -1));
                ui->supend_btn->setText("暂停");
                ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
                player->play();
                if(!timer->isActive())
                    timer->start(500);
            }
            else    //在上面的两者之间
            {
                for(int i = 0; i < ui->listWidget->count(); i++)
                {
                    qDebug()<<"last"<<i;
                    if(player->media().canonicalUrl().toString().section('/', -1, -1) == ui->listWidget->item(i)->text().section('/', -1, -1))
                    {
                        QString name;
                        if(i == 0)
                        {
                            i = ui->listWidget->count();
                            name = ui->listWidget->item(i - 1)->text();
                        }
                        else
                        {
                            name = ui->listWidget->item(i - 1)->text();
                        }

                        //QString name(ui->listWidget->item(i - 1)->text());
                        player->stop();
                        player->setMedia(QUrl::fromLocalFile(name));

                        ui->link_lab->setText("链接:" + name.section('/', -1, -1));
                        ui->supend_btn->setText("暂停");
                        ui->listWidget->setCurrentRow(i - 1);
                        player->play();
                        if(!timer->isActive())
                            timer->start(500);
                        break;
                    }
                }

            }
        }
    }
    else //顺序播放和单曲循环
    {
        if(ui->listWidget->currentRow() < 0)
        {
            QMessageBox::warning(this, "提示", "请双击列表播放");
            return;
        }
        if(ui->listWidget->count() > 0 )
        {
            for(int i = 0; i < ui->listWidget->count(); i++)
            {
                if(player->media().canonicalUrl().toString().section('/', -1, -1) == ui->listWidget->item(i)->text().section('/', -1, -1))
                {
                    if(i == 0)
                    {
                        QMessageBox::warning(this, "提示", "已到顶了");
                        return;
                    }
                    QString name(ui->listWidget->item(i - 1)->text());
                    player->setMedia(QUrl::fromLocalFile(name));
                    player->play();
                    ui->link_lab->setText("链接:" + name.section('/', -1, -1));
                    ui->supend_btn->setText("暂停");
                    ui->listWidget->setCurrentRow(i - 1);
                    if(!timer->isActive())
                        timer->start(500);
                    break;
                }
            }
        }

    }
}

/************************************ 下一首 ******************************************/

void MainWindow::click_next()//下一首
{
    if(ui->mode_btn->text() == tr("随机播放"))
    {
        qDebug()<<"随机播放"<<ui->listWidget->currentRow()<<ui->listWidget->count();
        if(ui->listWidget->count() > 0)
        {
            int num = rand()%ui->listWidget->count();
            QString string = ui->listWidget->item(num)->text();
            player->setMedia(QUrl::fromLocalFile(string));
            player->play();
            ui->link_lab->setText("链接:" + string.section('/', -1, -1));
            ui->supend_btn->setText("暂停");
            ui->listWidget->setCurrentRow(num);
            if(!timer->isActive())
                timer->start(500);

        }
        else
            QMessageBox::warning(this, "提示", "请添加歌曲");
    }
    else if(ui->mode_btn->text() == tr("列表循环"))
    {
        if(ui->listWidget->count() > 0)
        {
            if(ui->listWidget->currentRow() < 0) //从第一首开始播放
            {
                player->setMedia(QUrl::fromLocalFile(ui->listWidget->item(0)->text()));
                ui->link_lab->setText("链接:"+ui->listWidget->item(0)->text().section('/', -1, -1));
                ui->supend_btn->setText("暂停");
                ui->listWidget->setCurrentRow(ui->listWidget->currentRow() + 1);
                player->play();
                if(!timer->isActive())
                    timer->start(500);
            }
            else    //在选择中
            {

                for(int i = 0; i < ui->listWidget->count(); i++)
                {

                    //从最后一首的歌曲去除其它杂项获取到歌曲的名字,等E://
                    if(player->media().canonicalUrl().toString().section('/', -1, -1) == ui->listWidget->item(i)->text().section('/', -1, -1))
                    {
                        //QString name;

                       // qDebug()<<"pleyer"<<player->media().canonicalUrl().toString().section('/', -1, -1);
                       // qDebug()<<"list" << ui->listWidget->item(i)->text().section('/', -1, -1);
                        if(i == ui->listWidget->count() - 1)
                        {
                            i = -1;
                            QString fname = ui->listWidget->item(i + 1)->text();
                            player->setMedia(QUrl::fromLocalFile(fname));

                            ui->listWidget->setCurrentRow(i + 1);

                            ui->link_lab->setText("链接:" + fname.section('/', -1, -1));
                            ui->supend_btn->setText("暂停");
                            player->play();
                            if(!timer->isActive())
                                timer->start(500);
                             qDebug()<<"name"<<fname;
                        }
                        else
                        {
                            QString fname = ui->listWidget->item(i + 1)->text();
                            player->setMedia(QUrl::fromLocalFile(fname));

                            ui->listWidget->setCurrentRow(i + 1);

                            ui->link_lab->setText("链接:" + fname.section('/', -1, -1));
                            ui->supend_btn->setText("暂停");
                            player->play();
                            if(!timer->isActive())
                                timer->start(500);
                             qDebug()<<"name"<<fname;
                        }


                        qDebug()<<ui->listWidget->currentRow();
//                        player->setMedia(QUrl::fromLocalFile(name));

//                        ui->listWidget->setCurrentRow(i + 1);
                        qDebug()<<"ai "<<i + 1;
//                        ui->link_lab->setText("链接:" + name.section('/', -1, -1));
//                        ui->supend_btn->setText("暂停");
//                        player->play();
//                        if(!timer->isActive())
//                            timer->start(500);
                        return;
                    }

                }

            }
        }
        else
        {
            QMessageBox::warning(this, "提示", "请添加歌曲");
        }
    }
    else//顺序播放和单曲播放的下一首
    {
        if(ui->listWidget->count() > 0)//列表的数量
        {
            if(ui->listWidget->currentRow() < 0)//从第一首开始播放
            {
                player->setMedia(QUrl::fromLocalFile(ui->listWidget->item(0)->text()));
                ui->link_lab->setText("链接:"+ui->listWidget->item(0)->text().section('/', -1, -1));
                ui->supend_btn->setText("暂停");
                ui->listWidget->setCurrentRow(ui->listWidget->currentRow() + 1);
                player->play();
                if(!timer->isActive())
                    timer->start(500);
            }
            else//下一首
            {

                for(int i = 0; i < ui->listWidget->count(); i++)
                {
                    if(player->media().canonicalUrl().toString().section('/', -1, -1) == ui->listWidget->item(i)->text().section('/', -1, -1))
                    {
                        if(i == ui->listWidget->count() - 1)
                        {
                            QMessageBox::warning(this, "提示","已到底了");
                            return;
                        }
                        QString name = ui->listWidget->item(i + 1)->text();
                        player->setMedia(QUrl::fromLocalFile(name));

                        ui->supend_btn->setText("暂停");
                        ui->listWidget->setCurrentRow(i + 1);
                        ui->link_lab->setText("链接:" + name.section('/', -1, -1));
                        player->play();
                        if(!timer->isActive())
                            timer->start(500);
                        break;
                    }
                }

            }

        }

        else
        {
            QMessageBox::warning(this, "提示", "请添加歌曲");
        }
    }

}

//设置静音
void MainWindow::click_mute(bool value)
{
    ui->volume_sli->setEnabled(!value);
    player->setMuted(value);
}
//设置音量
void MainWindow::changevulume(int value)
{
    //qDebug()<<value;
    ui->volume_lab->setText("音量:" + QString::number(value));
    player->setVolume(value);

}
