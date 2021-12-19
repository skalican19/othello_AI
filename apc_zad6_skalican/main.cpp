
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

constexpr size_t BOARD_SIZE = 8; 

struct game {
    bool active;
    bool player_color; //0 - black, 1 - white
    float move_time;
    std::string last_state;    
};

struct command {
    std::string type; //type of command
    std::string move; 
    bool player_color;
    float time;
};

struct move {
    size_t x;
    size_t y;
};

bool is_number(std::string s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit); 
}

bool check_direction(const std::string state, const bool color, const move move, const int dir_x, const int dir_y) {
    size_t x = move.x + dir_x;
    size_t y = move.y + dir_y;
    size_t count = 0;

    const auto c_op = color ? 'X' : 'O';
    const auto c_self = !color ? 'X' : 'O';
    
    while (x < BOARD_SIZE && y < BOARD_SIZE && state[x * BOARD_SIZE + y] == c_op) {
        count++;
        x += dir_x;
        y += dir_y;
    }
    
    return x < BOARD_SIZE && y < BOARD_SIZE && count >= 1 && state[x * BOARD_SIZE + y] == c_self;
}

bool check_diagonals(const std::string state, const bool color, const move move) {
    return check_direction(state, color, move, -1, -1) ||
        check_direction(state, color, move, -1, 1) ||
        check_direction(state, color, move, 1, -1) ||
        check_direction(state, color, move, 1, 1);
}

bool check_straights(std::string state, bool color, move move) {
    return check_direction(state, color, move, 0, -1) ||
        check_direction(state, color, move, 0, 1) ||
        check_direction(state, color, move, 1, 0) ||
        check_direction(state, color, move, -1, 0);
}

std::vector<move> find_moves(const std::string state, const bool color) {
    std::vector<move> moves;
    
    for (size_t x = 0; x < BOARD_SIZE; x++) {
        for (size_t y = 0; y < BOARD_SIZE; y++) {
            if (state[x*BOARD_SIZE+y] == '-')
            if (check_diagonals(state, color, { x,y }) || check_straights(state, color, { x,y })) {
                moves.push_back(move{ x,y });
            }
        }
    }

    return moves;
}

game init_game_struct(command cmd) {
    game game{ true, cmd.player_color, cmd.time };

    if (!cmd.player_color)
        game.last_state = std::string("---------------------------OX------XO---------------------------");

    return game;
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
    game game = { false };
    while (std::getline(std::cin, command))
    {
        std::string result;
        try {
            auto cmd = parse_command(command);
            if (cmd.type == "STOP") { return EXIT_SUCCESS; }
            if (cmd.type == "START" && !game.active) {
                game = init_game_struct(cmd);
                
                if (!game.player_color) {
                    //make a move
                }
                
            }
            if (game.active && cmd.type == "MOVE") {
                std::vector<move> moves = find_moves(cmd.move, cmd.player_color);
            }
        }

        catch (std::string msg) {
            std::clog << msg << std::endl;
            return EXIT_FAILURE;
        }
        
        std::cout << result << std::endl;
    }

    return EXIT_SUCCESS;
}
