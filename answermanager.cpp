#include "answermanager.h"

AnswerManager::AnswerManager(QObject *parent)
    : QObject{parent}
{}

void AnswerManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    emit sgDataUpdated(frameType, dataType, data);
}

void AnswerManager::slNetEvent(NetEvents eventType, QString target, QVariantList data)
{
    emit sgNetEvent(eventType, target, data);
}
