#include "registerform.h"
#include "ui_registerform.h"
#include <QJsonDocument>
#include<QJsonObject>
#include <QRegExpValidator>
#include<string>
#include<QDebug>
#include <QString>
#include <QMessageBox>
RegisterForm::RegisterForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterForm)
{
    ui->setupUi(this);
    //只能输入数字
    writeClient = WriteClient::getTcpSocketClient();
    ui->accountLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{3,6}$")));
    ui->usernameLineEdit->setValidator(new QRegExpValidator(QRegExp("[\u4e00-\u9fa5a-zA-Z]{1,9}$")));
    ui->passwordLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-zA-Z]{6,16}$")));
    connect(ui->submitPushButton, &QPushButton::clicked, this, [=](){
        QString account = ui->accountLineEdit->text();
        if(account.length() < 3){
            QMessageBox::information(this,"注册提示","账号长度应为3-6位");
            return;
        }
        if(account.toInt() > 65535){
            QMessageBox::information(this,"注册提示","账号数字不能超过65535");
            return;
        }
        QString username = ui->usernameLineEdit->text();
        if(username.length() < 3){
            QMessageBox::information(this,"注册提示","昵称长度应为3-9位");
            return;
        }
        QString password = ui->passwordLineEdit->text();
        if(password.length() < 6){
            QMessageBox::information(this,"注册提示","密码长度应为6-16位");
            return;
        }
        QJsonObject information;
        information.insert("account", account);
        information.insert("username", username);
        information.insert("password", password);
        QJsonDocument doc;
        doc.setObject(information);
        QString s = doc.toJson(QJsonDocument::Compact);
        writeClient->writeText(0, s.toStdString(), REGISTER);
        writeClient->getTcpClient()->waitForReadyRead(-1);
        QJsonObject serverInfoObj = writeClient->readServerMsg();
        int status = serverInfoObj["status"].toInt();
        if(status == REGISTER_SUCCESS){
            QMessageBox::information(this,"注册提示","注册成功");
            this->close();
        }
        else if(status == REGISTER_FAIL){
            QMessageBox::information(this,"注册提示","账号已存在, 请填写其它账号");
        }
    });
}

RegisterForm::~RegisterForm()
{
    delete ui;
}

void RegisterForm::closeEvent ( QCloseEvent * e )
{
    this->loginForm->show();
    //this->close();
}
