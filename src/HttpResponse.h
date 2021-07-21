#pragma once
#include "HttpRequest.h"

#define ERR_HTTP_OK 200
#define ERR_HTTP_CREATED 201
#define ERR_HTTP_ACCEPTED 202
#define ERR_HTTP_NON_AUTHORITATIVE 203
#define ERR_HTTP_NO_CONTENT 204
#define ERR_HTTP_RESET_CONTENT 205
#define ERR_HTTP_PARTIAL_CONTENT 206
#define ERR_HTTP_MULITIPLE_CHOICES 300
#define ERR_HTTP_MULITIPLE_PERMANENTLY 301
#define ERR_HTTP_FOUND 302
#define ERR_HTTP_SEE_OTHER 303
#define ERR_HTTP_NOT_MODIFIED 304
#define ERR_HTTP_USE_PROXY 305
#define ERR_HTTP_BAD_REQUEST 400
#define ERR_HTTP_UNAUTHORIZED 401
#define ERR_HTTP_FORBIDDEN 403
#define ERR_HTTP_NO_FOUND 404
#define ERR_HTTP_SERVER_ERROR 500
#define ERR_HTTP_NOT_IMPLEMENTED 501
#define ERR_HTTP_BAD_GATWAY 502
#define ERR_HTTP_SERVICE_UNVALIABLE 503
#define ERR_HTTP_GATWAY_TIMEOUT 504
#define ERR_HTTP_UNKNOW_ERROR 600 

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse() = default;

	bool IsSucceed() const;
	long GetErrorCode(void);
	void SetErrorCode(long code);

	void SetResponseData(const vector<char>& data);
	const vector<char>* GetResponseData(void);
	string GetResponseDataAsString(void);

	void SetResponseHeader(const vector<char>& header);
	const vector<char>* GetResponseHeader(void);

	void SetErrorBuffer(const string& err);
	const string& GetErrorBuffer(void);

private:
	long m_nResponseCode;
	vector<char> m_vecResponseData;
	vector<char> m_vecResponseHeader;
	string m_strResponseDataStr;
	string m_strErrorBuffer;
};