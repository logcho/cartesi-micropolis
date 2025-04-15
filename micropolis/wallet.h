#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <unordered_map>

template<typename T>
class Wallet {
private:
    std::unordered_map<std::string, T> erc20;

public:
    /**
     * @brief Deposits amount of tokens to the wallet
     * @param tokenAddress The token contract address
     * @param amount The amount to deposit
     */
    void deposit(const std::string& tokenAddress, const T& amount) {
        erc20[tokenAddress] += amount;
    }

    /**
     * @brief Withdraws amount of tokens from the wallet if sufficient balance exists
     * @param tokenAddress The token contract address
     * @param amount The amount to withdraw
     * @return true if withdrawal successful, false if insufficient balance
     */
    bool withdraw(const std::string& tokenAddress, const T& amount) {
        auto it = erc20.find(tokenAddress);
        if (it != erc20.end() && it->second >= amount) {
            it->second -= amount;
            return true;
        }
        return false;
    }

    /**
     * @brief Gets the balance of tokens in the wallet
     * @param tokenAddress The token contract address
     * @return The token balance, 0 if token not found
     */
    T balance(const std::string& tokenAddress) const {
        auto it = erc20.find(tokenAddress);
        return it != erc20.end() ? it->second : T(0);
    }
};

#endif // WALLET_H
