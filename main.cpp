#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <cmath>

struct Item {
    int id;
    std::string name;

    bool operator==(const Item& other) const
        {return name == other.name;}

};

class Character {
private:
    int id;
    std::string name;
    std::map<std::string, double> wallet;
    std::vector<Item> inventory;
    static std::map<std::string, double> baseValues;

public:
    //Constructor
    Character(std::string n, int i) : name(n), id(i) {}

    static void setBaseValues(std::string currency, double value) {
        baseValues[currency]=value;
    }

    void addMoney(double amount, std::string currency) {
        wallet[currency]+=amount;
    }

    void addItem(const Item & item){
        inventory.push_back(item);
    }

    void coutStatus() const {
        std::cout << name << " | Inventory: " << inventory.size() << " | Wallet: ";
        if (wallet.empty()) {
            std::cout<<"Wallet is empty"<<std::endl;
        }
        else {
            for (auto const& balance : wallet) {
                std::cout<< "[ "<<balance.second<<" "<<balance.first<<" ]\n";
            }
        }
    }

    bool pay(Character & recipient, double price,const std::string price_currency,  const Item& targetItem);
    //const int amount_bought, TO IMPLEMENT LATER

    double getConversionRate(std::string from, std::string to) const;

    static void setExchangeRate(std::string currency, double valueInHUF) {
        baseValues[currency] = valueInHUF;
    }

};

bool Character::pay(Character & recipient, const double price,const std::string price_currency,  const Item& targetItem) {
    std::cout<<"PROCESSING OF TRANSACTION HAS BEGUN...\n";
    //MERCHANT HAS ITEM?

    auto itemIter = std::find(recipient.inventory.begin(), recipient.inventory.end(), targetItem);
    if (itemIter == recipient.inventory.end()) {
        std::cout<<"Transaction failed: "<<targetItem.name<<" not in "<<recipient.name<<"'s inventory."<<std::endl;
        return false;
    }

    bool PaymentSecured = false;
    std::string CurrencyToDeduct = price_currency;
    double AmountToDeduct = price;

    if (wallet.count(price_currency) && wallet[price_currency]>= price) {
        std::cout << "Paying directly with " << price_currency << ".\n";
        PaymentSecured = true;
    }
    else {
        std::cout<<"Insufficient fund in currency: "<<price_currency<<". Checking wallet for alternatives...\n";

        for (auto const& [my_curr, my_balance]: wallet) {
            if (my_curr == price_currency) continue;


            double rate = getConversionRate(my_curr, price_currency);

            if (rate <= 0) continue;

            double neededAmount = price / rate;

            if (my_balance >= neededAmount) {
                std::cout << "Found alternative! Converting " << my_curr << " for payment .\n";
                CurrencyToDeduct = my_curr;
                AmountToDeduct = neededAmount;
                PaymentSecured = true;
                break;
            }
        }

    }
    if (!PaymentSecured) {
        std::cout << "Transaction Failed: Insufficient funds in any currency.\n";
        return false;
    }




    //FINALLY, the exchange of money and goods

    //MONEY STUFF
    this->wallet[CurrencyToDeduct] -= AmountToDeduct;
    recipient.wallet[price_currency] += price;

    //GIT GUDS
    this->addItem(targetItem);
    recipient.inventory.erase(itemIter);

    std::cout<<"Transaction succesful!"<<std::endl;
    return true;
}

double Character::getConversionRate(std::string from, std::string to) const {
    if (baseValues.count(from)==0 || baseValues.count(to)==0) {
        std::cout<<"Error! Unknown currency."<<std::endl;
        return 0.0; //NO CONVERSION RATE AVAILABLE!!
    }
    double fromValue = baseValues[from];
    double toValue = baseValues[to];

    return fromValue / toValue;
}


//RATES MAP

std::map<std::string, double> Character::baseValues = {
    {"HUF", 1.0},
    {"USD", 330.0},
    {"EUR", 390.0},
    {"GBP", 444.45}
};


int main() {
    Character mainChar ("ME", 1);
    Character merchant ("Travelling Merchant", 2);

    Item sword (1, "Great Sword");

    merchant.addItem(sword);

    mainChar.addMoney(9, "GBP");

    std::cout << "\nSTATUS BEFORE TRANSACTION\n";
    mainChar.coutStatus();
    merchant.coutStatus();
    std::cout <<"\n";

    bool paymentToMerchant_success = mainChar.pay(merchant, 1000.0, "HUF", sword);

    if (paymentToMerchant_success) {
        std::cout << "\nSTATUS AFTER TRANSACTION\n";
        mainChar.coutStatus();
        merchant.coutStatus();
    }

    //std::cout << "Rate HUF -> USD: " <<mainChar.getConversionRate("HUF", "USD") << "\n";
    return 0;
}


