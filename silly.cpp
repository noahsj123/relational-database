// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "TableEntry.h"
#include "silly.h"
#include <getopt.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

//      Use enum classes instead of comparing strings
//      use switch statements?

bool quiet_mode = false;

class Table {
    private:
        //REAL DATA
        vector <vector <TableEntry> > data;
        //METADATA
        vector<string> col_names;
        vector<EntryType> col_types;
        string name;
        size_t num_cols;
        size_t num_rows;
        //INDICES
        struct BstIndex {
            map<TableEntry, vector<size_t> > bst_map;
            string sorted_col_name;
            size_t sorted_col_num;
        };

        struct HashIndex {
            unordered_map<TableEntry, vector<size_t> > hash_map;
            string sorted_col_name;
            size_t sorted_col_num;
        };

        BstIndex bst_index;
        HashIndex hash_index;
        bool bst;
        bool hash;
        
        struct LessThan {
            TableEntry threshold;
            size_t index;
            bool operator()(const vector<TableEntry> &row) {
                return row[index] < threshold;
            }
            LessThan(TableEntry &threshold_in, size_t &index_in) : threshold(threshold_in), index(index_in) {}
        };

        struct GreaterThan {
            TableEntry threshold;
            size_t index;
            bool operator()(const vector<TableEntry> &row) {
                return row[index] > threshold;
            }
            GreaterThan(TableEntry &threshold_in, size_t &index_in) : threshold(threshold_in), index(index_in) {}
        };

        struct EqualTo {
            TableEntry threshold;
            size_t index;
            bool operator()(const vector<TableEntry> &row) {
                return row[index] == threshold;
            }
            EqualTo(TableEntry &threshold_in, size_t &index_in) : threshold(threshold_in), index(index_in) {}
        };
        
    public:
        Table() : name(""), num_cols(0), num_rows(0), bst(false), hash(false) {}

        size_t col_error_check(string col_name_in) {
            //TODO: Make 9999999 col_names.size() instead
            size_t index = 9999999;
            for (size_t i = 0; i < col_names.size(); ++i) {
                if (col_name_in == col_names[i]) {
                    index = i;
                    break;
                }
            }
            return index;
        }
            

        void create(string name_in) {
            name = name_in;
            size_t num_cols_in;
            cin >> num_cols_in;
            num_cols = num_cols_in;

            col_types.reserve(num_cols_in);
            col_names.reserve(num_cols_in);

            //TODO: Switch???
            string col_type_in;
            for (size_t i = 0; i < num_cols_in; ++i) {
                cin >> col_type_in;
                if (col_type_in.at(0) == 'b') {
                    col_types.push_back(EntryType::Bool);
                }
                else if (col_type_in.at(0) == 'i') {
                    col_types.push_back(EntryType::Int);
                }
                else if (col_type_in.at(0) == 's') {
                    col_types.push_back(EntryType::String);
                }
                else {
                    col_types.push_back(EntryType::Double);
                }
            }
            cout << "New table " << name << " with column(s) ";
            string col_name_in;
            for (size_t i = 0; i < num_cols; ++i) {
                cin >> col_name_in;
                col_names.push_back(col_name_in);
                cout << col_name_in << " ";
            }
            cout << "created\n";
        }

        void insert_into() {
            size_t num_old_rows = num_rows;
            size_t num_new_rows;
            cin >> num_new_rows;
            if (num_new_rows == 0) {
                cout << "Added " << num_new_rows << " rows to " << name << " from position " 
                 << num_old_rows << " to " << num_rows - 1 << '\n';
                string junk; 
                getline(cin, junk);
                return;
            }
            num_rows += num_new_rows;
            data.resize(num_rows);
            string junk; 
            cin >> junk;
            
            //READ IN ALL THE TABLE ENTRIES
            for (size_t row = num_old_rows; row < num_old_rows + num_new_rows; ++row) {
                data[row].reserve(num_cols);
                for (size_t col = 0; col < num_cols; ++col) {
                    //TODO: Switch statement?
                    if (col_types[col] == EntryType::String) {
                        string s;
                        cin >> s;
                        TableEntry entry(s);
                        data[row].emplace_back(entry);
                    }
                    else if (col_types[col] == EntryType::Int) {
                        int i;
                        cin >> i;
                        TableEntry entry(i);
                        data[row].emplace_back(entry);
                    }
                    else if (col_types[col] == EntryType::Double) {
                        double d;
                        cin >> d;
                        TableEntry entry(d);
                        data[row].emplace_back(entry);
                    }
                    else {
                        bool b;
                        cin >> b;
                        TableEntry entry(b);
                        data[row].emplace_back(entry);
                    }
                }
            }
            if (hash) {
                for (size_t row = num_old_rows; row < num_old_rows + num_new_rows; ++row) {
                    hash_index.hash_map[data[row][hash_index.sorted_col_num]].push_back(row);
                }
            }
            //TODO: ISSUE IS WITH BST HERE
            else if (bst) {
                for (size_t row = num_old_rows; row < num_rows; ++row) {
                    bst_index.bst_map[data[row][bst_index.sorted_col_num]].push_back(row);
                } 
            }

            cout << "Added " << num_new_rows << " rows to " << name << " from position " 
                 << num_old_rows << " to " << num_rows - 1 << '\n';
        }

        void print() {
            size_t num_cols_in;
            cin >> num_cols_in;
            vector<size_t> col_nums;
            col_nums.reserve(num_cols_in);

            vector<string> temp_col_names;

            for (size_t i = 0; i < num_cols_in; ++i) {
                string col_name_in;
                cin >> col_name_in;
                size_t index = col_error_check(col_name_in);
                if (index == 9999999) {
                    cout << "Error during PRINT: " << col_name_in << " does not name a column in " << name << '\n';
                    string junk;
                    getline(cin, junk);
                    return;
                }
                temp_col_names.push_back(col_name_in);
                col_nums.push_back(index);
            }

            string whereOrAll;
            cin >> whereOrAll;

            //PRINT ALL
            if (whereOrAll.at(0) == 'A') {
                if (!quiet_mode) {
                    for (size_t i = 0; i < temp_col_names.size(); ++i) {
                        cout << temp_col_names[i] << " ";
                    }
                    cout << '\n';
                }
                print_all(col_nums);
            }
            else {
                string param_col;
                cin >> param_col;
                char op;
                cin >> op;

                size_t index2 = col_error_check(param_col);
                if (index2 == 9999999) {
                    cout << "Error during PRINT: " << param_col << " does not name a column in " << name << '\n';
                    string junk;
                    getline(cin, junk);
                    return;
                }

                if (!quiet_mode) {
                    for (size_t i = 0; i < temp_col_names.size(); ++i) {
                        cout << temp_col_names[i] << " ";
                    }
                    cout << '\n';
                }

                char b_or_h = 'n';
                if (bst && bst_index.sorted_col_name == param_col) {
                    b_or_h = 'b';
                }
                else if (op == '=' && hash && hash_index.sorted_col_name == param_col){
                    b_or_h = 'h';
                }

                if (col_types[index2] == EntryType::String) {
                    string s;
                    cin >> s;
                    print_helper(op, s, index2, col_nums, b_or_h);
                }
                else if (col_types[index2] == EntryType::Double) {
                    double d;
                    cin >> d;
                    print_helper(op, d, index2, col_nums, b_or_h);
                }
                else if (col_types[index2] == EntryType::Int) {
                    int i;
                    cin >> i;
                    print_helper(op, i, index2, col_nums, b_or_h);
                }
                else {
                    bool b;
                    cin >> b;
                    print_helper(op, b, index2, col_nums, b_or_h);
                }
            }
        }
        template <typename T>
        void print_helper(char op, T &value, size_t index, vector<size_t> col_nums, char b_or_h) {
            TableEntry threshold(value);
            if (op == '<') {
                LessThan ltf(threshold, index);
                print_helper_helper(ltf, col_nums, b_or_h);
            }
            else if (op == '>') {
                GreaterThan gtf(threshold, index);
                print_helper_helper(gtf, col_nums, b_or_h); 
            }
            else {
                EqualTo etf(threshold, index);
                print_helper_helper(etf, col_nums, b_or_h);
            }
        }

        template <typename Functor>
        void print_helper_helper(Functor &func, vector<size_t> col_nums, char b_or_h) {
            size_t count = 0;
            //BST INDEX
            if (b_or_h == 'b') {
                for (auto it = bst_index.bst_map.begin(); it != bst_index.bst_map.end(); ++it) {
                    if (func(data[it->second[0]])) {
                        count += it->second.size();
                        if (!quiet_mode) {
                            for (size_t row = 0; row < it->second.size(); ++row) {
                                for (size_t col = 0; col < col_nums.size(); ++col) {
                                    cout << data[it->second[row]][col_nums[col]] << " ";
                                }
                                cout << '\n';
                                
                            }
                        }
                    }
                }
            }
            //HASH INDEX
            else if (b_or_h == 'h') {
                auto it = hash_index.hash_map.find(func.threshold);
                if (it == hash_index.hash_map.end()) {
                    cout << "Printed " << count << " matching rows from " << name << '\n';
                    return;
                }
                count += it->second.size();
                if (!quiet_mode) {
                    for (size_t row = 0; row < it->second.size(); ++row) {
                        for (size_t col = 0; col < col_nums.size(); ++col) {
                            cout << data[it->second[row]][col_nums[col]] << " ";                 
                        }
                        cout << '\n';
                    }  
                }
            }
            //NO INDEX
            else {
                for (size_t row = 0; row < data.size(); ++row) {
                    if (func(data[row])) {
                        ++count;
                        if (!quiet_mode) {
                            for (size_t col = 0; col < col_nums.size(); ++col) {
                                cout << data[row][col_nums[col]] << ' ';
                            }
                            cout << '\n';
                        }
                    }
                }
            }
            cout << "Printed " << count << " matching rows from " << name << '\n';
        }

        void print_all(vector<size_t> col_nums) {
            if (!quiet_mode) {
                for (size_t row = 0; row < num_rows; ++row) {
                    for (size_t col = 0; col < col_nums.size(); ++col) {
                        cout << data[row][col_nums[col]] << " ";
                    }
                    cout << '\n';
                }
            }
            cout << "Printed " << num_rows << " matching rows from " << name << '\n';
            return;
        }

        void delete_from() {
            string col_name_in;
            cin >> col_name_in;
            char op;
            cin >> op;

            size_t index = col_error_check(col_name_in);
            if (index == 9999999) {
                cout << "Error during DELETE: " << col_name_in << " does not name a column in " << name << '\n';
                string junk;
                getline(cin, junk);
                return;
            }

            if (col_types[index] == EntryType::String) {
                string s;
                cin >> s;
                delete_helper(op, s, index);
            }
            else if (col_types[index] == EntryType::Double) {
                double d;
                cin >> d;
                delete_helper(op, d, index);
            }
            else if (col_types[index] == EntryType::Int) {
                int i;
                cin >> i;
                delete_helper(op, i, index);
            }
            else {
                bool b;
                cin >> b;
                delete_helper(op, b, index);
            }
        }

        template <typename T>
        void delete_helper(char op, T &value, size_t index) {
            TableEntry threshold(value);
            if (op == '<') {
                LessThan ltf(threshold, index);
                delete_helper_helper(ltf);
            }
            else if (op == '>') {
                GreaterThan gtf(threshold, index);
                delete_helper_helper(gtf);
            }
            else {
                EqualTo etf(threshold, index);
                delete_helper_helper(etf);
            }
        }

        template <typename Functor>
        void delete_helper_helper(Functor &func) {
            auto first = data.begin();
            auto last =  data.end();
            auto newEnd = remove_if(first, last, func);
            size_t count = 0;
            for (auto it = newEnd; it != last; ++it) {
                ++count;
            }
            num_rows -= count;
            data.erase(newEnd, last);
            cout << "Deleted " << count << " rows from " << name << '\n';
            
            if (bst) {
                bst_index.bst_map.clear();
                for (size_t row = 0; row < num_rows; ++row) {
                    bst_index.bst_map[data[row][bst_index.sorted_col_num]].push_back(row);
                }
            }
            else if (hash) {
                hash_index.hash_map.clear();
                for (size_t row = 0; row < num_rows; ++row) {
                    hash_index.hash_map[data[row][hash_index.sorted_col_num]].push_back(row);
                }
            }
        }

        void generate_index() {
            //use enum class instead?
            string index_type;
            cin >> index_type;
            string junk;
            cin >> junk;
            cin >> junk;
            string col_name_in;
            cin >> col_name_in;
            
            size_t index = col_error_check(col_name_in);
            if (index == 9999999) {
                cout << "Error during GENERATE: " << col_name_in << " does not name a column in " << name << '\n';
                getline(cin, junk);
                return;
            }

            hash_index.hash_map.clear();
            bst_index.bst_map.clear();
            if (index_type.at(0) == 'b') {
                bst_index.sorted_col_num = index;
                bst_index.sorted_col_name = col_name_in;
                bst = true;
                hash = false;
                for (size_t row = 0; row < data.size(); ++row) {
                    TableEntry temp = data[row][index];
                    bst_index.bst_map[temp].push_back(row);
                }
                cout << "Created bst index for table " << name << " on column " << col_name_in << '\n';
            }
            else {
                hash_index.sorted_col_num = index;
                hash_index.sorted_col_name = col_name_in;
                hash = true;
                bst = false;
                for (size_t row = 0; row < data.size(); ++row) {
                    hash_index.hash_map[data[row][index]].push_back(row);
                }
                cout << "Created hash index for table " << name << " on column " << col_name_in << '\n';
            }
        }
        size_t get_num_rows() {
            return num_rows;
        }
        size_t get_num_cols() {
            return num_cols;
        }
        vector<string> get_col_names() {
            return col_names;
        }
        vector<EntryType> get_col_types() {
            return col_types;
        }
        vector <vector<TableEntry> > get_data() {
            return data;
        }
        HashIndex& get_hash_index() {
            return hash_index;
        }
        bool get_hash() {
            return hash;
        }
        
};

class DataBase {
    private:
        unordered_map<string, Table> my_tables;

    public:
        void join(string table1, string table2) {
            string junk;
            cin >> junk;
            string colname1; 
            cin >> colname1;
            size_t main_index1 = my_tables[table1].col_error_check(colname1);
            if (main_index1 == 9999999) {
                cout << "Error during JOIN: " << colname1 << " does not name a column in " << table1 << '\n';
                string junk;
                getline(cin, junk);
                return;
            }
            cin >> junk;
            string colname2; 
            cin >> colname2;
            size_t main_index2 = my_tables[table2].col_error_check(colname2);
            if (main_index2 == 9999999) {
                cout << "Error during JOIN: " << colname2 << " does not name a column in " << table2 << '\n';
                string junk;
                getline(cin, junk);
                return;
            }
            cin >> junk;
            cin >> junk;
            size_t num_cols;
            cin >> num_cols;

            pair<size_t, size_t> table_num_col_num;
            vector<pair<size_t, size_t> > col_indices;

            string print_colname;
            vector<string> temp_col_names;

            for (size_t i = 0; i < num_cols; ++i) {
                cin >> print_colname;
                size_t one_or_two;
                cin >> one_or_two;
                if (one_or_two == 1) {
                    table_num_col_num.first = 1;
                    size_t print_col_index = my_tables[table1].col_error_check(print_colname);
                    if (print_col_index == 9999999) {
                        cout << "Error during JOIN: " << print_colname << " does not name a column in " << table1 << '\n';
                        string junk;
                        getline(cin, junk);
                        return;
                    }
                    table_num_col_num.second = print_col_index;
                    col_indices.push_back(table_num_col_num);
                }
                else {
                    table_num_col_num.first = 2;
                    size_t print_col_index = my_tables[table2].col_error_check(print_colname);
                    if (print_col_index == 9999999) {
                        cout << "Error during JOIN: " << print_colname << " does not name a column in " << table2 << '\n';
                        string junk;
                        getline(cin, junk);
                        return;
                    }
                    table_num_col_num.second = print_col_index;
                    col_indices.push_back(table_num_col_num);
                }
                temp_col_names.push_back(print_colname);
            }
            if (!quiet_mode) {
                for (size_t i = 0; i < temp_col_names.size(); ++i) {
                    cout << temp_col_names[i] << " ";
                }
                cout << '\n';
            }

            vector<vector<TableEntry> > data1 = my_tables[table1].get_data();
            vector<vector<TableEntry> > data2 = my_tables[table2].get_data();
            size_t count = 0;
            if (my_tables[table2].get_hash() && my_tables[table2].get_hash_index().sorted_col_name == colname2) {
                for (size_t row = 0; row < data1.size(); ++row) {
                    //May need to use data2 below here
                    auto it2 = my_tables[table2].get_hash_index().hash_map.find(data1[row][main_index1]);
                    if (it2 != my_tables[table2].get_hash_index().hash_map.end()) {
                        count += it2->second.size();
                        if (!quiet_mode) {
                            for (auto iter = it2->second.begin(); iter != it2->second.end(); ++iter) {
                                for (size_t j = 0; j < col_indices.size(); ++j) {
                                    if (col_indices[j].first == 1) {
                                        cout << data1[row][col_indices[j].second] << " ";
                                    }
                                    else {
                                        cout << data2[*iter][col_indices[j].second] << " ";
                                    }
                                }
                                cout << '\n';
                            }
                        }
                    }
                }
            }
            else {
                unordered_map<TableEntry, vector<size_t>> temp_hash;
                temp_hash.clear();
                size_t index = my_tables[table2].col_error_check(colname2);
                for (size_t row = 0; row < data2.size(); ++row) {
                    temp_hash[data2[row][index]].push_back(row);
                }

                for (size_t row = 0; row < data1.size(); ++row) {
                    //May need to use data2 below here
                    auto temp_it = temp_hash.find(data1[row][main_index1]);
                    if (temp_it != temp_hash.end()) {
                        count += temp_it->second.size();
                        if (!quiet_mode) {
                            for (size_t i = 0; i < temp_it->second.size(); ++i) {
                                for (size_t j = 0; j < col_indices.size(); ++j) {
                                    if (col_indices[j].first == 1) {
                                        cout << data1[row][col_indices[j].second] << " ";
                                    }
                                    else {
                                        cout << data2[temp_it->second[i]][col_indices[j].second] << " ";
                                    }
                                }
                                cout << '\n';
                            }
                        }
                    }
                }
            }
            cout << "Printed " << count << " rows from joining " << table1 << " to " << table2 << '\n';
        }

        int check_table_exists(string name_in, string command_name) {
            auto it = my_tables.find(name_in);
            if (it == my_tables.end()) {
                string junk;
                getline(cin, junk);
                cout << "Error during " << command_name <<  ": " << name_in
                     << " does not name a table in the database\n";
                return - 1;
            }
            return 0;
        }

        int read_input() {
            string cmd;
            do {
                cout << "% ";
                cin >> cmd;
                string junk;
                string name_in;
                //process cmd

                if (cin.fail()) {
                    cout << "Error: Reading from cin has failed\n";
                    exit(1);
                }

                if (cmd.at(0) == 'Q') {
                    cout << "Thanks for being silly!\n";
                    return 0;
                }
                else if (cmd.at(0) == '#') {
                    getline(cin, junk);
                }
                else if (cmd.at(0) == 'D') {
                    cin >> junk;
                    cin >> name_in;
                    cin >> junk;
                    if (check_table_exists(name_in, "DELETE") == 0) {
                        my_tables[name_in].delete_from();
                    }                    
                    // getline(cin, junk);
                }
                else if (cmd.at(0) == 'G') {
                    cin >> junk;
                    cin >> name_in;
                    if (check_table_exists(name_in, "GENERATE") == 0) {
                        my_tables[name_in].generate_index();
                    }
                }
                else if (cmd.at(0) == 'C') {
                    cin >> name_in;
                    const auto &it = my_tables.find(name_in);
                    if (it != my_tables.end()) {
                        string junk;
                        cout << "Error during CREATE: Cannot create already existing table " 
                             << name_in << '\n';
                        getline(cin, junk);
                    }
                    else {
                        my_tables[name_in].create(name_in);
                    }
                }
                else if (cmd.at(0) == 'I') {
                    cin >> junk;
                    cin >> name_in;
                    //check if table name doesn't exist
                    if (check_table_exists(name_in, "INSERT") == 0) {
                        my_tables[name_in].insert_into();
                    }
                }
                else if (cmd.at(0) == 'P') {
                    cin >> junk;
                    cin >> name_in;
                    if (check_table_exists(name_in, "PRINT") == 0) {
                        my_tables[name_in].print();
                    }
                }
                else if (cmd.at(0) == 'J') {
                    string table1;
                    cin >> table1;
                    cin >> junk;
                    string table2;
                    cin >> table2;
                    if (check_table_exists(table1, "JOIN") == 0 && check_table_exists(table2, "JOIN") == 0) {
                        join(table1, table2);
                    }
                    // cin >> junk;
                    // getline(cin, junk);
                }
                else if (cmd.at(0) == 'R') {
                    cin >> name_in;
                    if (check_table_exists(name_in, "REMOVE") == 0) {
                        my_tables.erase(name_in);
                        cout << "Table " << name_in << " deleted\n";
                    }
                }
                else if (cmd != "CREATE" && cmd != "INSERT" && cmd != "DELETE" && cmd != "GENERATE" 
                        && cmd != "PRINT" && cmd != "JOIN" && cmd != "REMOVE" && cmd != "QUIT") {
                    
                    //TODO: Not accounting for the second word here-- ex. what if INSERT INTT is misspelled
                    //      must be a better way to do this
                    cout << "Error: unrecognized command\n";
                    getline(cin, junk);
                }
            } while (cmd != "QUIT");
            //will never reach this
            return 0;
        }

};

void get_options (int argc, char * argv[]) {
    int gotopt = 0;
    int option_index = 0;

    option long_opts [] = {
        {"help",  no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'}
    };

    while ((gotopt = getopt_long(argc, argv, "hq", long_opts, &option_index)) != -1) {
        switch (gotopt) {
        case 'h':
            cout << "helpful message\n";
            exit(0);
            break;
        case 'q':
            quiet_mode = true;
            break;
        default:
            cout << "Error: invalid option" << endl;
            exit(1);
        }
    }
}

int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    cin >> boolalpha;
    cout << boolalpha;
    
    get_options(argc, argv);
    DataBase db;
    return db.read_input();
}