#ifndef CARTESI_H
#define CARTESI_H

#include "3rdparty/cpp-httplib/httplib.h"
#include <string>
#include <iostream>

/**
 * @brief Creates a notice with the given payload
 * @param cli The HTTP client to use
 * @param payload The payload to include in the notice
 */
void createNotice(httplib::Client &cli, const std::string& payload) {
    std::string notice = "{\"payload\":\"" + payload + "\"}";
    auto res = cli.Post("/notice", notice, "application/json");
    if (res && res->status == 201) {
        std::cout << "Notice created successfully" << std::endl;
    } else {
        std::cout << "Failed to create notice" << std::endl;
    }
}

/**
 * @brief Creates a report with the given payload
 * @param cli The HTTP client to use  
 * @param payload The payload to include in the report
 */
void createReport(httplib::Client &cli, const std::string& payload) {
    std::string report = "{\"payload\":\"" + payload + "\"}";
    auto res = cli.Post("/report", report, "application/json");
    if (res && res->status == 201) {
        std::cout << "Report created successfully" << std::endl;
    } else {
        std::cout << "Failed to create report" << std::endl; 
    }
}

#endif // CARTESI_H
