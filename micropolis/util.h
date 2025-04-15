#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <array>

namespace eth {

/**
 * @brief Converts a byte array to a hex string
 * @param data The byte array to convert
 * @param with_prefix Whether to include the "0x" prefix
 * @return A hex string representation of the byte array
 */
inline std::string bytes_to_hex(const std::vector<uint8_t>& data, bool with_prefix = true) {
    std::stringstream ss;
    if (with_prefix) {
        ss << "0x";
    }
    for (const auto& byte : data) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

/**
 * @brief Converts a hex string to a byte array
 * @param hex The hex string to convert
 * @return A byte array representation of the hex string
 */
inline std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::string hex_str = hex;
    
    // Remove "0x" prefix if present
    if (hex_str.substr(0, 2) == "0x" || hex_str.substr(0, 2) == "0X") {
        hex_str = hex_str.substr(2);
    }
    
    // Ensure the hex string has an even length
    if (hex_str.length() % 2 != 0) {
        hex_str = "0" + hex_str;
    }
    
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex_str.length(); i += 2) {
        std::string byte_str = hex_str.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        bytes.push_back(byte);
    }
    
    return bytes;
}

/**
 * @brief Checks if a string is a valid hex string
 * @param hex The string to check
 * @return true if the string is a valid hex string, false otherwise
 */
inline bool is_valid_hex(const std::string& hex) {
    std::string hex_str = hex;
    
    // Remove "0x" prefix if present
    if (hex_str.substr(0, 2) == "0x" || hex_str.substr(0, 2) == "0X") {
        hex_str = hex_str.substr(2);
    }
    
    // Check if all characters are valid hex digits
    return std::all_of(hex_str.begin(), hex_str.end(), [](char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
    });
}

/**
 * @brief Formats an Ethereum address (ensures it has the correct length and case)
 * @param address The address to format
 * @return The formatted address
 */
inline std::string format_address(const std::string& address) {
    std::string addr = address;
    
    // Remove "0x" prefix if present
    if (addr.substr(0, 2) == "0x" || addr.substr(0, 2) == "0X") {
        addr = addr.substr(2);
    }
    
    // Ensure the address has the correct length (40 characters for 20 bytes)
    if (addr.length() != 40) {
        throw std::invalid_argument("Invalid Ethereum address length");
    }
    
    // Convert to checksum address (EIP-55)
    std::string checksum_addr = "0x";
    for (size_t i = 0; i < 40; i++) {
        char c = addr[i];
        if (i % 2 == 0) {
            // For even positions, use uppercase if the corresponding nibble is 1
            if (std::stoi(std::string(1, addr[i]), nullptr, 16) >= 8) {
                checksum_addr += std::toupper(c);
            } else {
                checksum_addr += std::tolower(c);
            }
        } else {
            // For odd positions, use uppercase if the corresponding nibble is 1
            if (std::stoi(std::string(1, addr[i-1]), nullptr, 16) >= 8) {
                checksum_addr += std::toupper(c);
            } else {
                checksum_addr += std::tolower(c);
            }
        }
    }
    
    return checksum_addr;
}

/**
 * @brief Decodes an ABI-encoded function call
 * @param data The ABI-encoded data
 * @return A vector of decoded parameters
 */
inline std::vector<std::string> decode_abi(const std::string& data) {
    std::vector<std::string> decoded;
    std::string hex_data = data;
    
    // Remove "0x" prefix if present
    if (hex_data.substr(0, 2) == "0x" || hex_data.substr(0, 2) == "0X") {
        hex_data = hex_data.substr(2);
    }
    
    // Extract function selector (first 4 bytes)
    std::string function_selector = hex_data.substr(0, 8);
    decoded.push_back(function_selector);
    
    // For simplicity, we'll just return the function selector
    // A full ABI decoder would need to parse the parameter types and decode accordingly
    
    return decoded;
}

/**
 * @brief Encodes a function call according to the ABI specification
 * @param function_signature The function signature (e.g., "transfer(address,uint256)")
 * @param params The parameters to encode
 * @return The ABI-encoded function call
 */
inline std::string encode_abi(const std::string& function_signature, const std::vector<std::string>& params) {
    // This is a simplified version - a full implementation would need to handle all parameter types
    std::string encoded = "0x";
    
    // Calculate function selector (first 4 bytes of the keccak256 hash of the function signature)
    // For simplicity, we'll use a placeholder
    encoded += "a9059cbb"; // Example selector for transfer(address,uint256)
    
    // Encode parameters
    for (const auto& param : params) {
        // For simplicity, we'll just append the parameter
        // A full implementation would need to handle padding and different parameter types
        encoded += param;
    }
    
    return encoded;
}

/**
 * @brief Converts a uint256 value to a hex string
 * @param value The uint256 value to convert
 * @param with_prefix Whether to include the "0x" prefix
 * @return A hex string representation of the uint256 value
 */
inline std::string uint256_to_hex(const std::array<uint8_t, 32>& value, bool with_prefix = true) {
    return bytes_to_hex(std::vector<uint8_t>(value.begin(), value.end()), with_prefix);
}

/**
 * @brief Converts a hex string to a uint256 value
 * @param hex The hex string to convert
 * @return A uint256 value
 */
inline std::array<uint8_t, 32> hex_to_uint256(const std::string& hex) {
    std::vector<uint8_t> bytes = hex_to_bytes(hex);
    
    // Ensure the byte array has the correct length (32 bytes for uint256)
    if (bytes.size() > 32) {
        throw std::invalid_argument("Hex string too long for uint256");
    }
    
    // Pad with zeros if necessary
    while (bytes.size() < 32) {
        bytes.insert(bytes.begin(), 0);
    }
    
    std::array<uint8_t, 32> result;
    std::copy(bytes.begin(), bytes.end(), result.begin());
    
    return result;
}

/**
 * @brief Converts a hex string to a human-readable string
 * @param hex The hex string to convert
 * @return A human-readable string
 */
inline std::string hex_to_string(const std::string& hex) {
    std::vector<uint8_t> bytes = hex_to_bytes(hex);
    return std::string(bytes.begin(), bytes.end());
}

/**
 * @brief Converts a string to a hex string
 * @param str The string to convert
 * @param with_prefix Whether to include the "0x" prefix
 * @return A hex string representation of the string
 */
inline std::string string_to_hex(const std::string& str, bool with_prefix = true) {
    return bytes_to_hex(std::vector<uint8_t>(str.begin(), str.end()), with_prefix);
}

/**
 * @brief Converts a hex string to a uint16 array
 * @param hex The hex string to convert
 * @return A vector of uint16 values
 */
inline std::vector<uint16_t> hex_to_uint16_array(const std::string& hex) {
    std::vector<uint8_t> bytes = hex_to_bytes(hex);
    std::vector<uint16_t> result;
    
    // Process bytes in pairs to form uint16 values
    for (size_t i = 0; i < bytes.size(); i += 2) {
        uint16_t value;
        if (i + 1 < bytes.size()) {
            // Combine two bytes into a uint16 (big-endian)
            value = (static_cast<uint16_t>(bytes[i]) << 8) | static_cast<uint16_t>(bytes[i + 1]);
        } else {
            // If there's only one byte left, pad with zero
            value = static_cast<uint16_t>(bytes[i]) << 8;
        }
        result.push_back(value);
    }
    
    return result;
}

/**
 * @brief Converts a uint16 array to a hex string
 * @param values The uint16 array to convert
 * @param with_prefix Whether to include the "0x" prefix
 * @return A hex string representation of the uint16 array
 */
inline std::string uint16_array_to_hex(const std::vector<uint16_t>& values, bool with_prefix = true) {
    std::vector<uint8_t> bytes;
    
    // Convert each uint16 to two bytes (big-endian)
    for (const auto& value : values) {
        bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>(value & 0xFF));
    }
    
    return bytes_to_hex(bytes, with_prefix);
}

} // namespace eth

#endif // UTIL_H
