#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "json.hpp"
#include <regex>
#include <random>

// Используем псевдоним для удобства
using json = nlohmann::json;
using namespace std;
random_device rd;

// Предварительное объявление
bool matchDocument(const json& doc, const json& query);

bool checkCondition(const json& value, const json& condition) {
    cout << condition.dump(4) << endl;
    // Поддержка вложенных структур и прямого сравнения
    if (!condition.is_object()) {
        return value == condition; 
    }

    // Проверяем, является ли condition набором операторов ($gt, $lt...) или вложенным объектом
    bool isOperatorQuery = false;
    for (auto& [key, val] : condition.items()) {
        if (key[0] == '$') {
            isOperatorQuery = true;
            break;
        }
    }

    // Если это просто вложенный объект (например {"subfield": "val"}), а не оператор
    if (!isOperatorQuery) {
        if (value.is_object()) {
            return matchDocument(value, condition); // Рекурсия для вложенных полей
        }
        return value == condition; // Прямое сравнение объектов (полное совпадение)
    }

    // Обработка операторов
    for (auto& [op, arg] : condition.items()) {
        if (op == "$eq") { if (value != arg) return false; }
        else if (op == "$ne") { if (value == arg) return false; }
        else if (op == "$gt") { if (value <= arg) return false; }
        else if (op == "$lt") { if (value >= arg) return false; }
        else if (op == "$gte") { if (value < arg) return false; }
        else if (op == "$lte") { if (value > arg) return false; }
        else if (op == "$in") { 
            bool found = false;
            for (const auto& item : arg) {
                cout << item.dump(4) << endl;
                cout << value.dump(4) << endl;
                if (item == value) { found = true; break; }
            }
            if (!found) return false;
        }
        else if (op == "$not") { 
            if (checkCondition(value, arg)) return false; 
        }
    }
    return true;
}

bool matchDocument(const json& doc, const json& query) {
    if (query.empty()) return true;

    // Логические операторы верхнего уровня
    if (query.contains("$and")) {
        for (const auto& subQuery : query["$and"]) {
            if (!matchDocument(doc, subQuery)) return false;
        }
        return true;
    }
    if (query.contains("$or")) {
        for (const auto& subQuery : query["$or"]) {
            if (matchDocument(doc, subQuery)) return true;
        }
        return false;
    }

    // Проверка полей
    for (auto& [key, condition] : query.items()) {
        if (key[0] == '$') continue; 

        // 3) Корректная обработка отсутствующих полей
        if (!doc.contains(key)) {
             // Если мы проверяем $ne (не равно) или $not, отсутствие поля может быть валидным
             // Для упрощения передаем null, если поля нет
             if (!checkCondition(nullptr, condition)) return false;
        } else {
             if (!checkCondition(doc[key], condition)) return false;
        }
    }
    return true;
}

class Collection {
    string name;
    string path;
    string schemaConfigPath; // Путь к глобальному файлу schema.json
    int tuples_limit;
    json structure; 

    string generateId() {
        mt19937 gen(rd());
        return to_string(chrono::system_clock::now().time_since_epoch().count()) + "_" + to_string(gen());
    }

    vector<int> getFileIndexes() {
        vector<int> indexes;
        if (!filesystem::exists(path)) return {1}; // Защита
        
        for (const auto& entry : filesystem::directory_iterator(path)) {
            string fname = entry.path().filename().string();
            if (fname.find(".json") != string::npos) {
                try {
                    indexes.push_back(stoi(fname.substr(0, fname.find("."))));
                } catch (...) {}
            }
        }
        if (indexes.empty()) indexes.push_back(1);
        sort(indexes.begin(), indexes.end());
        return indexes;
    }

public:
    Collection(string newName, string newPath, int limit, json initialStructure, string configPath) 
                                                                                : name(newName),
                                                                                path(newPath),
                                                                                tuples_limit(limit),
                                                                                structure(initialStructure),
                                                                                schemaConfigPath(configPath) 
    {
        if (!filesystem::exists(path)) {
            filesystem::create_directories(path);
            ofstream out(path + "/1.json");
            out << "{}";
            out.close();
        }
    }

    void insert(json document) {
        string id;
        if (document.contains("_id")) id = document["_id"];
        else id = generateId(); 
        
        document["_id"] = id; 

        auto indexes = getFileIndexes();
        int lastIdx = indexes.back();
        string filePath = path + "/" + to_string(lastIdx) + ".json";
        
        json fileData;
        if (filesystem::exists(filePath) && filesystem::file_size(filePath) > 0) {
            ifstream in(filePath);
            try { in >> fileData; } catch(...) { fileData = json::object(); }
            in.close();
        } else {
            fileData = json::object();
        }

        if (fileData.size() >= tuples_limit) {
            filePath = path + "/" + to_string(++lastIdx) + ".json";
            fileData = json::object();
        }

        fileData[id] = document; 

        ofstream out(filePath);
        out << fileData.dump(4);
        out.close();
    }

    json find(const json& query, const json& projection = nullptr) {
        json result = json::array();
        auto indexes = getFileIndexes();

        for (int idx : indexes) {
            ifstream in(path + "/" + to_string(idx) + ".json");
            json chunk;
            if (in.good()) {
                try { in >> chunk; } catch(...) {
                    cerr << "Couldn't read file " + path + "/" + to_string(idx) + ".json" << endl;
                    continue;
                }
            }
            in.close();

            for (auto& [key, doc] : chunk.items()) {
                // matchDocument поддерживает глубокую вложенность
                if (matchDocument(doc, query)) {
                    if (projection != nullptr && !projection.empty()) {
                        json projectedDoc;
                        for(const auto& field : projection) {
                           if(doc.contains(field)) projectedDoc[field] = doc[field];
                        }
                        result.push_back(projectedDoc);
                    } else {
                        result.push_back(doc);
                    }
                }
            }
        }
        return result;
    }

    void update(const json& query, const json& updateOps, bool multi = false) {
        auto indexes = getFileIndexes();
        bool updatedOne = false;

        for (int idx : indexes) {
            if (!multi && updatedOne) break; 

            string fpath = path + "/" + to_string(idx) + ".json";
            ifstream in(fpath);
            json chunk; 
            if (in.good()) {
                 try { in >> chunk; } catch(...) { continue; }
            }
            in.close();

            bool fileChanged = false;
            for (auto& [key, doc] : chunk.items()) {
                if (matchDocument(doc, query)) {
                    if (updateOps.contains("$set")) {
                        for (auto& [k, v] : updateOps["$set"].items()) doc[k] = v;
                    }
                    if (updateOps.contains("$inc")) {
                        for (auto& [k, v] : updateOps["$inc"].items()) {
                            if (doc.contains(k)) doc[k] = doc[k].get<int>() + v.get<int>();
                        }
                    }
                    if (updateOps.contains("$push")) { 
                         for (auto& [k, v] : updateOps["$push"].items()) {
                             if (!doc.contains(k)) doc[k] = json::array();
                             doc[k].push_back(v);
                         }
                    }
                    
                    fileChanged = true;
                    updatedOne = true;
                    if (!multi) break; 
                }
            }

            if (fileChanged) {
                ofstream out(fpath);
                out << chunk.dump(4);
                out.close();
            }
        }
    }

    void remove(const json& query, bool multi = false) {
        auto indexes = getFileIndexes();
        bool deletedOne = false;

        for (int idx : indexes) {
            if (!multi && deletedOne) break;

            string fpath = path + "/" + to_string(idx) + ".json";
            ifstream in(fpath);
            json chunk;
            if (in.good()) {
                try { in >> chunk; } catch(...) { continue; }
            }
            in.close();

            vector<string> keysToDelete;
            for (auto& [key, doc] : chunk.items()) {
                if (matchDocument(doc, query)) {
                    keysToDelete.push_back(key);
                    deletedOne = true;
                    if (!multi) break; 
                }
            }

            if (!keysToDelete.empty()) {
                for(const auto& k : keysToDelete) chunk.erase(k);
                ofstream out(fpath);
                out << chunk.dump(4); 
                out.close();
            }
        }
    }
};

class DBMS {
    string schemaName;
    string configPath; // Храним путь к файлу
    int tuplesLimit;
    map<string, Collection*> collections;

public:
    DBMS(const string& cfgPath) : configPath(cfgPath) {
        // Чтение конфигурации
        ifstream f(configPath);
        if (!f.is_open()) {
            cerr << "Config file not found. Creating default schema.json..." << endl;
            // Дефолтная схема с вложенной структурой для примера
            json defaultSchema = {
                {"name", "MyDatabase"},
                {"tuples_limit", 5},
                {"structure", {
                    {"users", {
                        {"name", "str"},
                        {"age", "int"}
                    }},
                    {"products", {
                        {"title", "str"},
                        {"details", { {"weight", "int"}, {"origin", "str"} }} // Вложенная структура
                    }}
                }}
            };
            ofstream out(configPath);
            out << defaultSchema.dump(4);
            out.close();
            f.open(configPath);
        }

        json config;
        try {
            f >> config;
        } catch(...) {
            cerr << "Error parsing schema.json" << endl;
            return;
        }
        
        schemaName = config["name"];
        tuplesLimit = config["tuples_limit"];

        if (!filesystem::exists(schemaName)) {
            filesystem::create_directory(schemaName);
        }

        // Инициализация коллекций с передачей их структуры из JSON
        for (auto& [colName, schemaStruct] : config["structure"].items()) {
            string colPath = schemaName + "/" + colName;
            // Передаем структуру и путь к конфигу
            collections[colName] = new Collection(colName, colPath, tuplesLimit, schemaStruct, configPath);
        }
    }

    Collection* getCollection(const string& name) {
        if (collections.find(name) != collections.end()) {
            return collections[name];
        }
        cerr << "Collection not found!" << endl;
        return nullptr;
    }

    ~DBMS() {
        for (auto& kv : collections) delete kv.second;
    }
};

int main() {
    DBMS db("schema.json");
    
    Collection* products = db.getCollection("products");
    if (!products) return 1;

    cout << "Inserting products with nested structures..." << endl;
    
    // Вставка документа, соответствующего схеме
    products->insert({
        {"title", "Apple"},
        {"details", { {"weight", 100}, {"origin", "Poland"} }}
    });

    // Вставка документа
    products->insert({
        {"title", "Banana"},
        {"details", { {"weight", 150}, {"origin", "Ecuador"} }}
    });

    products->insert({
        {"title", "Mango"},
        {"details", { {"weight", 300}, {"origin", "Afrika"} }}
    });

    products->insert({
        {"title", "Arbuz"},
        {"details", { {"weight", 1000}, {"origin", "Russia"} }}
    });

    // Поиск по вложенному полю
    cout << "\nFind products from Ecuador:" << endl;
    json query = {
        {"details", {
            {"origin", "Ecuador"}
        }}
    };
    
    cout << products->find(query).dump(4) << endl;

    return 0;
}