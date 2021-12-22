#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QMutex>

class TerminalThread : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit TerminalThread();
    ~TerminalThread();

public slots:
    void Command(const QString& strCmd, const bool& bWaitForFinished);

signals:
    void start_command(const QString& strCmd, const bool& bWaitForFinished);
    void readyReadStandardOutput(const QString& strStandardOutput);
    void readyReadStandardError(const QString& strStandardError);
    void finished();
};

class TerminalWorker : public QObject
{
    Q_OBJECT
public slots:
    void doWork(const QString& strCmd, const bool& bWaitForFinished);

signals:
    void readyReadStandardOutput(const QString& strStandardOutput);
    void readyReadStandardError(const QString& strStandardError);
    void finished();
};

class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = nullptr);

public:
    void execute(const QString& strCmd, const bool& bWaitForFinished = true);

signals:
    void readyReadStandardOutput(const QString& strStandardOutput);
    void readyReadStandardError(const QString& strStandardError);
    void finished();

private slots:
    void on_readoutput();
    void on_readerror();

private:
    void initialize();

private:
    QString m_strPwd;
    static QMutex m_mutex;
    static QProcess *m_pCmd;
};

#endif // TERMINAL_H
