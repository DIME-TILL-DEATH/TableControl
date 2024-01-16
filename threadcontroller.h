#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <QObject>
#include <QThread>

class ThreadController:public QObject
{
    Q_OBJECT
public:
    ThreadController(QThread* mainThread);
    ~ThreadController();
    QThread *mainThread() const;
    QThread *backendThread() const;

private:
    QThread* m_mainThread;
    QThread* m_backendThread;
};

#endif // THREADCONTROLLER_H
