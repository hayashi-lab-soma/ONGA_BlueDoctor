#include "config.h"

/*!
 * \brief Config::Config
 * \param parent
 */
Config::Config(QObject *parent) : QObject(parent)
{
    //iniファイルの読み込み
    config = new QSettings("../resource/config.ini",
                           QSettings::IniFormat,
                           this);
}

bool Config::getBool(QString group, QString key)
{
    config->beginGroup(group);
    bool value = config->value(key).toBool();
    config->endGroup();
    return value;
}

QString Config::getString(QString group, QString key)
{
    config->beginGroup(group);
    QString value = config->value(key).toString();
    config->endGroup();
    return value;
}

int Config::getInteger(QString group, QString key)
{
    config->beginGroup(group);
    int value = config->value(key).toInt();
    config->endGroup();
    return value;
}

double Config::getDouble(QString group, QString key)
{
    config->beginGroup(group);
    double value = config->value(key).toDouble();
    config->endGroup();
    return value;
}

void Config::setInteger(QString group, QString key, int value)
{
    config->beginGroup(group);
    config->setValue(key, value);
    config->endGroup();
    return;
}
