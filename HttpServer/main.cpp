#include <httplib.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <map>

constexpr auto IP =
#ifdef _WIN32
"localhost";
#else
"172.24.22.226";
#endif

int main()
{
	using namespace httplib;
	using namespace std;
	using namespace std::chrono;
	
	map<string, string> urlMap;
	Server srv;
	regex urlReg(R"((ht|f)tp(s?)\:\/\/[0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*(:(0-9)*)*(\/?)([a-zA-Z0-9\-\.\?\,\'\/\\\+&amp;%\$#_]*)?)");

	ifstream mapFileRead("url.map");
	if (mapFileRead)
	{
		string key, value;
		while (mapFileRead)
		{
			getline(mapFileRead, key);
			getline(mapFileRead, value);
			urlMap[key] = value;
		}
		mapFileRead.close();
	}

	ofstream mapFileWrite("url.map");

	srv.bind_to_port(IP, 80);
	srv.set_mount_point("/", ".");
	srv.Get("/api/jump", [&](const Request& req, Response& res)
		{
			if (req.has_param("url"))
			{
				string body;
				auto url = req.get_param_value("url");
				if (regex_match(url, urlReg))
				{
					if (urlMap.find(url) == urlMap.end())
					{
						stringstream bufStream;
						string redirectUrl;
						bufStream << setiosflags(ios::uppercase) << hex << steady_clock::now().time_since_epoch().count();
						bufStream >> redirectUrl;

						redirectUrl = "jump/" + redirectUrl + ".html";
						std::ofstream ifs(redirectUrl);

						ifs << R"(<html><script language="javascript" type="text/javascript">window.location.href=')" << url << R"(';</script></html>)" << endl; 

						ifs.close();

						redirectUrl = "http://thatboy.info/" + redirectUrl;
						urlMap[url] = redirectUrl;

						mapFileWrite << url << endl << urlMap[url] << endl;
						mapFileWrite.flush();
						cout << "API-JUMP Create: " << redirectUrl << endl;
					}
					body = R"({"src":")" + url + R"(","url":")" + urlMap[url] + R"("})";
					cout << "API-JUMP Return: " << url << " ==> " << urlMap[url] << endl;
				}
				else
				{
					body = R"({"error":"not url"})";
					cout << "API-JUMP Return: " << url << " not url" << endl;
				}
				res.set_content(body, "text/json");
			}
		}
	);
	srv.set_error_handler([&](const Request& req, Response& res) {
		cout << "Error target:" << req.target << endl;
		res.set_redirect("/404.html");
		});


	if (srv.listen_after_bind())
		cout << "Server start failed. Permission denied!" << endl;
	else
		cout<<"Server started."<<endl;
}
