#include "log.h"

namespace yolo {

const char* LogLevel::ToStirng(LogLevel::Level level){
    switch (level)
    {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    
    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

Logger::Logger(const std::string& name):m_name(name) {
}

void Logger::addAppend(LogAppender::ptr appender)
{
    m_appenders.push_back(appender);
}

void Logger::delAppend(LogAppender::ptr appender)
{
    for (auto it = m_appenders.begin(); it!=m_appenders.end(); ++it)
    {
        if(*it == appender)
        {
            m_appenders.erase(it);
            break;
        }
    }
}


void Logger::log(LogLevel::Level level,LogEvent::ptr event)
{
    if (level >= m_level)
    {
        for(auto& i : m_appenders)
        {
            i->log(level,event);
        }
    }
}

void Logger::debug(LogEvent::ptr event)
{
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event)
{
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event)
{
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event)
{
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event)
{
    log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string& filename):m_filename(filename)
{
}

bool FileLogAppender::reopen()
{
    if(m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    //  !!转换数据类型
    return !!m_filestream;

}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        m_filestream << m_formatter->format(level, event);
    }
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level) 
    {
        std::cout << m_formatter->format(level, event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){
}

std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items) {
        i->format(ss, level, event);
    }
    return ss.str();
}

//%xxx %xxx{xxx} %%
void LogFormatter::init()
{
    //std,format,type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for (size_t i=0; i < m_pattern.size(); ++i)
    {
        if(m_pattern[i] != '%')
        {
            nstr.append(1,m_pattern[i]);
            continue;
        }

        if((i+1) < m_pattern.size()) {
            if (m_pattern[i+1] == '%') {
                nstr.append(1,'%');
                continue;
            }
        }


        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size())
        {
            if(isspace(m_pattern[n]))
            {
                break;
            }

            if(fmt_status == 0)
            {
                if(m_pattern[n] == '{')
                {
                    str = m_pattern.substr(i+1,n-i-1);
                    fmt_status = 1; //解析格式
                    ++n;
                    fmt_begin = n;
                    continue;
                }
            }

            if(fmt_status == 1)
            {
                if(m_pattern[n] == '}')
                {
                    fmt = m_pattern.substr(fmt_begin + 1,n-fmt_begin-1);
                    fmt_status = 2;
                    ++n;
                    break;
                }
            }
        }
        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            str = m_pattern.substr(i+1, n-i-1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }else if(fmt_status ==1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 1));
        }else if(fmt_status ==2) {
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
    }
    // %m -- 消息体
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程ID
    // %n -- 回车
    // %p -- level
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
}

class MessageFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToStirng(level);
    }
};

class ElapseLevelFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};


}