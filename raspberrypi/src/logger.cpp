#include "logger.h"
#include <QDir>

Logger::Logger(const std::string config_file, std::string filename)
{
    logger_timer = new QTimer();
    load_logger_settings(config_file);
    save_dir = filename+"/"+save_dir+"/";
    QDir dir(QString::fromStdString(save_dir));
    if(!dir.exists()){
        dir.mkpath(".");
    }
}

/**
 * @brief Logger::load_logger_settings: load logger settings from config file
 * @param config_file
 */
void Logger::load_logger_settings(const std::string config_file){
    std::ifstream cfg_file(config_file);
    if (cfg_file.is_open()){
        std::string line;
        while (std::getline(cfg_file, line)){
            if(line[0] == '#' || line.empty())
                continue;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            if(name == "time_delay"){
                time_delay = decltype(std::time(nullptr))(stoi(value));
            }
            else if(name == "logging_period"){
                logging_period = decltype(std::time(nullptr))(stoi(value));
            }
            else if(name == "folder"){
                save_dir = value;

            }
        }
        cfg_file.close();
    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
}


/**
 * @brief Logger::start_logging: start the logger's timer
 */
void Logger::start_logging(){
    logger_timer->start(logging_period); // start the logger
}

/**
 * @brief Logger::saving_to_file: save content into file
 * @param filename
 * @param content
 */
void Logger::saving_to_file(std::string filename, std::string content){
    QFile myfile(QString::fromStdString(filename));
    if (myfile.open(QIODevice::Append)) {
        myfile.write((char*)&content[0], content.length());
        qDebug()<<"write to "<<QString::fromStdString(filename);
    }
}

/**
 * @brief Logger::connect_logger: connect timer interrupt with logger
 * @param receiver
 * @param slot
 */
void Logger::connect_logger(const QObject *receiver, const char *slot){
    QObject::connect(logger_timer, SIGNAL(timeout()), receiver, slot);
}
