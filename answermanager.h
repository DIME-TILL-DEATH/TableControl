#ifndef ANSWERMANAGER_H
#define ANSWERMANAGER_H

#include <QObject>


#include "netmanager.h"

class AnswerManager : public QObject
{
    Q_OBJECT
public:
    explicit AnswerManager(QObject *parent = nullptr);

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void slNetEvent(NetEvents eventType, QString target, QVariantList data = {});

signals:
    void sgDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});
};

#endif // ANSWERMANAGER_H
