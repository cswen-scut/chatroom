#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include<QTcpSocket>
#include"dataencoder.h"
#include "readclient.h"
#include "user.h"
#include "writeclient.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();
    void connectServer();

private:
    Ui::LoginForm *ui;
    WriteClient * writeClient = nullptr;
    ReadClient * readClient = nullptr;
    User * user = nullptr;
    void closeEvent(QCloseEvent * e);
    void sleep(int msec);
};

#endif // LOGINFORM_H
