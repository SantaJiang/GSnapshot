#ifndef SYSCONFIG_H
#define SYSCONFIG_H

#include <QMutex>
#include <QObject>

class SysConfig : public QObject
{
    Q_OBJECT
protected:
    explicit SysConfig(QObject *parent = nullptr);

public:
    static SysConfig* getInstance();
    void setPath(QString path);
    QString getPath();

    void readConfig();
    void writeConfig();

private:
    static QMutex mutex;
    static SysConfig* m_pConfig;
    QString m_strPath;
};

#endif // SYSCONFIG_H
