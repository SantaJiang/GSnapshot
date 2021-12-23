﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Terminal.h"
#include "trayicon.h"
#include <QTime>
#include <QThread>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_timer(nullptr)
{
    ui->setupUi(this);
    initForm();
}

MainWindow::~MainWindow()
{
    delete ui;
    TrayIcon::Instance()->setVisible(false);
    exit(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() != QEvent::WindowStateChange)
        return;

    if(this->windowState() == Qt::WindowMinimized)
    {
        if (TrayIcon::Instance()->getVisible())
        {
            TrayIcon::Instance()->setVisible(false);
        }
        else
        {
            TrayIcon::Instance()->setVisible(true);
            TrayIcon::Instance()->showMessage(this->windowTitle(), "已经最小化到托盘,双击打开!");
        }
        this->hide();
        return;
    }

    event->accept();
}

void MainWindow::initForm()
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(16);
    effect->setColor(Qt::gray);
    effect->setOffset(0);
    ui->widget->setGraphicsEffect(effect);

    //设置托盘
    TrayIcon::Instance()->setIcon("://logo.ico");
    TrayIcon::Instance()->setToolTip(this->windowTitle());
    TrayIcon::Instance()->setMainWidget(this);
    TrayIcon::Instance()->setVisible(true);
    TrayIcon::Instance()->setVisible(false);
}

void MainWindow::readyReadStandardOutput(const QString &str)
{
    qDebug()<<__FUNCTION__<<str;
}

void MainWindow::readyReadStandardError(const QString &str)
{
    qDebug()<<__FUNCTION__<<str;
}

void MainWindow::commit()
{
    if(m_timer->interval() == 300)
        m_timer->setInterval(ui->spinBox->value() * 60 * 1000);

    TerminalThread* thread = new TerminalThread();
    connect(thread, SIGNAL(readyReadStandardOutput(const QString&)), this, SLOT(readyReadStandardOutput(const QString&)));
    connect(thread, SIGNAL(readyReadStandardError(const QString&)), this, SLOT(readyReadStandardError(const QString&)));

    thread->Command("git add -A",false);
    QString commit = QString("git commit -m \"%1\"").arg(QTime::currentTime().toString());

    thread->Command(commit,true);
}

void MainWindow::on_pushButton_select_clicked()
{
    QString itemName = QFileDialog::getExistingDirectory(this, "Please select an item");
    if (itemName.isNull())
        return;

    ui->lineEdit_address->setText(itemName);
}

void MainWindow::on_pushButton_ok_clicked(bool checked)
{
    if(ui->lineEdit_address->text().isEmpty())
        return;

    if(checked)
    {
        TerminalThread* thread = new TerminalThread();
        QString str = QString("cd %1").arg(ui->lineEdit_address->text());
        thread->Command(str,false);

        if(m_timer == nullptr)
        {
            m_timer = new QTimer(this);
            connect(m_timer, SIGNAL(timeout()), this, SLOT(commit()));
        }
        m_timer->setInterval(300);
        m_timer->start();
        ui->pushButton_ok->setStyleSheet("background-color: rgb(225, 127, 127);"
                                         "color: rgb(255, 255, 255);"
                                         "border-radius: 4px;");
        ui->pushButton_ok->setText("Executing");
    }
    else
    {
        if(m_timer->isActive())
            m_timer->stop();

        ui->pushButton_ok->setStyleSheet("background-color: rgb(95, 84, 152);"
                                         "color: rgb(255, 255, 255);"
                                         "border-radius: 4px;");
        ui->pushButton_ok->setText("OK");
    }
}
