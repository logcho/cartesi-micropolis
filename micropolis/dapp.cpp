#include <stdio.h>
#include <iostream>

#include "3rdparty/cpp-httplib/httplib.h"
#include "3rdparty/picojson/picojson.h"
#include "3rdparty/intx/include/intx/intx.hpp"
#include "src/micropolis.h"
#include "wallet.h"
#include "util.h"
#include "cartesi.h"

std::unordered_map<std::string, Micropolis> cities;

void createGameNotices(httplib::Client &cli, const Micropolis& game) {
    // Convert the map data to a vector of uint16_t
    std::vector<uint16_t> flattenedMap;
    for (int x = 0; x < WORLD_W; x++) {
        for (int y = 0; y < WORLD_H; y++) {
            flattenedMap.push_back(game.getTile(x, y));
        }
    }
    std::string map_payload = eth::uint16_array_to_hex(flattenedMap);
    createNotice(cli, map_payload);
}

std::string handle_advance(httplib::Client &cli, picojson::value data)
{
    std::string address = data.get("metadata").get("msg_sender").to_str();
    std::string payload = data.get("payload").to_str();
    std::cout << "Address: " << address << std::endl;
    std::cout << "Payload: " << payload << std::endl;

    std::string decoded_payload = eth::hex_to_string(payload);
    std::cout << "Decoded payload: " << decoded_payload << std::endl;
    picojson::value parsed_payload;
    std::string err = picojson::parse(parsed_payload, decoded_payload); 
    if (!err.empty()) {
        std::cout << "Error: " << err << std::endl;
        return "reject";
    }
    std::string method = parsed_payload.get("method").to_str();
    if(method == "createCity"){
        if(cities.find(address) != cities.end()) return "reject";
        cities[address] = Micropolis();
        std::cout << "City created for address " << address << std::endl;
        createGameNotices(cli, cities[address]);
        return "accept";
    }
    if(method == "doTool"){
        if(cities.find(address) == cities.end()) return "reject";
        EditingTool tool = static_cast<EditingTool>(std::stoi(parsed_payload.get("tool").to_str()));
        int x = std::stoi(parsed_payload.get("x").to_str());
        int y = std::stoi(parsed_payload.get("y").to_str());
        cities[address].doTool(tool, x, y);
        std::cout << "Tool " << tool << " done for address " << address << " at (" << x << "," << y << ")" << std::endl;
        createGameNotices(cli, cities[address]);
        return "accept";
    }

    return "accept";
}

std::string handle_inspect(httplib::Client &cli, picojson::value data)
{
    std::cout << "Received inspect request data " << data << std::endl;
    return "accept";
}

int main(int argc, char **argv)
{
    std::map<std::string, decltype(&handle_advance)> handlers = {
        {std::string("advance_state"), &handle_advance},
        {std::string("inspect_state"), &handle_inspect},
    };
    httplib::Client cli(getenv("ROLLUP_HTTP_SERVER_URL"));
    cli.set_read_timeout(20, 0);
    std::string status("accept");
    std::string rollup_address;
    while (true)
    {
        std::cout << "Sending finish" << std::endl;
        auto finish = std::string("{\"status\":\"") + status + std::string("\"}");
        auto r = cli.Post("/finish", finish, "application/json");
        std::cout << "Received finish status " << r.value().status << std::endl;
        if (r.value().status == 202)
        {
            std::cout << "No pending rollup request, trying again" << std::endl;
        }
        else
        {
            picojson::value rollup_request;
            picojson::parse(rollup_request, r.value().body);
            picojson::value metadata = rollup_request.get("data").get("metadata");
            auto request_type = rollup_request.get("request_type").get<std::string>();
            auto handler = handlers.find(request_type)->second;
            auto data = rollup_request.get("data");
            status = (*handler)(cli, data);
        }
    }
    return 0;
}
