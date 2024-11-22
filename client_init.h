#ifndef CLIENT_I
#define CLIENT_I
#include <iostream>
#include <dpp/dpp.h>
#include <dpp/exception.h>
#include "client_database.h"
#include "sqlite/sqlite3.h"
#include <chrono>

class Client{
public:
	~Client()
	{
		std::cout << "client has been called from stack" << '\n';
	}

	int8_t client_conn_serv();
	void client_command_handler();
	bool is_user_bot(const std::string& user_id);

	inline const static std::string token = ""; //<-- Your bot token here

};

#endif