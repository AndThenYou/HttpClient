#include "JZThread.h"
#include <stdio.h>

std::atomic<int> JZThread::g_numCreated = 0;

JZThread::JZThread(const ThreadFunc& func, const string& name)
	:m_bStart(false)
	,m_bJoin(false)
	,m_func(func)
	,m_name(name)
{
	g_numCreated++;
	if (m_name.empty())
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread_%d", g_numCreated);
		m_name = buf;
	}
}

JZThread::JZThread(ThreadFunc&& func, const string& name)
	:m_func(std::move(func))
	,m_name(name)
	,m_bJoin(false)
	,m_bStart(false)
{
	g_numCreated++;
	if (m_name.empty())
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread_%d", g_numCreated);
		m_name = buf;
	}
}

JZThread::~JZThread()
{
	if (m_bStart && !m_bJoin)
	{
		m_thread->detach();
	}
}

void JZThread::start()
{
	if (!m_bStart)
	{
		m_bStart = true;
		m_thread = std::make_shared<std::thread>(m_func);
	}
}

void JZThread::join()
{
	if (m_bStart && m_thread->joinable())
	{
		m_bJoin = true;
		m_thread->join();
	}
}