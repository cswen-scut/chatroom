#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "writeclient.h"
#include "loginform.h"
#include "readclient.h"
#include "readthread.h"
#include "user.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void disPlayUserInfo();
    LoginForm * loginForm;
    User * user = nullptr;

private:
    Ui::Widget *ui;
    WriteClient * writeClient = nullptr;
    ReadClient * readClient = nullptr;
    void closeEvent(QCloseEvent * e);
    ReadThread *thread;
};
#endif // WIDGET_H
