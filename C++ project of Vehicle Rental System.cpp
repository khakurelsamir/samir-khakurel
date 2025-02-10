#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Account {
    string passwordHash;
    string uniqueID;
    double balance;
};

struct Rental {
    string vehicleType;
    string vehicleName;
    int days;
    double totalCost;
    time_t rentalDate; // Date when the vehicle was rented
};

map<string, Account> users;
map<string, Rental> rentals;

// Available vehicles in showroom
vector<string> offroadBikes = {"Royal Enfield Himalayan", "KTM Duke 390", "Hero Xpulse 200"};
vector<string> offroadCars = {"Jeep Wrangler", "Toyota Land Cruiser", "Ford Bronco"};
vector<string> cityBikes = {"Honda Activa", "Yamaha FZ", "Suzuki Access"};
vector<string> cityCars = {"Honda City", "Toyota Corolla", "Hyundai i20"};
vector<string> longDriveCars = {"Toyota Fortuner", "Ford Endeavour", "Mahindra XUV700"};
vector<string> luxuryCars = {"Mercedes S-Class", "BMW 7 Series", "Audi A8"};

map<string, double> rentalRates = {
    {"Bike", 10.0}, {"Car", 30.0}, {"Offroad Bike", 15.0}, {"Offroad Car", 50.0},
    {"Long Drive Car", 60.0}, {"Luxury Car", 150.0}
};

double penaltyRate = 20.0; // Penalty rate per day for late return

string hashPassword(const string &password) {
    string hash = "";
    for (int i = 0; i < password.length(); i++) {
        hash += to_string((password[i] + 3) % 256);
    }
    return hash;
}

string generateUniqueID() {
    srand(time(0));
    string id = "UID-";
    for (int i = 0; i < 8; i++) {
        id += to_string(rand() % 10);
    }
    return id;
}

void loadUsers() {
    ifstream file("projects.txt");
    if (!file) return;

    string username, passwordHash, uniqueID;
    double balance;
    while (file >> username >> passwordHash >> uniqueID >> balance) {
        users[username] = {passwordHash, uniqueID, balance};
    }
    file.close();
}

void saveUsers() {
    ofstream file("projects.txt");
    for (map<string, Account>::iterator it = users.begin(); it != users.end(); ++it) {
        file << it->first << " " << it->second.passwordHash << " " << it->second.uniqueID << " " << it->second.balance << endl;
    }
    file.close();
}

void signup() {
    string username, password;
    cout << "Enter a username: ";
    cin >> username;

    if (users.find(username) != users.end()) {
        cout << "Username already exists. Try another.\n";
        return;
    }

    cout << "Enter a password: ";
    cin >> password;

    string uniqueID = generateUniqueID();
    users[username] = {hashPassword(password), uniqueID, 500.0};  // Default balance: $500
    saveUsers();
    cout << "Signup successful! Your ID: " << uniqueID << ". Initial balance: $500\n";
}

bool login(string &loggedInUser) {
    string username, password;
    cout << "Enter your username: ";
    cin >> username;

    if (users.find(username) == users.end()) {
        cout << "User not found. Please sign up.\n";
        return false;
    }

    cout << "Enter your password: ";
    cin >> password;

    if (users[username].passwordHash == hashPassword(password)) {
        cout << "Login successful! Welcome, " << username << "!\n";
        loggedInUser = username;
        return true;
    } else {
        cout << "Incorrect password. Try again.\n";
        return false;
    }
}

void displayVehicles(const vector<string> &vehicles) {
    for (size_t i = 0; i < vehicles.size(); i++) {
        cout << i + 1 << ". " << vehicles[i] << endl;
    }
}

void rentVehicle(const string &username) {
    if (users.find(username) == users.end()) {
        cout << "Error: User not found.\n";
        return;
    }

    cout << "\nSelect Rental Purpose:\n";
    cout << "1. Off-Roading\n";
    cout << "2. City Trip\n";
    cout << "3. Long Drive\n";
    cout << "4. Luxury Ride\n";
    cout << "Choose rental purpose (1-4): ";
    
    int purpose;
    cin >> purpose;

    vector<string> selectedList;
    string vehicleType;
    double rate;

    if (purpose == 1) {
        cout << "Choose Vehicle Type: (1 for Bike, 2 for Car): ";
        int type;
        cin >> type;
        if (type == 1) {
            vehicleType = "Offroad Bike";
            selectedList = offroadBikes;
            rate = rentalRates[vehicleType];
        } else {
            vehicleType = "Offroad Car";
            selectedList = offroadCars;
            rate = rentalRates[vehicleType];
        }
    } else if (purpose == 2) {
        cout << "Choose Vehicle Type: (1 for Bike, 2 for Car): ";
        int type;
        cin >> type;
        if (type == 1) {
            vehicleType = "Bike";
            selectedList = cityBikes;
            rate = rentalRates[vehicleType];
        } else {
            vehicleType = "Car";
            selectedList = cityCars;
            rate = rentalRates[vehicleType];
        }
    } else if (purpose == 3) {
        vehicleType = "Long Drive Car";
        selectedList = longDriveCars;
        rate = rentalRates[vehicleType];
    } else if (purpose == 4) {
        vehicleType = "Luxury Car";
        selectedList = luxuryCars;
        rate = rentalRates[vehicleType];
    } else {
        cout << "Invalid selection.\n";
        return;
    }

    cout << "Available " << vehicleType << "s:\n";
    displayVehicles(selectedList);
    cout << "Choose a vehicle (1-" << selectedList.size() << "): ";
    
    int choice;
    cin >> choice;
    
    if (choice < 1 || choice > selectedList.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    string vehicleName = selectedList[choice - 1];

    cout << "Enter number of days to rent: ";
    int days;
    cin >> days;

    if (days <= 0) {
        cout << "Invalid number of days.\n";
        return;
    }

    double totalCost = rate * days;
    if (users[username].balance < totalCost) {
        cout << "Insufficient balance. Please add funds.\n";
        return;
    }

    users[username].balance -= totalCost;
    rentals[username] = {vehicleType, vehicleName, days, totalCost, time(0)}; // Store rental date
    saveUsers();

    cout << "Rental successful! You rented " << vehicleName << " for " << days << " days.\n";
    cout << "Total cost: $" << totalCost << ". Remaining balance: $" << users[username].balance << endl;
}

double calculatePenalty(const Rental &rental) {
    time_t now = time(0);
    double secondsPerDay = 86400; // Number of seconds in a day
    double daysRented = difftime(now, rental.rentalDate) / secondsPerDay;
    int daysLate = daysRented - rental.days;

    if (daysLate > 0) {
        return daysLate * penaltyRate;
    }
    return 0.0;
}

void returnVehicle(const string &username) {
    if (rentals.find(username) == rentals.end()) {
        cout << "No rental found for this user.\n";
        return;
    }

    Rental rental = rentals[username];
    double penalty = calculatePenalty(rental);

    if (penalty > 0) {
        cout << "You returned the vehicle " << (difftime(time(0), rental.rentalDate) / 86400 - rental.days) << " days late.\n";
        cout << "Penalty: $" << penalty << endl;
        if (users[username].balance < penalty) {
            cout << "Insufficient balance to pay the penalty. Please add funds.\n";
            return;
        }
        users[username].balance -= penalty;
    }

    rentals.erase(username);
    saveUsers();
    cout << "Vehicle returned successfully. Remaining balance: $" << users[username].balance << endl;
}

void showRentals() {
    cout << "\nCurrent Rentals:\n";
    for (map<string, Rental>::iterator it = rentals.begin(); it != rentals.end(); ++it) {
        cout << "User: " << it->first << " | Vehicle: " << it->second.vehicleName 
             << " | Days: " << it->second.days << " | Cost: $" << it->second.totalCost << endl;
    }
}

int main() {
    loadUsers();
    string loggedInUser = "";

    while (true) {
        cout << "\n1. Signup\n2. Login\n3. Rent Vehicle\n4. Show Rentals\n5. Return Vehicle\n6. Exit\nChoice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: signup(); break;
            case 2: if (login(loggedInUser)) {} break;
            case 3: if (loggedInUser != "") rentVehicle(loggedInUser);
                    else cout << "Please login first.\n"; break;
            case 4: showRentals(); break;
            case 5: if (loggedInUser != "") returnVehicle(loggedInUser);
                    else cout << "Please login first.\n"; break;
            case 6: return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}