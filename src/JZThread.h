#pragma once
#include <thread>
#include <string>
#include <atomic>
#include <functional>
#include <memory>

using std::string;
using std::thread;
using std::atomic;

using ThreadFunc = std::function<void()>;
using ThreadPtr = std::shared_ptr<thread>;

class JZThread
{
public:
	JZThread(const JZThread&) = delete;
	JZThread& operator=(const JZThread&) = delete;

	explicit JZThread(const ThreadFunc& func, const string &name = string());
	explicit JZThread(ThreadFunc&& func, const string &name = string());
	~JZThread();

	void start();
	void join();

	bool isStart() { return m_bStart; }
	bool isJoin() { return m_bJoin; }
	std::thread::id tid() { return m_thread->get_id(); }
	const string& name() { return m_name; }
	static int threadNumCreated() { return g_numCreated; }

private:
	string m_name;
	bool m_bStart;
	bool m_bJoin;

	ThreadPtr m_thread;
	ThreadFunc m_func;

	static atomic<int> g_numCreated;

};