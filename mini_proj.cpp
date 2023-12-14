#include <iostream>
#include <fstream>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <algorithm> // for std::find_if

using namespace std;

class Account {
protected:
    double balance;
    string name;
    string accountNumber;
    string branch;

public:
    Account(double initialBalance, const string& accName, const string& accNumber, const string& accBranch)
        : balance(initialBalance), name(accName), accountNumber(accNumber), branch(accBranch) {}

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        balance = balance + amount;
        cout << "Deposited Rs" << amount << " to the account." << endl;
        logOperation("Deposit", "Account", amount);
    }

    void withdraw(double amount) {
        if (balance < amount) {
            cout << "\nWithdrawal Amount is greater than your current balance." << endl;
            logOperation("Failed Withdrawal", "Account", amount);
        } else {
            balance = balance - amount;
            cout << "Withdrawn Rs" << amount << " from the account." << endl;
            logOperation("Withdrawal", "Account", amount);
        }
    }

    void displayBalance() const {
        cout << "Account Balance: Rs" << balance << endl;
    }

    void displayAccountDetails() const {
        cout << "Account Holder: " << name << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Branch: " << branch << endl;
    }

    string getName() const {
        return name;
    }

    string getAccountNumber() const {
        return accountNumber;
    }

    string getBranch() const {
        return branch;
    }

private:
    void logOperation(const string& operationType, const string& accountType, double amount) {
        ofstream logFile("account_and_transaction_data.txt", ios::app);

        if (logFile.is_open()) {
            time_t now = time(0);
            tm* localTime = localtime(&now);

            logFile << "[" << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
                    << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << "] ";
            logFile << operationType << " " << accountType << " Rs" << amount << " for " << name << " (Account Number: " << accountNumber << ")" << endl;

            logFile.close();
        } else {
            cout << "Unable to open file for logging." << endl;
        }
    }
};

class SavingsAccount : public Account {
public:
    SavingsAccount(double initialBalance, const string& accName, const string& accNumber, const string& accBranch)
        : Account(initialBalance, accName, accNumber, accBranch) {}

    void displayBalance_saving() const {
        cout << "Savings Account Balance: Rs" << balance << endl;
    }
};

class NotSavingAccount : public Account {
public:
    NotSavingAccount(double initialBalance, const string& accName, const string& accNumber, const string& accBranch)
        : Account(initialBalance, accName, accNumber, accBranch) {}

    void displayBalance_nonSaving() const {
        cout << "Non-Saving Account Balance: Rs" << balance << endl;
    }
};

// Forward declarations
bool accountExists(const vector<SavingsAccount>& savingsAccounts, const vector<NotSavingAccount>& notSavingAccounts, const string& accNumber);
void createAccount(vector<SavingsAccount>& savingsAccounts, vector<NotSavingAccount>& notSavingAccounts);
void displayAllAccountDetails(const vector<SavingsAccount>& savingsAccounts, const vector<NotSavingAccount>& notSavingAccounts);

int main() {
    int ch, check;
    vector<SavingsAccount> allSavingsAccounts;
    vector<NotSavingAccount> allNotSavingAccounts;

    string withdrawAccount;
    double withdrawAmount;

    auto findAccount = [&](const string& accNumber) -> pair<bool, Account*> {
        for (size_t i = 0; i < allSavingsAccounts.size(); ++i) {
            if (allSavingsAccounts[i].getAccountNumber() == accNumber) {
                return { true, &allSavingsAccounts[i] };
            }
        }

        for (size_t i = 0; i < allNotSavingAccounts.size(); ++i) {
            if (allNotSavingAccounts[i].getAccountNumber() == accNumber) {
                return { true, &allNotSavingAccounts[i] };
            }
        }

        return { false, nullptr };
    };

    do {
        cout << "\n1. Create a new account\n2. Deposit money in account\n3. Withdraw money\n";
        cout << "4. Display updated balance\nEnter your choice : ";
        cin >> ch;

        switch (ch) {
            case 1:
                createAccount(allSavingsAccounts, allNotSavingAccounts);
                break;

            case 2:
                {
                    string depositAccount;
                    double depositToSavings, depositToNotSaving;

                    cout << "Enter the account number to deposit money: ";
                    cin >> depositAccount;

                    auto accountInfo = findAccount(depositAccount);

                    if (accountInfo.first) {
                        cout << "Enter amount to deposit to Account Holder " << accountInfo.second->getName() << "'s Savings Account: Rs";
                        cin >> depositToSavings;

                        cout << "Enter amount to deposit to Account Holder " << accountInfo.second->getName() << "'s Non-Saving Account: Rs";
                        cin >> depositToNotSaving;

                        accountInfo.second->deposit(depositToSavings);
                        accountInfo.second->deposit(depositToNotSaving);
                    } else {
                        cout << "Invalid account number for deposit." << endl;
                    }
                }
                break;

            case 3:
                char transactionType;
                cout << "Enter the nature of the transaction (e: emergency, f: food): ";
                cin >> transactionType;

                cout << "Enter the account number from which you want to withdraw: ";
                cin >> withdrawAccount;

                switch (transactionType) {
                    case 'e':
                        {
                            auto accountInfo = findAccount(withdrawAccount);
                            if (accountInfo.first) {
                                cout << "Enter amount to withdraw from Account Holder " << accountInfo.second->getName() << " (Emergency): Rs";
                                cin >> withdrawAmount;
                                accountInfo.second->withdraw(withdrawAmount);
                            } else {
                                cout << "Invalid account number for withdrawal." << endl;
                            }
                        }
                        break;

                    case 'f':
                        {
                            auto accountInfo = findAccount(withdrawAccount);
                            if (accountInfo.first) {
                                double withdrawAmount;
                                cout << "Enter amount to withdraw for food from Account Holder " << accountInfo.second->getName() << "'s Non-Saving Account: Rs";
                                cin >> withdrawAmount;
                                accountInfo.second->withdraw(withdrawAmount);
                            } else {
                                cout << "Invalid account number for withdrawal." << endl;
                            }
                        }
                        break;

                    default:
                        cout << "Invalid transaction type." << endl;
                }
                break;

            case 4:
                displayAllAccountDetails(allSavingsAccounts, allNotSavingAccounts);
                break;

            default:
                cout << "Invalid operation.";
        }

        cout << "\nDo you want to continue (1/0) : ";
        cin >> check;
    } while (check != 0);

    return 0;
}

// Function definitions
bool accountExists(const vector<SavingsAccount>& savingsAccounts, const vector<NotSavingAccount>& notSavingAccounts, const string& accNumber) {
    auto findAccount = [&](const Account& acc) {
        return acc.getAccountNumber() == accNumber;
    };

    return (std::find_if(savingsAccounts.begin(), savingsAccounts.end(), findAccount) != savingsAccounts.end()) ||
           (std::find_if(notSavingAccounts.begin(), notSavingAccounts.end(), findAccount) != notSavingAccounts.end());
}

void createAccount(vector<SavingsAccount>& savingsAccounts, vector<NotSavingAccount>& notSavingAccounts) {
    double initialSavingsBalance, initialNotSavingBalance;
    string name, accountNumber, branch;

    cin.ignore(); // Ignore any newline characters in the input buffer

    // Verify and get the user's name
    bool validName = false;
    do {
        cout << "Enter your name: ";
        getline(cin, name);

        // Check if the name contains only alphabets
        validName = true;
        for (char c : name) {
            if (!isalpha(c) && !isspace(c)) {
                validName = false;
                cout << "Error: Please enter your name using only alphabets and spaces." << endl;
                break;
            }
        }
    } while (!validName);

    // Verify and get the bank account number
    bool validAccountNumber = false;
    do {
        cout << "Enter your bank account number (must be 6 digits and contain only numbers): ";
        cin >> accountNumber;

        // Check if the account number contains only numbers and is 6 digits long
        validAccountNumber = true;
        for (char c : accountNumber) {
            if (!isdigit(c)) {
                validAccountNumber = false;
                cout << "Error: Bank account number must only contain numbers." << endl;
                break;
            }
        }

        if (validAccountNumber && accountNumber.length() != 6) {
            validAccountNumber = false;
            cout << "Error: Bank account number must be exactly 6 digits long." << endl;
        }

        // Check if the account number already exists
        if (validAccountNumber && accountExists(savingsAccounts, notSavingAccounts, accountNumber)) {
            validAccountNumber = false;
            cout << "Error: An account with this account number already exists. Please choose a different account number." << endl;
        }
    } while (!validAccountNumber);

    // Verify and get the branch name
    bool validBranch = false;
    do {
        cout << "Enter your branch name: ";
        cin.ignore(); // Clear the buffer before reading the branch name
        getline(cin, branch);

        // Check if the branch name contains only alphabets
        validBranch = true;
        for (char c : branch) {
            if (!isalpha(c) && !isspace(c)) {
                validBranch = false;
                cout << "Error: Branch name must only contain alphabets and spaces." << endl;
                break;
            }
        }
    } while (!validBranch);

    cout << "Enter initial balance for Savings Account: Rs";
    cin >> initialSavingsBalance;

    cout << "Enter initial balance for Non-Saving Account: Rs";
    cin >> initialNotSavingBalance;

    SavingsAccount savingsAccount(initialSavingsBalance, name, accountNumber, branch);
    NotSavingAccount notSavingAccount(initialNotSavingBalance, name, accountNumber, branch);

    savingsAccounts.push_back(savingsAccount);
    notSavingAccounts.push_back(notSavingAccount);

    ofstream outFile("account_and_transaction_data.txt", ios::app);
    outFile << "Account Holder: " << name << endl;
    outFile << "Account Number: " << accountNumber << endl;
    outFile << "Branch: " << branch << endl;
    outFile << "Initial Savings Account Balance: Rs" << initialSavingsBalance << endl;
    outFile << "Initial Non-Saving Account Balance: Rs" << initialNotSavingBalance << endl << endl;
    outFile.close();

    cout << "Account created successfully!" << endl;
}

void displayAllAccountDetails(const vector<SavingsAccount>& savingsAccounts, const vector<NotSavingAccount>& notSavingAccounts) {
    cout << "\nAccount Details for All Account Holders:" << endl;
    for (size_t i = 0; i < savingsAccounts.size(); ++i) {
        cout << "Account Holder " << i + 1 << ":" << endl;
        savingsAccounts[i].displayAccountDetails();
        savingsAccounts[i].displayBalance_saving();
        notSavingAccounts[i].displayBalance_nonSaving();
        cout << "------------------------------------------" << endl;
    }
}
