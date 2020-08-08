#ifndef READTHREAD_H
#define READTHREAD_H
#include<QThread>

class ReadThread : public QThread
{
public:
    ReadThread();
protected:
    void run() override;
private:

};

#endif // READTHREAD_H
