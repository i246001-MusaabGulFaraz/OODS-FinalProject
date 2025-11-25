#include <bits/stdc++.h>
using namespace std;


// --------------------------- Base classes ---------------------------
class Person {
protected:
    string name;
    string contact;
public:
    Person() {}
    Person(string n, string c): name(n), contact(c) {}
    virtual ~Person() {}
    string getName() const { return name; }
    string getContact() const { return contact; }
    void setName(const string& n) { name = n; }
    void setContact(const string& c) { contact = c; }
};

class Customer : public Person {
    string customerID;
    string address;
public:
    Customer() {}
    Customer(string id, string n, string c, string addr)
        : Person(n, c), customerID(id), address(addr) {}
    string getID() const { return customerID; }
    string getAddress() const { return address; }
    void setAddress(const string& a) { address = a; }
    // Serialize to file line
    string serialize() const {
        return customerID + "|" + name + "|" + contact + "|" + address;
    }
    static Customer deserialize(const string& line) {
        stringstream ss(line);
        string id,n,c,a;
        getline(ss, id, '|');
        getline(ss, n, '|');
        getline(ss, c, '|');
        getline(ss, a, '|');
        return Customer(id,n,c,a);
    }
};
//@Musaab ive made the base class here you are responsible for implementing a merge sort by prority and dates later down in the code
//also to implement service routines by taking into account priority and making scheduling procedures/ mechanic assignment, any other implementation you think could better make it more user friendly
class Mechanic : public Person {
    string mechanicID;
    string specialization;
    bool available;
    int workload;
public:
    Mechanic() : available(true), workload(0) {}
    Mechanic(string id, string n, string c, string spec, bool av=true, int wl=0)
        : Person(n,c), mechanicID(id), specialization(spec), available(av), workload(wl) {}
    string getID() const { return mechanicID; }
    string getSpecialization() const { return specialization; }
    bool isAvailable() const { return available; }
    int getWorkload() const { return workload; }
    void assignWork() { workload++; available = (workload < 5); } // example logic
    void finishWork() { if(workload>0) workload--; available = (workload < 5); }

    string serialize() const {
        return mechanicID + "|" + name + "|" + contact + "|" + specialization + "|" + (available ? "1":"0") + "|" + to_string(workload);
    }
    static Mechanic deserialize(const string& line) {
        stringstream ss(line);
        string id,n,c,s,av,wl;
        getline(ss,id,'|'); getline(ss,n,'|'); getline(ss,c,'|');
        getline(ss,s,'|'); getline(ss,av,'|'); getline(ss,wl,'|');
        return Mechanic(id,n,c,s, av=="1", stoi(wl));
    }
};

// --------------------------- Vehicle ---------------------------
class Vehicle {
    string regNo;
    string model;
    string ownerCustomerID;
    string type; // Car, Motorcycle, Truck
public:
    Vehicle() {}
    Vehicle(string r, string m, string owner, string t) : regNo(r), model(m), ownerCustomerID(owner), type(t) {}
    string getReg() const { return regNo; }
    string getModel() const { return model; }
    string getOwner() const { return ownerCustomerID; }
    string getType() const { return type; }

    string serialize() const {
        return regNo + "|" + model + "|" + ownerCustomerID + "|" + type;
    }
    static Vehicle deserialize(const string& line) {
        stringstream ss(line);
        string r,m,o,t;
        getline(ss,r,'|'); getline(ss,m,'|'); getline(ss,o,'|'); getline(ss,t,'|');
        return Vehicle(r,m,o,t);
    }
};

// --------------------------- ServiceRequest ---------------------------
enum class Status { Pending, InProgress, Completed, Cancelled };

inline string statusToStr(Status s) {
    switch(s) {
        case Status::Pending: return "Pending";
        case Status::InProgress: return "InProgress";
        case Status::Completed: return "Completed";
        case Status::Cancelled: return "Cancelled";
    }
    return "Unknown";
}
inline Status strToStatus(const string& s) {
    if(s=="Pending") return Status::Pending;
    if(s=="InProgress") return Status::InProgress;
    if(s=="Completed") return Status::Completed;
    return Status::Cancelled;
}

class ServiceRequest {
    string requestID;
    string vehicleReg;
    string customerID;
    string date; // YYYY-MM-DD as text for simplicity
    string serviceType;
    int priority; // higher = more urgent
    Status status;
    double estimatedCost;
    string assignedMechanicID;
public:
    ServiceRequest() {}
    ServiceRequest(string rid, string vreg, string cid, string d, string st, int p, double cost)
        : requestID(rid), vehicleReg(vreg), customerID(cid), date(d),
          serviceType(st), priority(p), status(Status::Pending), estimatedCost(cost), assignedMechanicID("none") {}

    // getters/setters
    string getID() const { return requestID; }
    string getVehicle() const { return vehicleReg; }
    string getCustomer() const { return customerID; }
    string getDate() const { return date; }
    int getPriority() const { return priority; }
    Status getStatus() const { return status; }
    double getCost() const { return estimatedCost; }
    string getAssignedMechanic() const { return assignedMechanicID; }

    void setAssignedMechanic(const string& mid) { assignedMechanicID = mid; }
    void setStatus(Status s) { status = s; }

    // operator for priority queue (max-heap by priority)
    bool operator<(const ServiceRequest& other) const {
        // for priority_queue default comparator uses <, so for highest priority first:
        return priority < other.priority;
    }

    string serialize() const {
        // requestID|vehicleReg|customerID|date|type|priority|status|estimatedCost|assignedMechanicID
        return requestID + "|" + vehicleReg + "|" + customerID + "|" + date + "|" + serviceType + "|" +
               to_string(priority) + "|" + statusToStr(status) + "|" + to_string(estimatedCost) + "|" + assignedMechanicID;
    }

    static ServiceRequest deserialize(const string& line) {
        stringstream ss(line);
        string rid,vreg,cid,d,st,p,s,cost,mid;
        getline(ss,rid,'|'); getline(ss,vreg,'|'); getline(ss,cid,'|'); getline(ss,d,'|');
        getline(ss,st,'|'); getline(ss,p,'|'); getline(ss,s,'|'); getline(ss,cost,'|'); getline(ss,mid,'|');
        ServiceRequest r(rid,vreg,cid,d,st, stoi(p), stod(cost));
        r.setStatus(strToStatus(s));
        r.setAssignedMechanic(mid);
        return r;
    }
};

// --------------------------- Bill ---------------------------

// @Haidar Rahim ive made the base class edit it as you see fit keep the serialize and de serialize they are needed for saving data/reading
//some procedures to be implmenented could be if there is a customer who has done things more then a couple time for us add in a discount..
//also add in a report function that would let us view all the sales in a timeframe producing a report you will also be responsible for the sorting by cost mergesort
class Bill {
    string billID;
    string requestID;
    double subtotal;
    double tax;
    double total;
    bool paid;
public:
    Bill() {}
    Bill(string bid, string rid, double st) : billID(bid), requestID(rid), subtotal(st) {
        tax = subtotal * 0.10; // example 10%
        total = subtotal + tax;
        paid = false;
    }
    string serialize() const {
        return billID + "|" + requestID + "|" + to_string(subtotal) + "|" + to_string(tax) + "|" + to_string(total) + "|" + (paid ? "1":"0");
    }
    static Bill deserialize(const string& line) {
        stringstream ss(line);
        string bid, rid, st, tx, tot, pd;
        getline(ss,bid,'|'); getline(ss,rid,'|'); getline(ss,st,'|'); getline(ss,tx,'|'); getline(ss,tot,'|'); getline(ss,pd,'|');
        Bill b; b.billID = bid; b.requestID = rid; b.subtotal = stod(st); b.tax = stod(tx); b.total = stod(tot); b.paid = (pd=="1");
        return b;
    }
};

// --------------------------- Storage / Repositories ---------------------------
class Repository {
public:
    static vector<string> splitLine(const string& s, char delim='|') {
        vector<string> parts;
        string cur;
        stringstream ss(s);
        while (getline(ss, cur, delim)) parts.push_back(cur);
        return parts;
    }
    // Generic file-read helper returning lines (skips empty).
    static vector<string> readFileLines(const string& filename) {
        ifstream in(filename);
        vector<string> out;
        if(!in) return out;
        string line;
        while(getline(in, line)) {
            if(!line.empty()) out.push_back(line);
        }
        in.close();
        return out;
    }
    static void appendLine(const string& filename, const string& line) {
        ofstream out(filename, ios::app);
        out << line << "\n";
        out.close();
    }
    static void overwriteFile(const string& filename, const vector<string>& lines) {
        ofstream out(filename, ios::trunc);
        for(const auto &l: lines) out << l << "\n";
        out.close();
    }
};

// --------------------------- Service Center (Controller) ---------------------------
class ServiceCenter {
    unordered_map<string, Customer> customers;
    unordered_map<string, Vehicle> vehicles;
    unordered_map<string, Mechanic> mechanics; // by mechanicID
    vector<ServiceRequest> requests;
    unordered_map<string, Bill> bills;

    // filenames
    const string customersFile = "customers.txt";
    const string vehiclesFile = "vehicles.txt";
    const string mechanicsFile = "mechanics.txt";
    const string requestsFile = "requests.txt";
    const string billsFile = "bills.txt";

public:
    ServiceCenter() {
        loadAll();
    }

    // ---------- Persistence ----------
    void loadAll() {
        // Load customers
        for (auto &line : Repository::readFileLines(customersFile)) {
            Customer c = Customer::deserialize(line);
            customers[c.getID()] = c;
        }
        // Load vehicles
        for (auto &line : Repository::readFileLines(vehiclesFile)) {
            Vehicle v = Vehicle::deserialize(line);
            vehicles[v.getReg()] = v;
        }
        // Load mechanics
        for (auto &line : Repository::readFileLines(mechanicsFile)) {
            Mechanic m = Mechanic::deserialize(line);
            mechanics[m.getID()] = m;
        }
        // Load requests
        for (auto &line : Repository::readFileLines(requestsFile)) {
            ServiceRequest r = ServiceRequest::deserialize(line);
            requests.push_back(r);
        }
        // Load bills
        for (auto &line : Repository::readFileLines(billsFile)) {
            Bill b = Bill::deserialize(line);
            bills[b.serialize().substr(0,4)] = b; // crude mapping
        }
    }

    void persistCustomer(const Customer& c) {
        Repository::appendLine(customersFile, c.serialize());
        customers[c.getID()] = c;
    }
    void persistVehicle(const Vehicle& v) {
        Repository::appendLine(vehiclesFile, v.serialize());
        vehicles[v.getReg()] = v;
    }
    void persistMechanic(const Mechanic& m) {
        Repository::appendLine(mechanicsFile, m.serialize());
        mechanics[m.getID()] = m;
    }
    void persistRequest(const ServiceRequest& r) {
        Repository::appendLine(requestsFile, r.serialize());
        requests.push_back(r);
    }
    void persistBill(const Bill& b) {
        Repository::appendLine(billsFile, b.serialize());
        bills[b.serialize().substr(0,4)] = b;
    }

    // ---------- Simple operations ----------
    bool addCustomer(const Customer& c) {
        if(customers.count(c.getID())) return false;
        persistCustomer(c);
        return true;
    }
    bool addVehicle(const Vehicle& v) {
        if(vehicles.count(v.getReg())) return false;
        if(!customers.count(v.getOwner())) return false; // owner must exist
        persistVehicle(v);
        return true;
    }
    bool addMechanic(const Mechanic& m) {
        if(mechanics.count(m.getID())) return false;
        persistMechanic(m);
        return true;
    }
    bool createRequest(const ServiceRequest& r) {
        if(vehicles.find(r.getVehicle()) == vehicles.end()) return false;
        persistRequest(r);
        return true;
    }

    // assign mechanic to a request
    bool assignMechanicToRequest(const string& requestID) {
        // find request
        for (auto &req : requests) {
            if (req.getID() == requestID) {
                // choose mechanic with min workload and matching specialization @Musaab u can hard code specializations if needed 
                string best = "";
                int minWorkload = INT_MAX;
                for (auto &p : mechanics) {
                    Mechanic &m = p.second;
                    if (m.getWorkload() < minWorkload) {
                        minWorkload = m.getWorkload();
                        best = m.getID();
                    }
                }
                if (best.empty()) return false;
                req.setAssignedMechanic(best);
                req.setStatus(Status::InProgress);
                mechanics[best].assignWork();
                // rewrite requests file fully 
                flushRequestsToFile();
                flushMechanicsToFile();
                return true;
            }
        }
        return false;
    }

    // Generate simple bill @Haidar this is to be edited implement a customer loyalty scheme here.
    Bill generateBill(const string& requestID, double subtotal) {
        string billID = "B" + to_string(rand()%10000 + 1000);
        Bill b(billID, requestID, subtotal);
        persistBill(b);
        return b;
    }

    // Basic query: get requests for a customer
    vector<ServiceRequest> getRequestsByCustomer(const string& customerID) {
        vector<ServiceRequest> out;
        for (auto &r: requests) if (r.getCustomer() == customerID) out.push_back(r);
        return out;
    }

    // Sorting by cost date and priority to be implemented also integrate it into main menu and test
    static void mergeByCost(vector<ServiceRequest>& arr, int l, int m, int r) {
        //insert merge sort
    }

    static void mergeSortByCost(vector<ServiceRequest>& arr, int l, int r) {
        //insert merge sort here
    }

    // priority queue
    vector<ServiceRequest> getTopUrgentRequests(int k) {
        priority_queue<ServiceRequest> pq;
        for (const auto &r: requests) pq.push(r);
        vector<ServiceRequest> out;
        for (int i=0; i<k && !pq.empty(); ++i) {
            out.push_back(pq.top()); pq.pop();
        }
        return out;
    }

    // ---------- file rewrite helpers ----------
    void flushRequestsToFile() {
        vector<string> lines;
        for (auto &r: requests) lines.push_back(r.serialize());
        Repository::overwriteFile(requestsFile, lines);
    }
    void flushMechanicsToFile() {
        vector<string> lines;
        for (auto &p: mechanics) lines.push_back(p.second.serialize());
        Repository::overwriteFile(mechanicsFile, lines);
    }

    // ---------- Console helpers ----------
    void printCustomer(const Customer& c) {
        cout << c.getID() << " | " << c.getName() << " | " << c.getContact() << " | " << c.getAddress() << "\n";
    }
    void printMechanic(const Mechanic& m) {
        cout << m.getID() << " | " << m.getName() << " | " << m.getContact() << " | " << m.getSpecialization()
             << " | available: " << m.isAvailable() << " | workload: " << m.getWorkload() << "\n";
    }
    void printRequest(const ServiceRequest& r) {
        cout << r.getID() << " | " << r.getVehicle() << " | " << r.getCustomer() << " | " << r.getDate()
             << " | prio: " << r.getPriority() << " | status: " << statusToStr(r.getStatus())
             << " | cost: " << r.getCost() << " | mech: " << r.getAssignedMechanic() << "\n";
    }

    
    void listCustomers() {
        for (auto &p: customers) printCustomer(p.second);
    }
    void listMechanics() {
        for (auto &p: mechanics) printMechanic(p.second);
    }
    void listRequests() {
        for (auto &r: requests) printRequest(r);
    }
};

// --------------------------- Main Menu (simple) ---------------------------
void showMenu() {
    cout << "\n===== Vehicle Service Center =====\n";
    cout << "1. Add Customer\n2. Add Vehicle\n3. Add Mechanic\n4. Create Service Request\n5. Assign Mechanic to Request\n6. Generate Bill\n7. List Customers\n8. List Mechanics\n9. List Requests\n10. Top urgent requests\n0. Exit\nChoose: ";
}

int main() {
    ServiceCenter svc;
    while(true) {
        showMenu();
        int choice; if(!(cin >> choice)) { cin.clear(); cin.ignore(10000,'\n'); continue; }
        if (choice == 0) break;

        if (choice == 1) {
            string id,name,contact,address;
            cout << "customerID: "; cin >> id;
            cin.ignore();
            cout << "name: "; getline(cin, name);
            cout << "contact: "; getline(cin, contact);
            cout << "address: "; getline(cin, address);
            Customer c(id, name, contact, address);
            if (svc.addCustomer(c)) cout << "Customer added.\n"; else cout << "Add failed (id exists).\n";
        }
        else if (choice == 2) {
            string reg, model, owner, type;
            cout << "regNo: "; cin >> reg;
            cin.ignore();
            cout << "model: "; getline(cin, model);
            cout << "ownerCustomerID: "; getline(cin, owner);
            cout << "type: "; getline(cin, type);
            if (svc.addVehicle(Vehicle(reg, model, owner, type))) cout << "Vehicle added.\n"; else cout << "Add failed.\n";
        }
        else if (choice == 3) {
            string id,n,c,s;
            cout << "mechanicID: "; cin >> id; cin.ignore();
            cout << "name: "; getline(cin,n);
            cout << "contact: "; getline(cin,c);
            cout << "specialization: "; getline(cin,s);
            if (svc.addMechanic(Mechanic(id,n,c,s))) cout << "Mechanic added.\n"; else cout << "Add failed.\n";
        }
        else if (choice == 4) {
            string rid, vreg, cid, date, stype;
            int prio; double cost;
            cout << "requestID: "; cin >> rid;
            cout << "vehicleReg: "; cin >> vreg;
            cout << "customerID: "; cin >> cid;
            cout << "date(YYYY-MM-DD): "; cin >> date;
            cout << "serviceType: "; cin >> stype;
            cout << "priority(int): "; cin >> prio;
            cout << "estimatedCost: "; cin >> cost;
            if (svc.createRequest(ServiceRequest(rid, vreg, cid, date, stype, prio, cost))) cout << "Request created.\n"; else cout << "Create failed.\n";
        }
        else if (choice == 5) {
            string rid; cout << "requestID to assign: "; cin >> rid;
            if (svc.assignMechanicToRequest(rid)) cout << "Mechanic assigned.\n"; else cout << "Assignment failed.\n";
        }
        else if (choice == 6) {
            string rid; double subtotal;
            cout << "requestID: "; cin >> rid;
            cout << "subtotal amount: "; cin >> subtotal;
            Bill b = svc.generateBill(rid, subtotal);
            cout << "Bill generated: " << b.serialize() << "\n";
        }
        else if (choice == 7) svc.listCustomers();
        else if (choice == 8) svc.listMechanics();
        else if (choice == 9) svc.listRequests();
        else if (choice == 10) {
            int k; cout << "k: "; cin >> k;
            auto top = svc.getTopUrgentRequests(k);
            for (auto &r: top) svc.printRequest(r);
        }
        else cout << "Not implemented.\n";
    }
    cout << "Goodbye.\n";
    return 0;
}
