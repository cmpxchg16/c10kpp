#include <mutex>
#include <condition_variable>
#include <queue>

class InterruptedException : public std::runtime_error {
public:
  explicit InterruptedException () : std::runtime_error("InterruptedException") {}
};

template <typename T>
class BlockingQueue
{

public:
	
	BlockingQueue() : interrupted_(false) {}
		
	T pop()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty())
		{
			cond_var_.wait(lock);
			if (interrupted_) 
			{
				throw InterruptedException();
			}
		}
		auto t = queue_.front();
		queue_.pop();
		return t;
	}
	
	void push(const T& t)
    {
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(t);
		lock.unlock();
		cond_var_.notify_one();
	}
	
	void interrupt()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		interrupted_ = true;
		cond_var_.notify_all();
	}
 
private:

	std::mutex mutex_;
	std::condition_variable cond_var_;
	std::queue<T> queue_;
	bool interrupted_;
};
