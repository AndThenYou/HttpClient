#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>

#include "curl.h"

using std::string;
using std::map;
using std::vector;

class HttpRequest;
class HttpResponse;

using RequestCallback = std::function<void(const std::shared_ptr<HttpRequest>&, const std::shared_ptr<HttpResponse>&)>;

enum Type
{
	eGet = 0,
	ePut,
	ePost,
	eDelete,
	eUnknow,
};

class HttpRequest
{
public:
	
	HttpRequest(Type type = eUnknow);
	~HttpRequest();

	void SetRequestType(Type type);
	Type GetRequestType(void);

	void SetUrl(const string& strUrl);
	const char* GetUrl(void);

	void AddHeader(const string& key, const string& value);
	const vector<string>& GetHearders();

	void SetUploadFilePath(const string& path);
	const string& GetUploadFilePath(void);

	void SetBody(const string& body);
	void SetBody(const char* body, size_t size);
	const char* GetBody(void);
	size_t GetBodyLen(void);

	void SetUserData(void * pUserData);
	void* GetUserData(void);

	void SetCallback(const RequestCallback& cb);
	const RequestCallback& GetCallback(void) const;

private:
	Type m_eType;
	string m_strUri;
	string m_strUploadFilePath;
	vector<string> m_vecHeards;
	vector<char> m_vecBody;

	void* m_pUserData;
	RequestCallback m_cb;
};