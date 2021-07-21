#include "JZThreadPool.h"

JZThreadPool::JZThreadPool(const string & name)
	:m_name(name)
	,m_nMaxQueueSize(1000)
	,m_bRunning(false)
{
}

JZThreadPool::~JZThreadPool()
{
	if (m_bRunning)
	{
		Stop();
	}
}

void JZThreadPool::SetMaxQueueSize(int size)
{
	if (size > 0)
	{
		m_nMaxQueueSize = size;
	}
}

void JZThreadPool::Start(int threadNums)
{
	if (m_bRunning || threadNums <= 0)
	{
		return;
	}
	m_bRunning = true;
	m_threadsVec.reserve(threadNums);
	for (int i = 0; i < threadNums; i++)
	{
		JZThreadPtr pThread = std::make_shared<JZThread>(std::bind(&JZThreadPool::ThreadEntryFunc, this));
		pThread->start();
		m_threadsVec.push_back(pThread);
	}
}

void JZThreadPool::Stop()
{
	{
		LockGard lk(m_queMtx);
		m_bRunning = false;
		m_queNoEmpty.notify_all();
	}

	for (auto & thread : m_threadsVec)
	{
		thread->join();
	}
}

void JZThreadPool::AddTask(const Task & task)
{
	if (m_threadsVec.empty())
	{
		task();
	}
	else
	{
		LockGard lock(m_queMtx);
		while (IsFull())
		{
			m_queNoFull.wait(lock);
		}
		m_queue.push(task);
		m_queNoEmpty.notify_one();
	}
}

void JZThreadPool::AddTask(Task && task)
{
	if (m_threadsVec.empty())
	{
		task();
	}
	else
	{
		LockGard lock(m_queMtx);
		while (IsFull())
		{
			m_queNoFull.wait(lock);
		}
		m_queue.push(std::move(task));
		m_queNoEmpty.notify_one();
	}
}

void JZThreadPool::ThreadEntryFunc()
{
	try {
		while (m_bRunning)
		{
			Task task = take();
			if (task)
			{
				task();
			}
		}
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "exception caught in ThreadPool %s\n", m_name.data());
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
	}
	catch (...)
	{
		fprintf(stderr, "Unknow exception caught in ThreadPool %s\n", m_name.data());
		throw;
	}
}

Task JZThreadPool::take()
{
	LockGard lock(m_queMtx);
	while (m_bRunning && m_queue.empty())
	{
		m_queNoEmpty.wait(lock);
	}
	Task task = nullptr;
	if (!m_queue.empty())
	{
		task = m_queue.front();
		m_queue.pop();
		if (m_nMaxQueueSize > 0)
		{
			m_queNoFull.notify_one();
		}
	}
	return task;
}

bool JZThreadPool::IsFull() const
{
	return m_nMaxQueueSize > 0 && m_queue.size() >= m_nMaxQueueSize;
}