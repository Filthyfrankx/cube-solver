#include "log.h"
#include <QDir>
#include <QFile>

Log::Log()
{
    startTime = QDateTime::currentDateTime();
    m_dir = QDir::currentPath() + "/log";
    m_filename = "log_" + startTime.toString("yyyyMMdd_HHmmss") + ".log";
    checkLogDir();
    saveLog(startTime.toString("程序开启时间：yyyy-MM-dd HH:mm:ss\n\n"));
}

// 检查日志目录
void Log::checkLogDir()
{
    QDir dir;
    if (!dir.exists(m_dir)) dir.mkdir(m_dir);
}

// 保存日志
void Log::saveLog(const QString &log)
{
    QFile logFile(m_dir + '/' + m_filename);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    logFile.close();
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    logFile.write(log.toLocal8Bit());
    logFile.close();
}

// 结束日志
void Log::endLog()
{
    QDateTime endTime = QDateTime::currentDateTime();
    saveLog(endTime.toString("\n程序关闭时间：yyyy-MM-dd HH:mm:ss"));
    qint64 usingtime = endTime.msecsTo(startTime);
    saveLog(QString("\n程序使用时间：%1 时 %2 分 %3 秒 %4 毫秒")
            .arg(usingtime / 3600000)
            .arg((usingtime / 60000) % 60)
            .arg((usingtime / 1000) % 60)
            .arg(usingtime % 1000));
}
