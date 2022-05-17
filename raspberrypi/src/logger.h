#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QDir>

class Logger
{
public:
    std::string save_dir;
    std::time_t time_delay;
    Logger(const std::string config_file, std::string filename);
    void connect_logger(const QObject *receiver, const char* slot);
    void start_logging();
    void saving_to_file(std::string filename, std::string content);

private:
    QTimer* logger_timer;
    std::time_t logging_period;
    void load_logger_settings(const std::string config_file);


};

#endif // LOGGER_H
