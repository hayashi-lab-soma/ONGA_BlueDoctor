#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QSettings>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);

    bool getBool(QString group, QString key);
    QString getString(QString group, QString key);
    int getInteger(QString group, QString key);
    double getDouble(QString group, QString key);

    void setInteger(QString group, QString key, int value);

signals:

public slots:

private:
    QSettings *config;
};

#endif // CONFIG_H
