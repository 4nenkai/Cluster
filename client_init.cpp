#include <iostream>
#include "client_init.h"

dpp::cluster client(Client::token);

int8_t Client::client_conn_serv()
{
	try {
		client.on_log(dpp::utility::cout_logger());
		client.start(false);
		return 1;
	}
	catch (dpp::exception ex)
	{
		std::cout << ex.what() << '\n';
		return -1;
	}
	return 0;
}

void Client::client_command_handler()
{
	client.on_ready([&](const dpp::ready_t& event) {
		if (dpp::run_once<struct set_presence>())
		{
			client.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_watching, "You."));
		}
		if (dpp::run_once<struct register_bot_commands>()) {
			dpp::slashcommand information_command("information", "Get information about the bot", client.me.id);
			client.global_command_create(information_command);

			dpp::slashcommand blacklist_command("blacklist", "Blacklist a user", client.me.id);
			blacklist_command.add_option(dpp::command_option(dpp::co_user, "user", "User to blacklist", true));
			blacklist_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of Blacklist", false));
			client.global_command_create(blacklist_command);

			dpp::slashcommand unblacklist_command("unblacklist", "unblacklist a user", client.me.id);
			unblacklist_command.add_option(
				dpp::command_option(dpp::co_user, "user", "User to unblacklist", true)
			);
			client.global_command_create(unblacklist_command);
		}
		});
}

int main()
{
	sqlite3* db;
	DatabaseMgmt::initialize_database(db);
	Client cl_;
	cl_.client_command_handler();
	client.on_slashcommand([&](const dpp::slashcommand_t& event) {
		std::string user_id = std::to_string(event.command.usr.id);
		std::cout << "Received command: " << event.command.get_command_name() << " from user: " << user_id << std::endl;
		if (event.command.get_command_name() == "information") {
			event.reply("Event hasn't set yet.");
		}
		});
	cl_.client_conn_serv();
	sqlite3_close(db);
	return 0;
}
