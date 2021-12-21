
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <limits>
#include <array>
#include <sstream>
#include <thread>
#include <chrono>

constexpr size_t BOARD_SIZE = 8; 
static bool time_elapsed = false;
static bool minimax_finished = false;

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

std::string state_from_move(std::string state, move next_move, bool player) {
    const std::array<std::pair<int,int>, 8> directions = { std::pair{-1,-1}, {-1,1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1} };
    const auto c_op = player ? 'X' : 'O';
    const auto c_self = !player ? 'X' : 'O';

    state[next_move.x * BOARD_SIZE + next_move.y] = c_self;

    for (const auto& dir : directions) {
        if (check_direction(state, player, next_move, dir.first, dir.second)) {
            
            size_t x = next_move.x + dir.first;
            size_t y = next_move.y + dir.second;

            while (x < BOARD_SIZE && y < BOARD_SIZE && state[x * BOARD_SIZE + y] == c_op) {
                state[x * BOARD_SIZE + y] = c_self;
                x += dir.first;
                y += dir.second;
            }
        }
    }   
       
    return state;
}

float count_heuristic(const std::string state) {
    int white = 0;
    int black = 0;
    for (const auto& c : state) {
        if (c == 'X') black++;
        else if (c == 'O') white++;
    }
    return 100.f * static_cast<float>(white - black) / static_cast<float>(white + black);
}

float mobility_heuristic(const std::string state) {
    auto white_moves = find_moves(state, true);
    auto black_moves = find_moves(state, false);

    if (white_moves.size() + black_moves.size() != 0)
        return 100.f * static_cast<float>(white_moves.size() - black_moves.size()) / static_cast<float>(white_moves.size() + black_moves.size());
    
    return 0.f;
}

float corner_heuristic(const std::string state) {
    int white = 0;
    int black = 0;
   
    std::array<move, 4> corners = { move{0,0}, move{0,7}, move{7,0}, move{7,7} };
    for (const auto& corner : corners) {
        if (state[corner.x * BOARD_SIZE + corner.y] == 'O') white++;
        if (state[corner.x * BOARD_SIZE + corner.y] == 'X') black++;
    }

    if (white + black == 0) {
        return 0.f;
    }

    return 100.f * static_cast<float>(white - black) / static_cast<float>(white + black);
}

float heuristic_val(std::string curr_state) {
    float count_val = count_heuristic(curr_state);
    float mobility_val = mobility_heuristic(curr_state);
    float corner_val = corner_heuristic(curr_state);

    return 25*count_val + 5*mobility_val + 30*corner_val;
}

std::pair<float, move> minimax(std::string game_state, int depth, float alpha, float beta, bool player) {
    if (depth == 0 || time_elapsed) {
        return { heuristic_val(game_state), {0,0} };
    }
    
    std::vector<move> possible_moves = find_moves(game_state, player);
    if (possible_moves.size() == 0) {
        return { heuristic_val(game_state), {0,0} };
    }
        
    if (player) {
        auto max_value = -std::numeric_limits<float>::infinity();
        move best_move = { 0,0 };
        for (const auto& move : possible_moves) {

            const auto new_state = state_from_move(game_state, move, player);
            auto new_val = minimax(new_state, depth - 1,  alpha, beta, !player);

            if (new_val.first > max_value) {
                max_value = new_val.first;
                best_move = move;
            }
            if (new_val.first > alpha) alpha = new_val.first;
            if (beta <= alpha) break;
        }

        return { max_value, best_move };
    }
    else {
        auto min_value = std::numeric_limits<float>::infinity();
        move best_move = { 0,0 };

        for (const auto& move : possible_moves) {

            const auto new_state = state_from_move(game_state, move, player);
            auto new_val = minimax(new_state, depth - 1, alpha, beta, !player);

            if (new_val.first < min_value) {
                min_value = new_val.first;
                best_move = move;
            };
            if (new_val.first < beta) beta = new_val.first;
            if (beta <= alpha) break; 
        }
        return { min_value, best_move };
    }
}

void timer(float move_time) {
    float time = 0;
    while (time < move_time && !minimax_finished) {
        time += 0.25f;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        std::cout << time << std::endl;
    }
    time_elapsed = true;
}

std::string get_response(game game, std::string state) {
    const std::array<char, 8> convert = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

    std::thread t(timer, game.move_time);

    auto result = minimax(state, 25, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), !game.player_color);
    minimax_finished = true;

    t.join();
    
    std::stringstream response;
    response << result.second.x << convert[result.second.y] << std::endl;

    return response.str();

}

game init_game_struct(command cmd) {
    game game{ true, cmd.player_color, cmd.time };

    if (cmd.player_color)
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
                
                if (game.player_color) {
                    result = get_response(game, game.last_state);
                }
                
            }
            if (game.active && cmd.type == "MOVE") {
                result = get_response(game, cmd.move);
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
