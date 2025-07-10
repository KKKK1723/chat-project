#ifndef SINGLETON_H
#define SINGLETON_H
#include"global.h"

template<typename T>
class Singleton
{
protected:
    Singleton()=default;//允许子类构造
    //禁止拷贝构造
    Singleton(const Singleton<T>&a)=delete;
    //禁止拷贝赋值
    Singleton & operator=(const Singleton<T>&a)=delete;


public:
    //获取实例
    static std::shared_ptr<T>& GetInstance()
    {
        static std::shared_ptr<T> instance=std::shared_ptr<T>(new T);
        return instance;
    }

    ~Singleton()
    {
        std::cout<<"this is Singleton destruct"<<std::endl;
    }
};

#endif // SINGLETON_H

