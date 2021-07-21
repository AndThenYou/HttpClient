#include "HttpResponse.h"

HttpResponse::HttpResponse()
	:m_nResponseCode(ERR_HTTP_UNKNOW_ERROR)
{
}

bool HttpResponse::IsSucceed() const
{
	return ERR_HTTP_OK == m_nResponseCode;
}

long HttpResponse::GetErrorCode(void)
{
	return m_nResponseCode;
}

void HttpResponse::SetErrorCode(long code)
{
	m_nResponseCode = code;
}

void HttpResponse::SetResponseData(const vector<char>& data)
{
	m_vecResponseData = data;
}

const vector<char>* HttpResponse::GetResponseData(void)
{
	return &m_vecResponseData;
}

string HttpResponse::GetResponseDataAsString(void)
{
	if (m_vecResponseData.empty())
		return "";
	return string(&*m_vecResponseData.begin(), m_vecResponseData.size());
}

void HttpResponse::SetResponseHeader(const vector<char>& header)
{
	m_vecResponseHeader = header;
}

const vector<char>* HttpResponse::GetResponseHeader(void)
{
	return &m_vecResponseHeader;
}

void HttpResponse::SetErrorBuffer(const string & err)
{
	m_strErrorBuffer = err;
}

const string & HttpResponse::GetErrorBuffer(void)
{
	return m_strErrorBuffer;
}
