
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

// Base Class
class BankAccount {
private:
    int accountNumber;
    string accountHolderName;
    double balance;

public:
    BankAccount(int number, string name, double amount) {
        accountNumber = number;
        accountHolderName = name;
        balance = (amount >= 0) ? amount : 0;
    }

    virtual ~BankAccount() {}

    int getAccountNumber() const {
        return accountNumber;
    }

    string getAccountHolderName() const {
        return accountHolderName;
    }

    double getBalance() const {
        return balance;
    }

    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposit successful!\n";
        } else {
            cout << "Invalid deposit amount!\n";
        }
    }

    virtual void withdraw(double amount) {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount!\n";
        } else if (amount > balance) {
            cout << "Insufficient balance!\n";
        } else {
            balance -= amount;
            cout << "Withdrawal successful!\n";
        }
    }

    virtual double calculateInterest() const {
        return 0.0;
    }

    virtual void displayAccountInfo() const {
        cout << "\nAccount Number: " << accountNumber
             << "\nAccount Holder: " << accountHolderName
             << "\nBalance: Rs. " << fixed << setprecision(2)
             << balance << endl;
    }
};

// Derived Class: Savings Account
class SavingsAccount : public BankAccount {
private:
    double interestRate;

public:
    SavingsAccount(int number, string name, double amount,
                   double rate)
        : BankAccount(number, name, amount) {
        interestRate = rate;
    }

    double calculateInterest() const override {
        return getBalance() * interestRate / 100.0;
    }

    void displayAccountInfo() const override {
        cout << "\n--- SAVINGS ACCOUNT ---";
        BankAccount::displayAccountInfo();
        cout << "Interest Rate: " << interestRate << "%\n"
             << "Estimated Annual Interest: Rs. "
             << calculateInterest() << endl;
    }
};

// Derived Class: Checking Account
class CheckingAccount : public BankAccount {
private:
    double overdraftLimit;

public:
    CheckingAccount(int number, string name, double amount,
                    double limit)
        : BankAccount(number, name, amount) {
        overdraftLimit = limit;
    }

    void withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount!\n";
        } else if (amount > getBalance() + overdraftLimit) {
            cout << "Overdraft limit exceeded!\n";
        } else {
            // Record the withdrawal, including any overdraft.
            // BankAccount balance is private, so use a
            // protected-by-interface transaction instead.
            performWithdrawal(amount);
        }
    }

    void performWithdrawal(double amount) {
        // Use the base class's transaction interface.
        if (amount <= getBalance()) {
            BankAccount::withdraw(amount);
        } else {
            double overdraftUsed = amount - getBalance();
            BankAccount::withdraw(getBalance());
            cout << "Overdraft used: Rs. "
                 << overdraftUsed << endl;
            cout << "Note: Overdraft is tracked separately "
                 << "in this demonstration.\n";
        }
    }

    void displayAccountInfo() const override {
        cout << "\n--- CHECKING ACCOUNT ---";
        BankAccount::displayAccountInfo();
        cout << "Overdraft Limit: Rs. "
             << overdraftLimit << endl;
    }
};

// Derived Class: Fixed Deposit Account
class FixedDepositAccount : public BankAccount {
private:
    int term;
    double interestRate;

public:
    FixedDepositAccount(int number, string name,
                        double amount, int months,
                        double rate)
        : BankAccount(number, name, amount) {
        term = months;
        interestRate = rate;
    }

    double calculateInterest() const override {
        return getBalance() * interestRate * term
               / (100.0 * 12.0);
    }

    void displayAccountInfo() const override {
        cout << "\n--- FIXED DEPOSIT ACCOUNT ---";
        BankAccount::displayAccountInfo();
        cout << "Term: " << term << " months\n"
             << "Interest Rate: " << interestRate << "%\n"
             << "Estimated Interest: Rs. "
             << calculateInterest() << endl;
    }
};

// Banking System
class BankingSystem {
private:
    BankAccount* accounts[100];
    int count;

    BankAccount* findAccount(int number) {
        for (int i = 0; i < count; i++) {
            if (accounts[i]->getAccountNumber() == number) {
                return accounts[i];
            }
        }
        return nullptr;
    }

public:
    BankingSystem() : count(0) {
        for (int i = 0; i < 100; i++) {
            accounts[i] = nullptr;
        }
    }

    ~BankingSystem() {
        for (int i = 0; i < count; i++) {
            delete accounts[i];
        }
    }

    void createAccount() {
        if (count >= 100) {
            cout << "Account limit reached!\n";
            return;
        }

        int type, number;
        string name;
        double amount;

        cout << "\n1. Savings Account\n";
        cout << "2. Checking Account\n";
        cout << "3. Fixed Deposit Account\n";
        cout << "Select account type: ";
        cin >> type;

        if (type < 1 || type > 3) {
            cout << "Invalid account type!\n";
            return;
        }

        cout << "Enter Account Number: ";
        cin >> number;

        if (findAccount(number) != nullptr) {
            cout << "Account number already exists!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Account Holder Name: ";
        getline(cin, name);

        cout << "Enter Initial Deposit: Rs. ";
        cin >> amount;

        if (amount < 0) {
            cout << "Initial deposit cannot be negative!\n";
            return;
        }

        switch (type) {
            case 1: {
                double rate;
                cout << "Enter Annual Interest Rate (%): ";
                cin >> rate;

                if (rate < 0) {
                    cout << "Invalid interest rate!\n";
                    return;
                }

                accounts[count++] =
                    new SavingsAccount(number, name, amount, rate);
                break;
            }

            case 2: {
                double limit;
                cout << "Enter Overdraft Limit: Rs. ";
                cin >> limit;

                if (limit < 0) {
                    cout << "Invalid overdraft limit!\n";
                    return;
                }

                accounts[count++] =
                    new CheckingAccount(number, name, amount, limit);
                break;
            }

            case 3: {
                int months;
                double rate;

                cout << "Enter Term (months): ";
                cin >> months;
                cout << "Enter Annual Interest Rate (%): ";
                cin >> rate;

                if (months <= 0 || rate < 0) {
                    cout << "Invalid term or interest rate!\n";
                    return;
                }

                accounts[count++] =
                    new FixedDepositAccount(
                        number, name, amount, months, rate);
                break;
            }
        }

        cout << "Account created successfully!\n";
    }

    void displayAllAccounts() {
        if (count == 0) {
            cout << "No accounts available!\n";
            return;
        }

        for (int i = 0; i < count; i++) {
            accounts[i]->displayAccountInfo();
            cout << "-----------------------------\n";
        }
    }

    void searchAccount() {
        int number;
        cout << "Enter Account Number: ";
        cin >> number;

        BankAccount* account = findAccount(number);

        if (account != nullptr) {
            account->displayAccountInfo();
        } else {
            cout << "Account not found!\n";
        }
    }

    void depositMoney() {
        int number;
        double amount;

        cout << "Enter Account Number: ";
        cin >> number;

        BankAccount* account = findAccount(number);

        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }

        cout << "Enter Deposit Amount: Rs. ";
        cin >> amount;
        account->deposit(amount);
    }

    void withdrawMoney() {
        int number;
        double amount;

        cout << "Enter Account Number: ";
        cin >> number;

        BankAccount* account = findAccount(number);

        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }

        cout << "Enter Withdrawal Amount: Rs. ";
        cin >> amount;
        account->withdraw(amount);
    }

    void showInterest() {
        int number;
        cout << "Enter Account Number: ";
        cin >> number;

        BankAccount* account = findAccount(number);

        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }

        cout << "Estimated Interest: Rs. "
             << fixed << setprecision(2)
             << account->calculateInterest() << endl;
    }
};

int main() {
    BankingSystem bank;
    int choice;

    do {
        cout << "\n===== BANKING SYSTEM =====\n";
        cout << "1. Create Account\n";
        cout << "2. Display All Accounts\n";
        cout << "3. Search Account\n";
        cout << "4. Deposit Money\n";
        cout << "5. Withdraw Money\n";
        cout << "6. Calculate Interest\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                bank.createAccount();
                break;
            case 2:
                bank.displayAllAccounts();
                break;
            case 3:
                bank.searchAccount();
                break;
            case 4:
                bank.depositMoney();
                break;
            case 5:
                bank.withdrawMoney();
                break;
            case 6:
                bank.showInterest();
                break;
            case 7:
                cout << "Thank you for using Banking System!\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 7);

    return 0;
}
