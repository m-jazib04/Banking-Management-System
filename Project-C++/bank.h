#include <iostream>
#include <string>
#include <stack>
#include <queue>

#include <unordered_map>
#include <sstream>
#include <cmath>

#include <stdexcept> 
#include <cstdlib>

using namespace std;

struct TreeNode {
	int accountNumber;
	string name;
	string address;
	string contact;
	double balance;
	int pin;
	string username;  // Unique username for each user
	stack<string> transactionHistory; // Stack for transaction history
	TreeNode* left;
	TreeNode* right;

	TreeNode(int accNum, string name, string address, string contact, double balance, int pin, string username)
		: accountNumber(accNum), name(name), address(address), contact(contact), balance(balance), pin(pin), username(username), left(nullptr), right(nullptr) {}
};

class BankingSystem {
private:
	TreeNode* root;
	unordered_map<int, TreeNode*> accountMap; // Hash table for quick account lookup
	unordered_map<string, TreeNode*> usernameMap; // Hash table for username lookup
	int accountCounter = 1000; // Unique account number generator

	// Helper function to insert a node into the BST
	TreeNode* insert(TreeNode* node, TreeNode* newNode) {
		if (!node) return newNode;
		if (newNode->accountNumber < node->accountNumber) {
			node->left = insert(node->left, newNode);
		}
		else {
			node->right = insert(node->right, newNode);
		}
		return node;
	}

	// Helper function to search for an account in the BST
	TreeNode* search(TreeNode* node, int accountNumber) {
		if (!node || node->accountNumber == accountNumber) return node;
		if (accountNumber < node->accountNumber) return search(node->left, accountNumber);
		return search(node->right, accountNumber);
	}

	// ATM cash withdrawal
	void dispenseCash(double amount) {
		if (fmod(amount, 500) != 0) {
			cout << "Amount must be in multiples of 500!" << endl;
			char choice;
			cout << "Do you want to withdrwal by cheque? (y/n): ";
			cin >> choice;
			if (choice == 'y' || choice == 'Y') {
				cout << "Deposit by cheque option selected." << endl;
			}
			else {
				cout << "Please enter a valid amount in multiples of 500." << endl;
				system("cls"); // Clear the screen
			}
			return;
		}

		int notes5000 = 0, notes1000 = 0, notes500 = 0;

		try {
			while (amount >= 5000) {
				notes5000++;
				amount -= 5000;
			}
			while (amount >= 1000) {
				notes1000++;
				amount -= 1000;
			}
			while (amount >= 500) {
				notes500++;
				amount -= 500;
			}

			if (amount > 0) {
				throw runtime_error("ATM cannot dispense exact change for this amount!");
			}

			cout << "Dispensed: " << endl;
			cout << "5000 PKR notes: " << notes5000 << endl;
			cout << "1000 PKR notes: " << notes1000 << endl;
			cout << "500 PKR notes: " << notes500 << endl;
		}
		catch (const runtime_error& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Admin: Delete an account
	void deleteAccount(int accountNumber) {
		try {
			TreeNode* nodeToDelete = accountMap[accountNumber];
			if (nodeToDelete) {
				cout << "Account " << accountNumber << " deleted successfully!" << endl;
				accountMap.erase(accountNumber);
			}
			else {
				throw invalid_argument("Account not found!");
			}
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Admin: Update account details (e.g., address, contact)
	void updateAccountDetails(int accountNumber) {
		try {
			TreeNode* account = accountMap[accountNumber];
			if (account) {
				cout << "Updating details for Account " << accountNumber << endl;
				string newAddress, newContact;

				cout << "Enter new Address: ";
				cin.ignore();
				getline(cin, newAddress);
				cout << "Enter new Contact: ";
				getline(cin, newContact);

				account->address = newAddress;
				account->contact = newContact;

				cout << "Account details updated successfully!" << endl;
			}
			else {
				throw invalid_argument("Account not found!");
			}
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

public:
	BankingSystem() : root(nullptr) {}

	void createAccount() {
		try {
			string name, address, contact, username;
			double initialDeposit;
			int pin;

			cout << "Enter Full Name: ";
			cin.ignore();
			getline(cin, name);

			cout << "Enter Address: ";
			getline(cin, address);
			cout << "Enter Contact: ";
			getline(cin, contact);
			cout << "Enter Initial Deposit: ";
			cin >> initialDeposit;


			if (initialDeposit <= 0) {
				throw invalid_argument("Initial deposit must be greater than 0!");
			}

			cout << "Set a 4-digit PIN: ";
			cin >> pin;

			if (pin < 1000 || pin > 9999) {
				throw invalid_argument("PIN must be a 4-digit number!");
			}

			cout << "Enter Username (unique for each customer): ";
			cin >> username;

			if (usernameMap.find(username) != usernameMap.end()) {
				throw invalid_argument("Username already taken! Please choose a different one.");
			}

			int accountNumber = accountCounter++;
			TreeNode* newNode = new TreeNode(accountNumber, name, address, contact, initialDeposit, pin, username);
			root = insert(root, newNode);
			accountMap[accountNumber] = newNode;
			usernameMap[username] = newNode;

			cout << "Account created successfully! Your Account Number is: " << accountNumber << endl;
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Function to deposit cash
	void depositCash(TreeNode* account) {
		try {
			double amount;

			cout << "Enter Amount to Deposit: ";
			cin >> amount;

			if (amount <= 0) {
				throw invalid_argument("Deposit amount must be greater than zero!");
			}

			account->balance += amount;
			account->transactionHistory.push("Deposited: PKR." + to_string(amount));
			cout << "Deposit successful! New Balance: PKR." << account->balance << endl;
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Function to withdraw cash
	void withdrawCash(TreeNode* account) {
		try {
			double amount;

			cout << "Enter Amount to Withdraw: ";
			cin >> amount;

			if (amount <= 0) {
				throw invalid_argument("Withdrawal amount must be greater than zero!");
			}

			if (account->balance < amount) {
				throw runtime_error("Insufficient funds!");
			}

			account->balance -= amount;
			account->transactionHistory.push("Withdrew: PKR." + to_string(amount));
			dispenseCash(amount);
			cout << "Withdrawal successful! New Balance: PKR." << account->balance << endl;
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Function to transfer money
	void transferMoney(TreeNode* account) {
		try {
			int targetAccountNumber;
			double amount;

			cout << "Enter target Account Number: ";
			cin >> targetAccountNumber;
			cout << "Enter Amount to Transfer: ";
			cin >> amount;

			if (amount <= 0) {
				throw invalid_argument("Transfer amount must be greater than zero!");
			}

			if (account->balance < amount) {
				throw runtime_error("Insufficient funds!");
			}

			TreeNode* targetAccount = accountMap[targetAccountNumber];
			if (targetAccount) {
				account->balance -= amount;
				targetAccount->balance += amount;
				account->transactionHistory.push("Transferred: PKR." + to_string(amount) + " to Account " + to_string(targetAccountNumber));
				targetAccount->transactionHistory.push("Received: PKR." + to_string(amount) + " from Account " + to_string(account->accountNumber));
				cout << "Transfer successful! New Balance: PKR." << account->balance << endl;
			}
			else {
				throw invalid_argument("Target account not found!");
			}
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Function to view transaction history
	void viewTransactionHistory(TreeNode* account) {
		stack<string> tempHistory = account->transactionHistory;
		if (tempHistory.empty()) {
			cout << "No transactions found." << endl;
			return;
		}

		cout << "--- Transaction History ---" << endl;
		while (!tempHistory.empty()) {
			cout << tempHistory.top() << endl;
			tempHistory.pop();
		}
	}

	// Function to change PIN
	void changePin(TreeNode* account) {
		try {
			int newPin;

			cout << "Enter new 4-digit PIN: ";
			cin >> newPin;

			if (newPin < 1000 || newPin > 9999) {
				throw invalid_argument("PIN must be a 4-digit number!");
			}

			account->pin = newPin;
			cout << "PIN changed successfully!" << endl;
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
	}

	// Function to login as customer
	TreeNode* loginAsCustomer() {
		try {
			int accountNumber, pin;
			cout << "Enter Account Number: ";
			cin >> accountNumber;
			cout << "Enter PIN: ";
			cin >> pin;

			if (accountMap.find(accountNumber) == accountMap.end()) {
				throw invalid_argument("Account not found!");
			}

			TreeNode* account = accountMap[accountNumber];
			if (account->pin == pin) {
				cout << "Customer login successful!" << endl;
				return account;
			}
			else {
				throw invalid_argument("Incorrect PIN!");
			}
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
		return nullptr;
	}

	// Admin login
	bool loginAsAdmin() {
		try {
			string username, password;
			cout << "Enter Admin Username: ";
			cin >> username;
			cout << "Enter Admin Password: ";
			cin >> password;

			if (username == "admin" && password == "admin123") {
				cout << "Admin login successful!" << endl;
				return true;
			}
			throw invalid_argument("Invalid admin credentials!");
		}
		catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
		return false;
	}


	// Function to search account by username
	void searchAccountByUsername() {
		string username;
		cout << "Enter the username to search: ";
		cin >> username;

		// Iterate through all accounts to find the username
		TreeNode* foundAccount = nullptr;
		for (const auto& entry : accountMap) {
			if (entry.second->username == username) {
				foundAccount = entry.second;
				break;
			}
		}

		if (foundAccount) {
			cout << "Account Found!" << endl;
			cout << "Account Number: " << foundAccount->accountNumber << endl;
			cout << "Name: " << foundAccount->name << endl;
			cout << "Balance: $" << foundAccount->balance << endl;
		}
		else {
			cout << "Account not found with the given username!" << endl;
		}
	}

	void inOrder(TreeNode* node) {
		if (!node) return;
		inOrder(node->left);
		cout << "Account Number: " << node->accountNumber
			<< " | Name: " << node->name
			<< " | Balance: $" << node->balance << endl;
		inOrder(node->right);
	}

	void displayAllAccounts() {
		cout << "\n--- All Accounts (Sorted by Account Number) ---\n";
		inOrder(root);
	}


	// Function to process customer menu
	void customerMenu(TreeNode* account) {
		int choice;
		do {
			system("cls");
			cout << "\n--- Customer Menu ---\n";
			cout << "1. Deposit Cash\n";
			cout << "2. Withdraw Cash\n";
			cout << "3. Transfer Money\n";
			cout << "4. View Transaction History\n";
			cout << "5. Change PIN\n";
			cout << "6. Exit Customer Menu\n";
			cout << "Enter your choice: ";
			cin >> choice;

			switch (choice) {
			case 1:
				depositCash(account);
				break;
			case 2:
				withdrawCash(account);
				break;
			case 3:
				transferMoney(account);
				break;
			case 4:
				viewTransactionHistory(account);
				break;
			case 5:
				changePin(account);
				break;
			case 6:
				cout << "Exiting Customer Menu." << endl;
				break;
			default:
				cout << "Invalid choice. Please try again." << endl;
			}
			system("pause");
		} while (choice != 6);
	}

	// Function to process admin menu
	void adminMenu() {
		int choice;
		do {
			system("cls");
			cout << "\n--- Admin Menu ---\n";
			cout << "1. Delete Account\n";
			cout << "2. Update Account Details\n";
			cout << "3. View Account by Username\n";
			cout << "4. View All Accounts\n";
			cout << "4. Exit Admin Menu\n";
			cout << "Enter your choice: ";
			cin >> choice;

			int accountNumber;
			switch (choice) {
			case 1:
				cout << "Enter Account Number to Delete: ";
				cin >> accountNumber;
				deleteAccount(accountNumber);
				break;
			case 2:
				cout << "Enter Account Number to Update: ";
				cin >> accountNumber;
				updateAccountDetails(accountNumber);
				break;
			case 3:
				searchAccountByUsername();
				break;
			case 4:
				displayAllAccounts();
				break;
			case 5:
				cout << "Exiting Admin Menu." << endl;
				break;
			default:
				cout << "Invalid choice. Please try again." << endl;

			}
			system("pause");
		} while (choice != 4);
	}
	// Function to process the main menu
	void mainMenu() {
		int choice;
		do {
			cout << "\n--- Main Menu ---\n";
			cout << "1. Create New Account\n";
			cout << "2. Login as Customer\n";
			cout << "3. Login as Admin\n";
			cout << "4. Exit\n";
			cout << "Enter your choice: ";
			cin >> choice;

			switch (choice) {
			case 1:
				createAccount();
				break;
			case 2: {
						TreeNode* account = loginAsCustomer();
						if (account) {
							customerMenu(account);
						}
						break;
			}
			case 3:
				if (loginAsAdmin()) {
					adminMenu();
				}
				break;
			case 4:
				cout << "Exiting System." << endl;
				break;
			default:
				cout << "Invalid choice. Please try again." << endl;
			}
			system("pause");
		} while (choice != 4);
	}
};