#pragma once
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "JZThread.h"

using std::string;
using std::queue;
using std::vector;
using std::condition_variable;
using std::mutex;

using Task = std::function<void()>;
using JZThreadPtr = std::shared_ptr<JZThread>;
using LockGard = std::unique_lock<std::mutex>;

class JZThreadPool
{
public:
	JZThreadPool(const JZThreadPool&) = delete;
	JZThreadPool& operator=(const JZThreadPool&) = delete;

	explicit JZThreadPool(const string& name = string("ThreadPool"));
	~JZThreadPool();

	void SetMaxQueueSize(int size);

	void Start(int threadNums);
	void Stop();

	void AddTask(const Task& task);
	void AddTask(Task&& task);

private:
	void ThreadEntryFunc();
	bool IsFull() const;
	Task take();
private:
	string m_name;
	bool m_bRunning;
	int m_nMaxQueueSize;
	queue<Task> m_queue;
	mutable mutex m_queMtx;
	condition_variable m_queNoFull;
	condition_variable m_queNoEmpty;
	vector<JZThreadPtr> m_threadsVec;
};