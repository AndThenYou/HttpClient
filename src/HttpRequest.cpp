#include "HttpRequest.h"

HttpRequest::HttpRequest(Type type)
	:m_eType(type)
{
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::SetRequestType(Type type)
{
	m_eType = type;
}

Type HttpRequest::GetRequestType(void)
{
	return m_eType;
}

void HttpRequest::SetUrl(const string & strUrl)
{
	m_strUri = strUrl;
}

const char * HttpRequest::GetUrl(void)
{
	return m_strUri.c_str();
}

void HttpRequest::AddHeader(const string & key, const string & value)
{
	string tmp = key + ":" + value;
	m_vecHeards.push_back(tmp);
}

const vector<string>& HttpRequest::GetHearders()
{
	return m_vecHeards;
}


void HttpRequest::SetUploadFilePath(const string & path)
{
	m_strUploadFilePath = path;
}

const string & HttpRequest::GetUploadFilePath(void)
{
	return m_strUploadFilePath;
}

void HttpRequest::SetBody(const string & body)
{
	m_vecBody.assign(body.begin(), body.end());
}

void HttpRequest::SetBody(const char * body, size_t size)
{
	while (size-- > 0) 
	{
		m_vecBody.push_back(*body++);
	}
}

const char* HttpRequest::GetBody(void)
{
	if (m_vecBody.empty())
		return nullptr;
	return &m_vecBody.front();
}

size_t HttpRequest::GetBodyLen(void)
{
	return m_vecBody.size();
}

void HttpRequest::SetUserData(void * pUserData)
{
	m_pUserData = pUserData;
}

void * HttpRequest::GetUserData(void)
{
	return m_pUserData;
}

void HttpRequest::SetCallback(const RequestCallback & cb)
{
	m_cb = cb;
}

const RequestCallback & HttpRequest::GetCallback(void) const
{
	return m_cb;
}
