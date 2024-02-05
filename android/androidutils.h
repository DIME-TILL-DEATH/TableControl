#ifndef ANDROIDUTILS_H
#define ANDROIDUTILS_H

#include <qglobal.h>
#ifdef Q_OS_ANDROID

#include "activityresultmanager.h"

//#include <QtAndroidExtras>
#include <QtCore/private/qandroidextras_p.h>

#include <QString>

class AndroidUtils
{
public:
    AndroidUtils();

    static bool checkPermission(QString strPermission);
    static bool requestPermission(QString strPermission);

    static void pickFile(ActivityType fileType, QString filter, QAndroidActivityResultReceiver *resultReceiver);

    static void bindProcessToNetwork();
};
#endif

#endif // ANDROIDUTILS_H
