#include <iostream>
#include <queue>
#include <stack>
#include <list>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// ================= Ticket =================
struct Ticket
{
    int id;
    string firstName;
    string middleName;
    string lastName;
    string collegeID; // 9-digit college ID
    string service;
    string studentType;
    string imagePath; // NEW: Path to student's image (for web phase)

    int typePriority;
    int importance;
    int isVIP;

    int arrivalTime;
    int deadlineDays;
};

// ================= BST Node =================
struct TreeNode {
    Ticket data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Ticket t) : data(t), left(nullptr), right(nullptr) {}
};

// ================= Global =================
int counter = 0;
int timeCounter = 0;
list<Ticket> ticketList;
TreeNode* bstRoot = nullptr; // Global BST Root

// ================= Compare for Priority Queue =================
struct Compare
{
    bool operator()(Ticket a, Ticket b)
    {
        if (a.typePriority != b.typePriority)
            return a.typePriority < b.typePriority;
        if (a.importance != b.importance)
            return a.importance < b.importance;
        if (a.isVIP != b.isVIP)
            return a.isVIP < b.isVIP;
        return a.arrivalTime > b.arrivalTime;
    }
};

priority_queue<Ticket, vector<Ticket>, Compare> pq;
stack<Ticket> undoStack;

// ================= BST Functions =================
TreeNode* insertBST(TreeNode* root, Ticket t) {
    if (root == nullptr) return new TreeNode(t);
    if (t.id < root->data.id)
        root->left = insertBST(root->left, t);
    else
        root->right = insertBST(root->right, t);
    return root;
}

TreeNode* searchBST(TreeNode* root, int id) {
    if (root == nullptr || root->data.id == id)
        return root;
    if (id < root->data.id)
        return searchBST(root->left, id);
    return searchBST(root->right, id);
}

// Helper to remove from BST (simplified for this project)
TreeNode* deleteBST(TreeNode* root, int id) {
    if (root == nullptr) return root;
    if (id < root->data.id)
        root->left = deleteBST(root->left, id);
    else if (id > root->data.id)
        root->right = deleteBST(root->right, id);
    else {
        if (root->left == nullptr) {
            TreeNode* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            TreeNode* temp = root->left;
            delete root;
            return temp;
        }
        // Node with two children: Get inorder successor
        TreeNode* temp = root->right;
        while (temp->left != nullptr) temp = temp->left;
        root->data = temp->data;
        root->right = deleteBST(root->right, temp->data.id);
    }
    return root;
}

// ================= Binary Search =================
int binarySearch(vector<Ticket>& vec, string targetID) {
    int low = 0, high = vec.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (vec[mid].collegeID == targetID) return mid;
        if (vec[mid].collegeID < targetID) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// ================= Functions =================

void printLogo() {
    cout << "  ____   ____   ____  _______     ____   ____   ____  _______  " << endl;
    cout << " /    \\ /    \\ /    \\|       \\   /    \\ /    \\ /    \\|       \\ " << endl;
    cout << "|  AA  |  AA  |  SS  |   TT    | |  AA  |  AA  |  SS  |   TT    |" << endl;
    cout << "|  __  |  __  |  __  |   __    | |  __  |  __  |  __  |   __    |" << endl;
    cout << "|_|  |_|_|  |_|______|__|  |___| |_|  |_|_|  |_|______|__|  |___|" << endl;
    cout << "            WELCOME TO AAST UNIVERSITY PORTAL                   " << endl;
    cout << "================================================================" << endl;
}

int getServicePriority(string s)
{
    if (s == "emergency") return 5;
    if (s == "government") return 4;
    if (s == "academic") return 3;
    if (s == "financial") return 2;
    return 1;
}

int baseImportance(string s)
{
    if (s == "emergency") return 5;
    if (s == "academic") return 4;
    if (s == "government") return 3;
    if (s == "financial") return 2;
    return 1;
}

void rebuildQueue()
{
    while (!pq.empty()) pq.pop();
    for (auto &t : ticketList)
    {
        int waiting = timeCounter - t.arrivalTime;
        t.importance += waiting / 5;
        pq.push(t);
    }
}

// ================= Book Ticket =================
void bookTicket()
{
    Ticket t;
    counter++;
    timeCounter++;

    t.id = counter;
    t.arrivalTime = timeCounter;

    cout << "\nEnter First Name: ";
    cin >> t.firstName;
    cout << "Enter Middle Name: ";
    cin >> t.middleName;
    cout << "Enter Last Name: ";
    cin >> t.lastName;
    cout << "Enter College ID (9 digits): ";
    cin >> t.collegeID;

    if (t.collegeID.length() != 9)
    {
        cout << "Invalid College ID! Must be exactly 9 digits.\n";
        counter--;
        timeCounter--;
        return;
    }

    cout << "Service (emergency/government/academic/financial/normal): ";
    cin >> t.service;

    t.typePriority = getServicePriority(t.service);
    t.importance = baseImportance(t.service);

    cout << "Student Type (normal/final/graduate): ";
    cin >> t.studentType;

    if (t.studentType == "graduate") t.importance += 2;
    else if (t.studentType == "final") t.importance += 1;

    cout << "Is VIP? (1=Yes / 0=No): ";
    cin >> t.isVIP;

    cout << "Days before deadline: ";
    cin >> t.deadlineDays;

    // Simulate image path
    t.imagePath = "images/" + t.collegeID + ".jpg";

    if (t.deadlineDays <= 1) t.importance += 2;
    else if (t.deadlineDays <= 3) t.importance += 1;

    if (t.typePriority == 5) cout << "Emergency Case!\n";
    if (ticketList.size() > 7) cout << "⚠ System Busy!\n";

    // Add to structures
    ticketList.push_back(t);
    pq.push(t);
    undoStack.push(t);
    bstRoot = insertBST(bstRoot, t); // Insert into BST

    cout << "Ticket #" << t.id << " Booked successfully.\n";
}

// ================= Serve =================
void serveNext()
{
    if (pq.empty())
    {
        cout << "No Tickets\n";
        return;
    }

    Ticket t = pq.top();
    pq.pop();

    ticketList.remove_if([&](Ticket x) { return x.id == t.id; });
    bstRoot = deleteBST(bstRoot, t.id); // Remove from BST

    cout << "\nServing Ticket #" << t.id
         << " | Name: " << t.firstName << " " << t.middleName << " " << t.lastName
         << " | College ID: " << t.collegeID
         << " | Service: " << t.service << endl;
    cout << "Image Path: " << t.imagePath << endl;
}

// ================= Search (Binary Search & BST) =================
void searchTicket() {
    int method;
    cout << "\nSearch Method:\n1. BST (by Ticket ID)\n2. Binary Search (by College ID)\nChoice: ";
    cin >> method;

    if (method == 1) {
        int id;
        cout << "Enter Ticket ID to search: ";
        cin >> id;
        TreeNode* res = searchBST(bstRoot, id);
        if (res) {
            cout << "Found! Name: " << res->data.firstName << " " << res->data.lastName << " | Service: " << res->data.service << endl;
        } else {
            cout << "Ticket ID not found in BST.\n";
        }
    } else if (method == 2) {
        string cid;
        cout << "Enter College ID to search: ";
        cin >> cid;
        
        // Prepare sorted vector for binary search
        vector<Ticket> vec(ticketList.begin(), ticketList.end());
        sort(vec.begin(), vec.end(), [](Ticket a, Ticket b) {
            return a.collegeID < b.collegeID;
        });

        int idx = binarySearch(vec, cid);
        if (idx != -1) {
            cout << "Found! Ticket ID: " << vec[idx].id << " | Name: " << vec[idx].firstName << " " << vec[idx].lastName << endl;
        } else {
            cout << "College ID not found using Binary Search.\n";
        }
    }
}

// ================= Cancel =================
void cancelTicket()
{
    string collegeID;
    cout << "Enter College ID: ";
    cin >> collegeID;

    bool found = false;
    for (auto it = ticketList.begin(); it != ticketList.end(); ++it)
    {
        if (it->collegeID == collegeID)
        {
            bstRoot = deleteBST(bstRoot, it->id); // Sync BST
            ticketList.erase(it);
            found = true;
            break;
        }
    }

    if (found)
    {
        rebuildQueue();
        cout << "Cancelled Successfully\n";
    }
    else cout << "Not Found\n";
}

// ================= Undo =================
void undoLast()
{
    if (undoStack.empty())
    {
        cout << "Nothing to Undo\n";
        return;
    }

    Ticket t = undoStack.top();
    undoStack.pop();

    ticketList.remove_if([&](Ticket x) { return x.id == t.id; });
    bstRoot = deleteBST(bstRoot, t.id); // Sync BST
    rebuildQueue();

    cout << "Undo Ticket #" << t.id << endl;
}

// ================= Status =================
void showStatus()
{
    cout << "\n===== STATUS =====\n";
    cout << "Total Tickets: " << ticketList.size() << endl;
    if (ticketList.empty()) cout << "No tickets to display.\n";
    else
    {
        for (const auto &t : ticketList)
        {
            cout << "ID: " << t.id << " | " << t.firstName << " " << t.lastName 
                 << " | ID: " << t.collegeID << " | Service: " << t.service << endl;
        }
    }
    cout << "==================\n";
}

// ================= Main =================
int main()
{
    printLogo(); // Display "Image"
    int choice;

    do
    {
        cout << "\n===== SMART UNIVERSITY SYSTEM =====\n";
        cout << "1. Book Ticket\n";
        cout << "2. Serve Next\n";
        cout << "3. Cancel Ticket\n";
        cout << "4. Undo\n";
        cout << "5. Show Status\n";
        cout << "6. Search Ticket (BST/Binary Search)\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1: bookTicket(); break;
        case 2: serveNext(); break;
        case 3: cancelTicket(); break;
        case 4: undoLast(); break;
        case 5: showStatus(); break;
        case 6: searchTicket(); break;
        }

    } while (choice != 7);

    return 0;
}

}