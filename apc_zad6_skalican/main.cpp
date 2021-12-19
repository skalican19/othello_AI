
#include <iostream>
#include <string>
#include <algorithm>



struct game {
    bool color; //0 - black, 1 - white
    float move_time;
    float curr_time;
};

struct command {
    std::string type; //type of command
    std::string move; 
    bool player_color;
    float time;
};

bool is_number(std::string s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit); 
}

bool move_valid(std::string move) {
    return true;
}

command parse_command(const std::string input) {
    size_t ws_pos = input.find_first_of(" ");
    const std::string cmd_type = input.substr(0, ws_pos); 

    command cmd = {};
    
    //if input is only one word its either stop on unsupported
    if (ws_pos == std::string::npos) {
        if (cmd_type == "STOP") {
            return { cmd_type };
        }
        else {
            throw std::string("Unsupported command.");
        }
    }

    //cut whitespaces between params
    std::string params = input.substr(ws_pos);
    ws_pos = params.find_first_not_of(" ");
    //if only whitespaces unsupported
    if (ws_pos == std::string::npos) { throw std::string("Unsupported command."); }

    params = params.substr(ws_pos);
    
    if (cmd_type == "START") {
        cmd.type = cmd_type;

        if (params[1] != ' ') { throw std::string("Unsupported command."); }

        //player color either black or white
        if (params[0] == 'B') { cmd.player_color = false; }
        else if ((params[0] == 'W')) { cmd.player_color = true; }
        else { throw std::string("Unsupported command."); }

        ws_pos = params.substr(1).find_first_not_of(" ");
        params = params.substr(ws_pos + 1);

        if (is_number(params)) {
            cmd.time = static_cast<float> (std::stoi(params));
        }

    }
    else if (cmd_type == "MOVE") {
        cmd.type = cmd_type;
        
        if (params.size() != 64 && move_valid(params)) { throw std::string("Invalid move."); }

        cmd.move = params;
    }
    else {
        throw std::string("Unsupported command.");
    }

    return cmd;
}


int main()
{
    std::string command;
    while (std::getline(std::cin, command))
    {
        std::string result;
        try {
            auto cmd = parse_command(command);
            if (cmd.type == "STOP") { return EXIT_SUCCESS; }
            std::cout << cmd.type << " " << cmd.player_color << " " << cmd.time << std::endl;
        }

        catch (std::string msg) {
            std::clog << msg << std::endl;
            return EXIT_FAILURE;
        }
        
        std::cout << result << std::endl;
    }

    return EXIT_SUCCESS;
}
