
#include <iostream>
#include <string>

void parse_command(std::string input) {
    size_t ws_pos = input.find_first_of(" ");
    std::string command = input.substr(0, ws_pos);
    if (command == "START") {
        //TODO
    }
    else if (command == "STOP") {
        //TODO
    }
    else if (command == "MOVE") {
        //TODO
    }
}


int main()
{
    std::string command;
    while (std::getline(std::cin, command))
    {
        std::string result;
        parse_command(command);
        std::cout << result << std::endl;
    }
}
