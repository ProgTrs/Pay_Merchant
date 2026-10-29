# Pay Merchant

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-26-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++ 26" />
  <img src="https://img.shields.io/badge/CMake-4.0-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake 4.0" />
  <img src="https://img.shields.io/badge/Status-Demo-28A745?style=for-the-badge" alt="Demo project" />
</p>

<p align="center">
  <strong>A lightweight C++ transaction simulation for merchant trading, wallet management, and multi-currency conversion.</strong>
</p>

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Project Structure](#project-structure)
- [Architecture & Design](#architecture--design)
- [Core Transaction Flow](#core-transaction-flow)
- [Currency Conversion Logic](#currency-conversion-logic)
- [Build and Run](#build-and-run)
- [Example Execution](#example-execution)
- [Limitations](#limitations)
- [Future Improvements](#future-improvements)

---

## Overview

`Pay_Merchant` is a compact C++ project that models a simple in-game or simulation-style economy. It demonstrates:

- a character with a wallet and inventory,
- merchant-to-player item exchange,
- direct currency payment,
- automatic fallback payment using alternative currencies,
- static exchange-rate conversion based on a base currency map.

The project is intentionally compact and educational. It focuses on the mechanics of a single `pay()` function, showing how a buyer can purchase an item from a merchant while checking whether the required funds are available in the preferred currency or in another wallet balance.

This repo acts as a demonstration of object-oriented modeling, STL containers, and basic financial logic in C++.

---

## Key Features

- Multi-currency wallet support using `std::map<std::string, double>`
- Inventory system using `std::vector<Item>`
- Merchant validation before completing a trade
- Direct payment when the exact currency exists and is sufficient
- Automatic conversion from an alternate currency when direct payment fails
- Static FX table for conversion between currencies
- Clean console-based demonstration of transaction flow

---

## Project Structure

```text
Pay_Merchant/
├── CMakeLists.txt
├── main.cpp
├── README.md
└── .idea/            # editor metadata (not part of the runtime logic)
```

### File overview

- `CMakeLists.txt` — defines the project and builds the executable
- `main.cpp` — contains all gameplay/transaction logic and the demo entry point
- `README.md` — project documentation and usage guide

---

## Architecture & Design

### 1) `Item`

```cpp
struct Item {
    int id;
    std::string name;

    bool operator==(const Item& other) const {
        return name == other.name;
    }
};
```

The `Item` type is intentionally simple:

- `id` identifies the item
- `name` gives the item a readable label
- equality is based on `name`, not `id`, so item comparison in the merchant inventory is name-based

### 2) `Character`

The `Character` class models the actor in the system:

- `wallet`: the player's stored amounts by currency
- `inventory`: list of owned items
- `baseValues`: a static exchange rate table shared by all `Character` instances

Important methods include:

- `addMoney(double amount, std::string currency)`
- `addItem(const Item & item)`
- `coutStatus() const`
- `pay(Character & recipient, double price, const std::string price_currency, const Item& targetItem)`
- `getConversionRate(std::string from, std::string to) const`

### 3) Exchange-rate model

The project uses a static map:

```cpp
std::map<std::string, double> Character::baseValues = {
    {"HUF", 1.0},
    {"USD", 330.0},
    {"EUR", 390.0},
    {"GBP", 444.45}
};
```

This means:

- `HUF` is the base unit
- `USD`, `EUR`, and `GBP` are normalized relative to HUF
- conversion is computed by dividing one currency's base value by another's

This is a simplified foreign-exchange model intended for demonstration rather than production finance.

---

## Core Transaction Flow

The core behavior is implemented in `Character::pay()`.

### Transaction steps

1. Confirm the target item exists in the merchant's inventory.
2. Check whether the buyer has enough of the `price_currency` already.
3. If not, iterate through all wallet currencies except the requested one.
4. Compute the conversion rate between each alternative currency and the required price currency.
5. Determine how much of the alternative currency would be needed to fund the payment.
6. If the balance is sufficient, use that currency and complete the transaction.
7. Deduct the buyer balance, credit the merchant, transfer the item, and remove it from the merchant inventory.

### Pseudocode

```cpp
if (item not in merchant inventory) {
    fail
}

if (buyer has enough requested currency) {
    pay directly
} else {
    for each alternate currency in wallet {
        if (currency != requested currency) {
            convert using rate
            if (sufficient funds) {
                use alternate currency
                break
            }
        }
    }
}

if (no valid payment path) {
    fail
}

buyer wallet -= spend amount
merchant wallet += received price
buyer inventory += item
merchant inventory -= item
```

---

## Currency Conversion Logic

The conversion function is:

```cpp
double Character::getConversionRate(std::string from, std::string to) const {
    if (baseValues.count(from) == 0 || baseValues.count(to) == 0) {
        std::cout << "Error! Unknown currency." << std::endl;
        return 0.0;
    }

    double fromValue = baseValues[from];
    double toValue = baseValues[to];

    return fromValue / toValue;
}
```

This yields a ratio used in the transaction logic:

```cpp
double neededAmount = price / rate;
```

### Example

If the buyer holds `GBP` and wants to pay `1000 HUF`:

- `GBP` base value = `444.45`
- `HUF` base value = `1.0`
- conversion rate from `GBP` to `HUF` = `444.45 / 1.0 = 444.45`

Then the required amount in GBP is roughly:

```text
1000 / 444.45 ≈ 2.25 GBP
```

If the buyer wallet contains enough GBP, the code may pay using GBP even if the merchant price is stated in HUF.

---

## Build and Run

### Requirements

- CMake 4.0+
- A modern C++ compiler supporting C++26
- A compatible Unix-like shell or Windows build environment

### Build steps

```bash
cmake -S . -B build
cmake --build build
```

### Run the executable

```bash
./build/Pay_Merchant
```

### Project configuration

The project is configured in `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 4.0)
project(Pay_Merchant)

set(CMAKE_CXX_STANDARD 26)

add_executable(Pay_Merchant main.cpp)
```

---

## Example Execution

The demo creates:

- a player named `ME`
- a merchant named `Travelling Merchant`
- an item: `Great Sword`

The player is given `9 GBP`, and the merchant is holding the sword. The program then attempts a payment for `1000 HUF`.

### Example runtime output

```text
STATUS BEFORE TRANSACTION
ME | Inventory: 0 | Wallet: [ 9 GBP ]
Travelling Merchant | Inventory: 1 | Wallet: [ ]

PROCESSING OF TRANSACTION HAS BEGUN...
Paying directly with HUF.
Transaction failed: Great Sword not in Travelling Merchant's inventory.
```

The actual flow in the current code depends on the item being present in the merchant inventory, which it is in the demo. When successful, the program prints a transaction-completion message and refreshed wallet state.

---

## Limitations

This project is intentionally a small demonstration, and it has several practical limitations:

- The exchange-rate system is static and hard-coded.
- Currency conversion uses a simple ratio, not a real market exchange model.
- Inventory comparison is based on the item name only.
- There is no stock quantity system.
- No persistence layer or save system is implemented.
- No transaction rollback or atomic consistency checks.
- There is no handling for negative balances or invalid numeric inputs.
- The code is console-based and does not expose a user interface.

These trade-offs make the project clear and compact, but they also mean it is not production-ready as a financial system.

---

## Future Improvements

Potential enhancements for a more realistic or scalable version include:

- Separate `Wallet` and `Inventory` data structures
- Support for multiple items and quantities
- More robust FX conversion with live rates or a configurable rate table
- Validation for insufficient funds and invalid currencies
- Unit tests for purchase edge cases
- CLI or GUI layers for user interaction
- Transaction logging and audit records
- Proper object modeling with encapsulation and exceptions

---

## Summary

`Pay_Merchant` provides a concise but educational example of:

- state management,
- item transfer,
- wallet operations,
- multi-currency financial logic,
- C++ standard library usage,
- and a simple transaction pipeline.

It is ideal for learning how object-oriented logic and financial rules can be modeled in a compact C++ program.

<p align="center">
  <a href="#top">Back to top</a>
</p>
