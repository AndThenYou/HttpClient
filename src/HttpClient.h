#pragma once
#include "JZThreadPool.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <memory>

using RequestPtr = std::shared_ptr<HttpRequest>;
using ResponsePtr = std::shared_ptr <HttpResponse>;

class HttpClient
{
public:
	static HttpClient* GetInstance(void) { return m_instance; }

	void SetConnectTimeout(long second) { m_nConnectTimeout = second; }
	long GetConnectTimeout(void) const { return m_nConnectTimeout; }
	void SetReadTimeout(long second) { m_nReadTimeout = second; }
	long GetReadTimeout(void) const { return m_nReadTimeout; }

	void Request(const RequestPtr& request);
	void RequestImmediately(const RequestPtr& request);

private:
	HttpClient();
	~HttpClient();
	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;

	void process(const RequestPtr& request, const ResponsePtr& response);

	void doGet(const RequestPtr& request, const ResponsePtr& response);
	void doPut(const RequestPtr& request, const ResponsePtr& response);
	void doPost(const RequestPtr& request, const ResponsePtr& response);
	void doDelete(const RequestPtr& request, const ResponsePtr& response);

private:
	static HttpClient* m_instance;

	long m_nConnectTimeout;
	long m_nReadTimeout;

	JZThreadPool m_threadPool;
};