#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include "loginform.h"
#include "writeclient.h"

#include <QWidget>

namespace Ui {
class RegisterForm;
}

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterForm(QWidget *parent = nullptr);
    ~RegisterForm();
    LoginForm * loginForm;

private:
    Ui::RegisterForm *ui;
    WriteClient *writeClient;
    void closeEvent(QCloseEvent * e);
};

#endif // REGISTERFORM_H
