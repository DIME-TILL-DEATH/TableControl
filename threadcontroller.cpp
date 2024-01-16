#include "threadcontroller.h"

ThreadController::ThreadController(QThread *mainThread)
    : QObject(nullptr),
    m_mainThread{mainThread}
{
    m_backendThread = new QThread();

    m_backendThread->setObjectName("Backend thread");
    m_backendThread->start();
}

ThreadController::~ThreadController()
{
    m_backendThread->quit();
    m_backendThread->wait();
}

QThread *ThreadController::mainThread() const
{
    return m_mainThread;
}

QThread *ThreadController::backendThread() const
{
    return m_backendThread;
}


