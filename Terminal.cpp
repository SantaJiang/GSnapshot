#include "Terminal.h"
#include <QEventLoop>

/**
 * @brief TerminalThread
 */
TerminalThread::TerminalThread()
{
    TerminalWorker *worker = new TerminalWorker();
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, SIGNAL(readyReadStandardOutput(const QString&)), this, SIGNAL(readyReadStandardOutput(const QString&)));
    connect(worker, SIGNAL(readyReadStandardError(const QString&)), this, SIGNAL(readyReadStandardError(const QString&)));
    connect(worker, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(this, SIGNAL(start_command(const QString&, const bool&)), worker, SLOT(doWork(const QString&, const bool&)));
    workerThread.start();
}

TerminalThread::~TerminalThread()
{
    workerThread.quit();
    workerThread.wait();
}

void TerminalThread::Command(const QString &strCmd, const bool &bWaitForFinished)
{
    emit start_command(strCmd, bWaitForFinished);
}

/**
 * @brief TerminalWorker
 */
void TerminalWorker::doWork(const QString &strCmd, const bool &bWaitForFinished)
{
    Command *ti = new Command();
    connect(ti, SIGNAL(readyReadStandardOutput(const QString&)), this, SIGNAL(readyReadStandardOutput(const QString&)));
    connect(ti, SIGNAL(readyReadStandardError(const QString&)), this, SIGNAL(readyReadStandardError(const QString&)));
    connect(ti, SIGNAL(finished()), this, SIGNAL(finished()));
    ti->execute(strCmd, bWaitForFinished);
}

/**
 * @brief Command
 */
QMutex Command::m_mutex;
QProcess* Command::m_pCmd = nullptr;
Command::Command(QObject *parent) : QObject(parent)
{
    if (m_pCmd == nullptr)
    {
        m_pCmd = new QProcess(this);
        connect(m_pCmd , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
        connect(m_pCmd , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));
        initialize();
    }
}

void Command::initialize()
{   
#if defined Q_OS_WIN

    m_pCmd->start("cmd");
    m_pCmd->waitForStarted();

#elif defined Q_OS_LINUX

    m_pCmd->start("bash");
    m_pCmd->waitForStarted();

#elif defined Q_OS_MACOS

    m_pCmd->start("bash");
    m_pCmd->waitForStarted();

#endif
}

void Command::execute(const QString &strCmd, const bool &bWaitForFinished)
{
    m_mutex.lock();

#if defined Q_OS_LINUX

    emit readyReadStandardOutput(strCmd);
    QString strTermCmd = strCmd;
    bool bSudo = strTermCmd.contains("sudo ");

    if (bSudo)
    {
        //去掉"sudo "
        strTermCmd.replace("sudo ", "sudo -S ");
    }
    strTermCmd += '\n';
    m_pCmd->write(strTermCmd.toUtf8().data());

#elif defined Q_OS_WIN

    QString strTermCmd = strCmd + '\n';
    m_pCmd->write(strTermCmd.toUtf8().data());

#elif defined Q_OS_MACOS

    QString strTermCmd = strCmd + '\n';
    m_pCmd->write(strTermCmd.toUtf8().data());

#endif

    if (bWaitForFinished)
    {
        m_pCmd->waitForFinished();
    }

    m_mutex.unlock();
    emit finished();
}

void Command::on_readoutput()
{
    QString strInfo = QString::fromLocal8Bit(m_pCmd->readAllStandardOutput().data());
    emit readyReadStandardOutput(strInfo);
}

void Command::on_readerror()
{
    QString strErr = QString::fromLocal8Bit(m_pCmd->readAllStandardOutput().data());
    emit readyReadStandardError(strErr);
}
