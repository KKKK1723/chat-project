#include"LogicSystem.h"
#include"HttpConnection.h"
#include"VerifyGrpcClient.h"
#include"RedisMgr.h"

//注册Get请求
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert({ url,handler });
}

//注册Post请求
void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	_post_handlers.insert({ url,handler });
}

LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection)
		   {
		connection->_response.body().clear();
		beast::ostream(connection->_response.body()) << "receive get test req\n";

		int i = 0;
		for (auto& elem : connection->_get_params)
		{
			i++;
			beast::ostream(connection->_response.body()) << "param " << i << "  key is " << elem.first << "\n";
			beast::ostream(connection->_response.body()) << "param " << i << "  value is " << elem.second << "\n";
			;
		} });

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection)
		{

			auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
			std::cout << "receive body is " << body_str << std::endl;
			connection->_response.set(http::field::content_type, "text/json");
			Json::Value root;
			Json::Value src_root;
			Json::Reader reader;
			bool success = reader.parse(body_str, src_root);
			if (!success)
			{
				std::cout << "Failed to parse JSON data!" << std::endl;
				root["error"] = ErrorCodes::Error_Json;
				std::string json_str = root.toStyledString();
				beast::ostream(connection->_response.body()) << json_str;
				return true;
			}

			//判断是否有这个key
			if (!src_root.isMember("email"))
			{
				//std::cout << "没有" << std::endl;
				std::cout << "Json not have key-email!" << std::endl;
				root["error"] = ErrorCodes::Error_Json;
				std::string json_str = root.toStyledString();
				beast::ostream(connection->_response.body()) << json_str;
				return true;
			}

			
			std::string email_str = src_root["email"].asString();
			std::cout << "email is " << email_str << std::endl;
			GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email_str);
			root["error"] = rsp.error();
			root["email"] = src_root["email"];
			std::string json_str = root.toStyledString();
			beast::ostream(connection->_response.body()) << json_str;
			return true;

		});

	RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection)
			{

				auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
				std::cout << "receive body is " << body_str << std::endl;
				connection->_response.set(http::field::content_type, "text/json");
				Json::Value root;
				Json::Value src_root;
				Json::Reader reader;
				bool success = reader.parse(body_str, src_root);
				if (!success)
				{
					std::cout << "Failed to parse JSON data!" << std::endl;
					root["error"] = ErrorCodes::Error_Json;
					std::string json_str = root.toStyledString();
					beast::ostream(connection->_response.body()) << json_str;
					return true;
				}

				//判断验证码是否过期
				std::string verify_code = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString());
				if (verify_code=="")
				{
					std::cout << " get varify code expired" << std::endl;
					root["error"] = ErrorCodes::VerifyExpired;
					std::string json_str = root.toStyledString();
					beast::ostream(connection->_response.body()) << json_str;
					return true;
				}

				//判断验证码是否错误 (用户可能输错了验证码)
				if (verify_code != src_root["verifycode"].asString())
				{
					std::cout << " get varify code error , error is  verify_code != src_root[verifycode] " << std::endl;
					root["error"] = ErrorCodes::VerifyExpired;
					std::string json_str = root.toStyledString();
					beast::ostream(connection->_response.body()) << json_str;
					return true;
				}


				root["error"] = ErrorCodes::Success;
				root["email"] = src_root["email"].asString();
				root["user"] = src_root["user"].asString();
				root["passwd"] = src_root["passwd"].asString();
				root["confirm"] = src_root["confirm"].asString();
				root["verifycode"] = src_root["verifycode"].asString();
				std::string json_str = root.toStyledString();
				beast::ostream(connection->_response.body()) << json_str;
				return true; });
}

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection>connection)
{
	if (_get_handlers.find(url) == _get_handlers.end())
	{
		return false;
	}

	_get_handlers[url](connection);
	return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection>connection)
{
	if (_post_handlers.find(url) == _post_handlers.end())
	{
		return false;
	}

	_post_handlers[url](connection);
	return true;
}