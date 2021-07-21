#include <iostream>
#include <Windows.h>
#include <mutex>

#include "HttpClient.h"

std::mutex mtx;

void onMessage(const RequestPtr& request, const ResponsePtr& response)
{
	if (response->GetErrorCode() == ERR_HTTP_OK)
	{
		std::cout << "request success." << std::endl;
		std::cout << "Recv Hearders: " << std::endl;

		const vector<char>* pvec = response->GetResponseHeader();
		for (auto & it : *pvec)
		{
			std::cout << it;
		}
		std::cout << std::endl;

		std::cout << "Recv Body: " << std::endl << response->GetResponseDataAsString() << std::endl;
	}
	else
	{
		std::cout << "request fail. code: " << response->GetErrorCode() << " msg: " << response->GetErrorBuffer() << std::endl;
	}
}

// get http
void TestGet1()
{
	RequestPtr request = std::make_shared<HttpRequest>(eGet);
	request->SetUrl("http://httpbin.org/ip");
	request->SetCallback(onMessage);
	HttpClient::GetInstance()->Request(request);
}

// get https
void TestGet2()
{
	RequestPtr request = std::make_shared<HttpRequest>(eGet);
	request->SetUrl("https://httpbin.org/get");
	request->SetCallback(onMessage);
	HttpClient::GetInstance()->Request(request);
}

void TestPost()
{
	RequestPtr request = std::make_shared<HttpRequest>(ePost);
	request->SetUrl("http://httpbin.org/post");
	request->SetCallback(onMessage);
	HttpClient::GetInstance()->Request(request);
}


int main()
{
	TestPost();
	system("pause");
	return 0;
}