#include "sysconfig.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDir>

SysConfig* SysConfig::m_pConfig = nullptr;
QMutex SysConfig::mutex;

SysConfig::SysConfig(QObject *parent) : QObject(parent)
{

}

SysConfig* SysConfig::getInstance()
{
    if(m_pConfig == nullptr)
    {
        mutex.lock();
        if(m_pConfig == nullptr)
        {
            m_pConfig = new SysConfig();
        }
        mutex.unlock();
    }
    return m_pConfig;
}

void SysConfig::setPath(QString path)
{
    m_strPath = path;
}

QString SysConfig::getPath()
{
    return m_strPath;
}

void SysConfig::readConfig()
{
    QFile file(QCoreApplication::applicationDirPath() + "/config.json");
    if(!file.open(QFile::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if(!doc.isObject())
        return;

    QJsonObject obj = doc.object();
    if(obj.contains("Path"))
        m_strPath = obj["Path"].toString();
    else
        m_strPath = "";

}

void SysConfig::writeConfig()
{
    QJsonObject obj;
    obj.insert("Path",QJsonValue(m_strPath));

    QJsonDocument doc(obj);
    QFile file(QCoreApplication::applicationDirPath() + "/config.json");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        return;

    file.write(doc.toJson());
}
