#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <unordered_map>

template<typename T>
class Wallet {
    private:
        std::unordered_map<std::string, T> erc20; 
    public:
        void deposit(const std::string& tokenAddress, const T& amount){
            erc20[tokenAddress] += amount;
        }
        bool withdraw(const std::string& tokenAddress, const T& amount){
            if(erc20.find(tokenAddress) != erc20.end() && erc20[tokenAddress] >= amount){
                erc20[tokenAddress] -= amount;
                return true;
            }
            return false;
        }
        T balance(const std::string& tokenAddress){
            if(erc20.find(tokenAddress) != erc20.end()){
                return erc20[tokenAddress];
            }
            else return T(0);
        }
};

#endif // WALLET_H
