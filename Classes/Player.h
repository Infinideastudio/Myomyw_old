#pragma once
#include <string>
#include <functional>
class Player
{
public:
	static void login(std::string server, std::string name, std::function<void()> onConnect, std::function<void(std::string)> onError);
	static void loginWithGuest();
	static void logout();

	static std::string getName();
	static inline std::string getServer() { return server; }
	static inline std::string getServerName() { return serverName; }
	static inline bool isLogged() { return logged; }
	static inline bool isGuest() { return guest; }
private:
	static std::string name, server, serverName;
	static bool logged, guest;
};