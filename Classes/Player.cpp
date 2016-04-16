#include "Player.h"
#include "Text.h"
#include "network/HttpClient.h"
using namespace cocos2d::network;

std::string Player::name, Player::server, Player::serverName;
bool Player::logged = false, Player::guest = false;

void Player::login(std::string name, std::string server, std::function<void()> onConnect, std::function<void(std::string)> onError)
{
	if (!logged) {
		auto request = new HttpRequest();
		request->setRequestType(HttpRequest::Type::GET);
		request->setUrl((server + "/is_server").c_str());

		request->setResponseCallback([server, name, onConnect, onError](HttpClient* client, HttpResponse* response) {
			if (response->isSucceed()) {
				auto buffer = response->getResponseData();
				Player::server = server;
				serverName = std::string(buffer->begin(), buffer->end());
				Player::name = name;
				guest = false;
				logged = true;
				onConnect();
			}
			else {
				onError(response->getErrorBuffer());
			}
		});
		HttpClient::getInstance()->send(request);
		request->release();
	}
}

void Player::loginWithGuest()
{
	if (!logged) {
		guest = true;
		logged = true;
	}
}

void Player::logout()
{
	logged = false;
}

std::string Player::getName()
{
	if (logged) {
		if (guest) {
			return Text::get("guest");
		}
		else {
			return name;
		}
	}
	else {
		return Text::get("notLogged");
	}
}
