#pragma once
#include <thread>
#include "IContentWriter.h"
#include "../util/ThreadSafeQueue.h"

class BaseContentWriter : IContentWriter
{
public:
    BaseContentWriter()
    {
        _queue = new ThreadSafeQueue<std::string>();
    };
    ~BaseContentWriter()
    {
        delete _queue;
    };
    //---- Write to Media
    virtual bool WriteToMedia(std::string logcontent) = 0;

    bool Flush()
    {
        std::string content;
  
        while (!_queue->empty())
        {
            content = std::string(_queue->pop());
            //--- Write to Appropriate Media
            //--- Calls the Overriden method
            WriteToMedia(content);
        }
        return true;
    }

    bool Write(std::string content)
    {
        _queue->push(content);
        if (_queue->size() <= 10)
            return true;

        // Do flush in a separate thread
        // std::lock_guard<std::mutex> lock(mutex_);
        // std::thread t1(&BaseContentWriter::Flush, this);
        // t1.detach();

        return true;
    }

    bool Empty() {
        return _queue->size() == 0;
    }

private:
    ThreadSafeQueue<std::string> *_queue;
    mutable std::mutex mutex_;
};
