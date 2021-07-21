#include "HttpClient.h"
#include "curl.h"
#include <stdio.h>

static size_t writeCallback(void* data, size_t size, size_t nMember, void* stream)
{
	std::vector<char>* recvBuf = static_cast<std::vector<char>*>(stream);
	size_t writeSize = size * nMember;
	recvBuf->insert(recvBuf->end(), (char*)data, (char*)data + writeSize);

	return writeSize;
}

static size_t readCallback(void* data, size_t size, size_t nMember, void* stream)
{
	size_t nRead = fread(data, size, nMember, (FILE*)stream);
	return nRead;
}

class JZCurl
{
public:
	JZCurl()
		:headers_(nullptr)
	{
		curl_ = curl_easy_init();
	}

	~JZCurl()
	{
		if (curl_)
		{
			curl_easy_cleanup(curl_);
		}
		if (headers_)
		{
			curl_slist_free_all(headers_);
		}
	}

	template<typename T>
	bool SetOption(CURLoption option, T data)
	{
		return CURLE_OK == curl_easy_setopt(curl_, option, data);
	}

	bool Init(const HttpClient& client, const RequestPtr& request, const ResponsePtr& response, char* errorBuf)
	{
		if (curl_ == nullptr) return false;
		// set no signal
		if (!SetOption(CURLOPT_NOSIGNAL, 1L)) return false;
		// set accept encoding
		if (!SetOption(CURLOPT_ACCEPT_ENCODING, "")) return false;
		// set timeout 
		if (!SetOption(CURLOPT_TIMEOUT, client.GetReadTimeout())) return false;
		// set connect timeout
		if (!SetOption(CURLOPT_CONNECTTIMEOUT, client.GetConnectTimeout())) return false;
		// no ssl
		if (!SetOption(CURLOPT_SSL_VERIFYPEER, false)) return false;
		if (!SetOption(CURLOPT_SSL_VERIFYHOST, false)) return false;
		/*set header*/
		std::vector<std::string> headers = request->GetHearders();
		if (!headers.empty())
		{
			for (auto& header : headers)
			{
				headers_ = curl_slist_append(headers_, header.c_str());
			}

			if (!SetOption(CURLOPT_HTTPHEADER, headers_))
			{
				return false;
			}
		}
		/*set main option*/
		return	SetOption(CURLOPT_URL, request->GetUrl()) &&
			SetOption(CURLOPT_WRITEFUNCTION, writeCallback) &&
			SetOption(CURLOPT_WRITEDATA, response->GetResponseData()) &&
			SetOption(CURLOPT_HEADERFUNCTION, writeCallback) &&
			SetOption(CURLOPT_HEADERDATA, response->GetResponseHeader()) &&
			SetOption(CURLOPT_ERRORBUFFER, errorBuf);
	}

	bool Perform(long& code)
	{
		CURLcode ret = curl_easy_perform(curl_);
		if (CURLE_OK != ret) return false;

		ret = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code);
		if (ret != CURLE_OK || !(code >= 200 && code < 300))
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
			return false;
		}
		return true;
	}
	

private:
	CURL* curl_;
	curl_slist* headers_;
};


HttpClient* HttpClient::m_instance = new HttpClient;


HttpClient::HttpClient()
	:m_threadPool("HttpClientThreadPool")
	,m_nConnectTimeout(30)
	,m_nReadTimeout(60)
{
	m_threadPool.SetMaxQueueSize(100);
	m_threadPool.Start(4);
	CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
	if (ret != CURLE_OK)
	{
		fprintf(stderr, "curl init fail! code: %d\n", ret);
		abort();
	}
}

HttpClient::~HttpClient()
{
	curl_global_cleanup();
}

void HttpClient::Request(const RequestPtr & request)
{
	ResponsePtr response = std::make_shared<HttpResponse>();
	m_threadPool.AddTask(std::bind(&HttpClient::process, this, request, response));
}

void HttpClient::RequestImmediately(const RequestPtr& request)
{
	ResponsePtr response = std::make_shared<HttpResponse>();
	std::thread t = std::thread(std::bind(&HttpClient::process, this, request, response));
	t.detach();
}

void HttpClient::process(const RequestPtr& request, const ResponsePtr& response)
{
	switch (request->GetRequestType())
	{
	case Type::eGet:
		doGet(request, response);
		break;
	case Type::ePut:
		doPut(request, response);
		break;
	case Type::ePost:
		doPost(request, response);
		break;
	case Type::eDelete:
		doDelete(request, response);
		break;
	default:
		break;
	}

	auto callback = request->GetCallback();
	if (callback)
	{
		callback(request, response);
	}
}

void HttpClient::doGet(const RequestPtr& request, const ResponsePtr& response)
{
	char errbuf[1024] = { 0 };
	long resposeCode = ERR_HTTP_UNKNOW_ERROR;
	JZCurl curl;
	bool res = curl.Init(*this, request, response, errbuf);
	res &= curl.SetOption(CURLOPT_FOLLOWLOCATION, true);
	res &= curl.Perform(resposeCode);

	response->SetErrorCode(resposeCode);
	if (!res)
	{
		response->SetErrorBuffer(errbuf);
	}
}

void HttpClient::doPut(const RequestPtr& request, const ResponsePtr& response)
{
	char errbuf[1024] = { 0 };
	long resposeCode = ERR_HTTP_UNKNOW_ERROR;

	FILE *fp = nullptr;
	string path = request->GetUploadFilePath();
	size_t size = 0;
	if (!path.empty())
	{
		fp = fopen(path.data(), "rb");
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
	}
	
	JZCurl curl;
	bool res = curl.Init(*this, request, response, errbuf)
		&& curl.SetOption(CURLOPT_CUSTOMREQUEST, "PUT")
		&& curl.SetOption(CURLOPT_VERBOSE, true)
		&& curl.SetOption(CURLOPT_POSTFIELDS, request->GetBody())
		&& curl.SetOption(CURLOPT_POSTFIELDSIZE, request->GetBodyLen());

	if (fp)
	{
		res &= curl.SetOption(CURLOPT_UPLOAD, 1L)
			&& curl.SetOption(CURLOPT_READFUNCTION, readCallback)
			&& curl.SetOption(CURLOPT_READDATA, fp)
			&& curl.SetOption(CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
	}
	res &= curl.Perform(resposeCode);
	response->SetErrorCode(resposeCode);
	if (!res)
	{
		response->SetErrorBuffer(errbuf);
	}
	if (fp)
	{
		fclose(fp);
	}
}

void HttpClient::doPost(const RequestPtr& request, const ResponsePtr& response)
{
	char errbuf[1024] = { 0 };
	long resposeCode = ERR_HTTP_UNKNOW_ERROR;
	JZCurl curl;
	bool res = curl.Init(*this, request, response, errbuf)
		&& curl.SetOption(CURLOPT_POST, 1)
		&& curl.SetOption(CURLOPT_POSTFIELDS, request->GetBody())
		&& curl.SetOption(CURLOPT_POSTFIELDSIZE, request->GetBodyLen())
		&& curl.Perform(resposeCode);

	response->SetErrorCode(resposeCode);
	if (!res)
	{
		response->SetErrorBuffer(errbuf);
	}
}

void HttpClient::doDelete(const RequestPtr& request, const ResponsePtr& response)
{
	char errorBuf[1024] = { 0 };
	long responseCode = -1;

	JZCurl curl;
	bool res = curl.Init(*this, request, response, errorBuf) &&
		curl.SetOption(CURLOPT_CUSTOMREQUEST, "DELETE") &&
		curl.SetOption(CURLOPT_FOLLOWLOCATION, true) &&
		curl.Perform(responseCode);

	response->SetErrorCode(responseCode);
	if(!res)
	{
		response->SetErrorBuffer(errorBuf);
	}
}