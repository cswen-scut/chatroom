#include "widget.h"
#include "ui_widget.h"
#include <QDir>
#include<QFileDialog>
#include <QJsonDocument>
#include<QJsonObject>
#include <QMessageBox>
#include<readthread.h>
#include<QMap>
#include <QVariantMap>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //获取客户端连接
    writeClient = WriteClient::getTcpSocketClient();
    //获取读端
    qDebug() << "开始read端1---------";
    readClient = ReadClient::getTcpSocketClient();
    //发送文本数据
    connect(ui->sendTextPushButton, &QPushButton::clicked, this, [=](){
       QString text = ui->inputTextEdit->toPlainText();
       if(text.trimmed().length() == 0){
           QMessageBox::information(this,"发送提示","不能发送空内容");
           return;
       }
       writeClient->writeText(user->getUserId(), text.toStdString());
       ui->inputTextEdit->clear();
    });
    thread = new ReadThread;
    thread->start();
    //发送图片
    connect(ui->sendImagePushButton, &QPushButton::clicked, this, [=](){
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *jpeg *.bmp)"));
        qDebug() << "文件名---------------" << fileName;
        QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
        string imagePath = code->fromUnicode(fileName).data();
        //string imagePath = fileName.toStdString();
        if(imagePath != ""){
            writeClient->writeImage(user->getUserId(), imagePath);
            qDebug() << "发送文件成功";
        }
    });

    connect(readClient, &ReadClient::textDisplayAble, this, [=](){
       ui->contentListWidget->addItem(QString::fromStdString(readClient->getTextContent()));
    });
    ui->contentListWidget->setIconSize(QSize(200,200));
    connect(readClient, &ReadClient::imageDisplayAble, this, [=](){
       QListWidgetItem * pic = new QListWidgetItem;
       pic->setIcon(QIcon(QString::fromStdString(readClient->getImagePath())));
       ui->contentListWidget->addItem(pic);
    });
    connect(ui->exitPushButton, &QPushButton::clicked, this, [=](){
         //client->getTcpClient()->waitForReadyRead(3);
         //QString logoutMsg = client->readMsg();
        // qDebug() << logoutMsg;
         thread->exit();
         this->close();
         this->loginForm->close();
    });

    connect(readClient, &ReadClient::noticeDisplayAble, this, [=](){
       ui->noticeListWidget->addItem(QString::fromStdString(readClient->getNoticeContent()));
    });
    connect(readClient, &ReadClient::onlineDisplayAble, this, [=](){
        ui->onlineListWidget->clear();
        QVariantList onlineList = QJsonDocument::fromJson(QString::fromStdString(readClient->getTextContent()).toUtf8()).toVariant().toList();
        for(auto & user: onlineList){
            QVariantMap map = user.toMap();
            QString element = map["account"].toString() + "\t" + map["username"].toString() + "\t" + map["loginTime"].toString();
            ui->onlineListWidget->addItem(element);
        }
        ui->onlineLabel->setText("当前在线人数(" + QString::number(onlineList.size()) + "人)");
    });

}

Widget::~Widget()
{
    delete ui;
    delete user;
}

void Widget::disPlayUserInfo(){
    ui->userLabel->setText(user->getUsername() + "(" + QString::number(user->getUserId()) + ")");
}

void Widget::closeEvent ( QCloseEvent * e )
{
    thread->exit();
    this->loginForm->close();
    //this->close();
}

