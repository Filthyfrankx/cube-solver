#ifndef LOG_H
#define LOG_H

#include <QDateTime>
#include <QString>

class Log
{
public:
    Log();

public:
    // 开启时间
    QDateTime startTime;
    // 保存目录
    QString m_dir;
    // 文件名
    QString m_filename;

private:
    // 检查日志目录
    void checkLogDir();

public:
    // 保存日志
    void saveLog(const QString & log);
    // 结束日志
    void endLog();

};

#endif // LOG_H
