#include <qglobal.h>

#ifdef Q_OS_ANDROID
#include "androidutils.h"

AndroidUtils::AndroidUtils()
{

}

bool AndroidUtils::checkPermission(QString strPermission)
{
    auto result = QtAndroidPrivate::checkPermission(strPermission).then([](
            QtAndroidPrivate::PermissionResult result) {
                return result;
            });

    result.waitForFinished();
    return result.result() != QtAndroidPrivate::PermissionResult::Denied;
}

bool AndroidUtils::requestPermission(QString strPermission)
{
    auto result = QtAndroidPrivate::requestPermission(strPermission);
    return result.result() != QtAndroidPrivate::PermissionResult::Denied;
}

void AndroidUtils::pickFile(ActivityType fileType, QString filter, QAndroidActivityResultReceiver *resultReceiver)
{
    QJniObject ACTION_OPEN_DOCUMENT = QJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_OPEN_DOCUMENT");

    jint FLAG_READ_PERMISSION = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    jint FLAG_PERSISTABLE_PERMISSION = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_PERSISTABLE_URI_PERMISSION");

    QJniObject intent("android/content/Intent");
    if (ACTION_OPEN_DOCUMENT.isValid() && intent.isValid())
    {
        intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;", ACTION_OPEN_DOCUMENT.object<jstring>());
        intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", QJniObject::fromString(filter).object<jstring>());

        intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", FLAG_READ_PERMISSION);
        intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", FLAG_PERSISTABLE_PERMISSION);

        QtAndroidPrivate::startActivity(intent, (int)fileType, resultReceiver);
    }
}

void AndroidUtils::bindProcessToNetwork()
{
    QJniObject::callStaticMethod<void>(
        "com.fmone.utils/NetUtils", "bindToWiFi",
        "(Landroid/content/Context;)V",
        QNativeInterface::QAndroidApplication::context());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        qDebug() << "Java exception";
        env->ExceptionClear();
    }
}

#endif
