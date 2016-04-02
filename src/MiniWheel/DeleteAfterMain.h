

LeakDetector<T>
Singleton<T>

DeleteAtShutdown: 
任何继承自本类的子类的实例都会在main函数结束后自动按与构造函数相反的顺序析构,除非
在此之前用户手动调用了delete.(本函数无锁,线程安全)

class DeleteAtShutdown
{
public:
    DeleteAtShutdown();
    virtual ~DeleteAtShutdown();
private:
    DeleteAtShutdown* next;
};