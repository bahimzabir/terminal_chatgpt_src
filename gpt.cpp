#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <fcntl.h>
#include "json.hpp"


//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
//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
std::string API_KEY = "";
using json = nlohmann::json;

const std::string ENDPOINT = "https://api.openai.com/v1/chat/completions";

const std::string MODEL_NAME = "gpt-3.5-turbo";
int		send_message(const std::string &message, int testing);
void	print_message(std::string content_message);

void encryptekey(std::string& str) {
	int i = 0;
	while(i < str.size() - 6) {
		str[i] = str[i] + 1;
		i++;
	}
	while (i < str.size()) {
		str[i] = str[i] - 2;
		i++;
	}
}

void decryptekey(std::string& str) {
	int i = 0;
	while(i < str.size() - 6) {
		str[i] = str[i] - 1;
		i++;
	}
	while (i < str.size()) {
		str[i] = str[i] + 2;
		i++;
	}
}

int	craeteApiFile() {

	std::string path = std::getenv("HOME");

	path += "/chatgpt/.apifile";
	std::string 	key;
	//std::cout << "Please go to https://platform.openai.com/account/api-keys and click Create new secret key\n";
	std::cout << "then past your openAI api key here:\n>>";
	std::getline(std::cin, key);
	API_KEY = key;
	std::cerr << "Checking the Key...\n";
	if (!send_message("test", 1)) {
		return 0;
	}
	std::ofstream file(path);
	if (!API_KEY.empty() && API_KEY[0] == 's')
		encryptekey(key);
	file << key;
	file.close();
	return 1;
}

void	getApiKey() {
	std::fstream	apifile;
	std::string path = std::getenv("HOME");

	path += "/chatgpt/.apifile";

	apifile.open(path, std::ios::in);

	if (!apifile.is_open()) {
		while (!craeteApiFile());
		apifile.open(path, std::ios::in);
		if (!apifile.is_open())
		{
			std::cerr << "cannot open file '" + path + "', no permissions\n";
			exit(1);
		}
		std::cerr << "your Key is updated, please run the gpt again.\n";
		exit(1);
	}
	else if (!std::getline(apifile, API_KEY))
	{
		while (!craeteApiFile());
		std::cerr << "your Key is updated, please run the gpt again\n";
		exit(1);
	}
	apifile.close();
}


void	print_message(std::string content_message) {

	int in = 0;
	std::string reply;

	for (int i = 0; i < content_message.length(); i++)
	{	
		if(content_message[i] == '"' && !in)
			in = 1;
		else if (content_message[i] == '"' && in)
			in = 0;
		if (content_message[i] == '\\')
		{	
			if (!in && content_message[i + 1] == 'n')
			{
				reply += "\n";
				i += 1;
				continue;
			}
			i++;
		}
		reply += content_message[i];
	}
	std::cout << "--------------------------------\n";
		std::cout << reply << "\n";
	std::cout << "--------------------------------\n";
}

int send_message(const std::string &message, int testing)
{
	std::string data = "{ \"model\": \"" + MODEL_NAME + "\", \"user\": \"" + "user" + "\", \"messages\": [{\"role\": \"user\", \"content\": \"" + "`" + message + "`\"}] }";
	if (!API_KEY.empty() && API_KEY[0] == 't')
		decryptekey(API_KEY);
	std::string command = "curl -s -X POST -H \"Content-Type: application/json\" -H \"Authorization: Bearer " + API_KEY + "\" -d '" + data + "' " + ENDPOINT;
	FILE *fp = popen(command.c_str(), "r");
	if (fp == nullptr)
	{
		std::cout << "Failed to connect to the server please check you connection" << std::endl;
		exit(1);
	}

	char response[8192];
	size_t response_len = fread(response, 1, sizeof(response) - 1, fp);
	response[response_len] = '\0';
	pclose(fp);

	json dat = json::parse(response);
	std::string msg;
	try {
		msg = dat["choices"][0]["message"]["content"];
	} catch (std::exception str) {
		msg = dat["error"]["message"];
		print_message(msg);
		if (testing != 1)
			std::cout << "To update the api key please RUN: chatgbt -k" << std::endl;
		return 0;
	}
	if(!testing)
		print_message(msg);
	return 1;
	
}

int main(int argc, char **argv)
{
	std::string user_input;

	if (argc != 2)
	{
		std::cout << "chatgpt v1.2: Usage: chatgbt <question>\nif you trying to update the api key use: chatgbt -k" << std::endl;
		return 1;
	}
	std::string arg = argv[1];
	if (arg[0] == '-') {
		if (arg == "-k") {
			while (!craeteApiFile());
			std::cout << "your Key is updated, please run the gpt again.\n";
			std::cout << "chatgpt v1.2: if you need to reinstall/update the program go to https://github.com/bahimzabir/terminal_chatgbt_installer\n";
			exit(1);
		}
		else if(arg == "--help") {
			std::cout << "chatgpt v1.2: Usage: chatgbt <question>\nif you trying to update the api key use: chatgbt -k" << std::endl;
			return 0;
		}
		else {
			std::cout << "chatgpt v1.2: Usage: chatgbt <question>\nif you trying to update the api key use: chatgbt -k" << std::endl;
			return 1;
		}
	}
	getApiKey();
	user_input = argv[1];
	user_input.erase(user_input.find_last_not_of("\n") + 1);
	send_message(argv[1], 0);

	return 0;
}
