#include <qdebug.h>
#include <QFile>
#include <QStandardPaths>

#include <QCoreApplication>

#ifdef Q_OS_ANDROID
#include <jni.h>

#include <QtCore/private/qandroidextras_p.h> //
#include "androidutils.h"

#include "activityresultmanager.h"


ActivityResultManager::ActivityResultManager()
{

}

void ActivityResultManager::handleActivityResult(int receiverRequestCode, int resultCode, const QJniObject &data)
{
    jint RESULT_OK = QJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");

    if(resultCode == RESULT_OK)
    {
        switch((ActivityType)receiverRequestCode)
        {
        case ActivityType::PICK_FILE:
            {
                QJniObject uriObject = data.callObjectMethod("getData", "()Landroid/net/Uri;");
                takeReadUriPermission(uriObject);
                processUri(uriObject);

                qDebug() << "Gcode file path: " << m_filePath;
                qDebug() << "Gcode file name: " << m_fileName;

                emit sgFilePicked(m_filePath, m_fileName);
                break;
            }

            default:
            {
                qDebug() << __FUNCTION__ << "Unknown activity result";
            }
        }
    }
    else
    {
        qDebug() << __FUNCTION__ << "Activity result not OK. Code: " << resultCode;
    }
}

void ActivityResultManager::processUri(QJniObject uriObject)
{

    m_filePath = uriObject.toString();
//    m_filePath.replace("%2F", "/");
//    m_filePath.replace("%3A", ":");

    m_fileName= QJniObject::callStaticObjectMethod(
            "com.fmone.utils/FileUtils", "getFileName",
            "(Landroid/net/Uri;Landroid/content/Context;)Ljava/lang/String;",
            uriObject.object(),
            QNativeInterface::QAndroidApplication::context()).toString();
}

void ActivityResultManager::takeReadUriPermission(QJniObject uriObject)
{
    bool result = AndroidUtils::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    if(!result)
    {
        qDebug() << "READ_EXTERNAL_STORAGE permission denied, trying to request";
         AndroidUtils::requestPermission("android.permission.READ_EXTERNAL_STORAGE");
    }

    QJniObject::callStaticMethod<void>(
            "com.fmone.utils/FileUtils", "takeReadUriPermission",
            "(Landroid/net/Uri;Landroid/content/Context;)V",
            uriObject.object(),
            QNativeInterface::QAndroidApplication::context());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        qDebug() << "Java exception";
        env->ExceptionClear();
    }

    qDebug() << "Read URI permission taken";
}

void ActivityResultManager::takeWriteUriPermission(QJniObject uriObject)
{
    bool result = AndroidUtils::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(!result)
    {
        qDebug() << "WRITE_EXTERNAL_STORAGE permission denied, trying to request";
         AndroidUtils::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    }

    QJniObject::callStaticMethod<void>(
            "com.fmone.utils/FileUtils", "takeWriteUriPermission",
            "(Landroid/net/Uri;Landroid/content/Context;)V",
            uriObject.object(),
            QNativeInterface::QAndroidApplication::context());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        qDebug() << "Java exception";
        env->ExceptionClear();
    }

    qDebug() << "Write URI permission taken";
}

// //TODO: move to AndroidUtils
// QString ActivityResultManager::getFileNameFromUri(QString uri)
// {
//         QJniObject uriJni = QJniObject::callStaticObjectMethod(
//             "android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;",
//             QJniObject::fromString(uri).object<jstring>());

//         QString fileName = QJniObject::callStaticObjectMethod(
//                 "com.fmone.utils/FileUtils", "getFileName",
//                 "(Landroid/net/Uri;Landroid/content/Context;)Ljava/lang/String;",
//                 uriJni.object(), QNativeInterface::QAndroidApplication::context()).toString();
//         return fileName;
// }
#endif
