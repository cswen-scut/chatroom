#include "loginform.h"
#include "ui_loginform.h"
#include "widget.h"
#include"writeclient.h"
#include "readclient.h"
#include "registerform.h"

#include <QMessageBox>
#include <QTimer>

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    connect(ui->loginPushButton, &QPushButton::clicked, this, &LoginForm::connectServer);
    connect(ui->registerPushButton, &QPushButton::clicked, this, [=](){
        writeClient = WriteClient::getTcpSocketClient();
        if(writeClient == nullptr){
            qDebug() << "连接服务器超时";
            QMessageBox::information(this, "登录提示", "连接服务器超时");
            return;
        }
        RegisterForm * rf = new RegisterForm;
        rf->loginForm = this;
        rf->show();
        this->hide();
    });

}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::connectServer(){
    qDebug() << "-------------开始连接服务器----------------";
    writeClient = WriteClient::getTcpSocketClient();
    if(writeClient == nullptr){
        qDebug() << "连接服务器超时";
        QMessageBox::information(this, "登录提示", "连接服务器超时");
        return;
    }
    QString account = ui->userIdLineEdit->text();
    qDebug() << "账号:" << account;
    QString password = ui->passwordLineEdit->text();
    qDebug() << "密码:" << password;

    QJsonObject information;
    information.insert("account", account);
    information.insert("password", password);
    QJsonDocument doc;
    doc.setObject(information);
    string loginMsg =doc.toJson(QJsonDocument::Compact).toStdString();
    writeClient->writeText(account.toUtf8().toInt(), loginMsg, LOGIN);
    writeClient->getTcpClient()->waitForReadyRead(-1);
    qDebug() << "收到后台确认信息---------------------";
    QJsonObject serverInfoObj = writeClient->readServerMsg();
    qDebug() << serverInfoObj["status"];
    int status = serverInfoObj["status"].toInt();
    qDebug() << "status:" << status;
    if(status == LOGIN_SUCCESS){
        readClient = ReadClient::getTcpSocketClient();
        qDebug() << "登陆成功, 开始read端---------";
        //readClient->writeText(account, "", READ);
        Widget * widget = new Widget;
        widget->loginForm = this;
        user = new User(account.toUtf8().toInt(), serverInfoObj["username"].toString());
        widget->user = user;
        qDebug() << "account:" << account << "->" << "username:" << serverInfoObj["username"].toString();
        widget->disPlayUserInfo();
        widget->show();
        readClient->writeText(account.toUtf8().toInt(), "", READ);
        this->hide();
    }
    else if(status == LOGIN_FAIL){
        QMessageBox::information(this, "登录提示", "账号或密码错误,请重试");
    }
    else if(status == LOGIN_EXIST){
        QMessageBox::information(this, "登录提示", "该账号已登录，不可重复登录");
    }
}

void LoginForm::closeEvent ( QCloseEvent * e )
{
    qDebug() << "loginform关闭了";
    int userId = user != nullptr? user->getUserId():0;

    if(readClient != nullptr){
        qDebug() << "readClient关闭了";
        writeClient->writeText(userId, "", LOGOUT);
        //确保登出信息能够及时发到服务端
        sleep(300);
        writeClient->getTcpClient()->disconnectFromHost();
        sleep(100);
        readClient->getTcpClient()->disconnectFromHost();
        sleep(100);
    }
    else if(writeClient != nullptr){
        qDebug() << "writeClient关闭了";
        //确保关闭信息能够及时发到服务端
        writeClient->writeText(userId, "", CLOSE);
        sleep(300);
        writeClient->getTcpClient()->disconnectFromHost();
        sleep(100);
    }
}

void LoginForm::sleep(int msec){
    QEventLoop eventloop;
    QTimer::singleShot(msec, &eventloop, SLOT(quit()));
    eventloop.exec();
}


