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

// Логика операторов и фильтрации

bool checkCondition(const json& value, const json& condition) {
    if (!condition.is_object()) {
        return value == condition; // Простое равенство: {"status": "fail"}
    }

    for (auto& [op, arg] : condition.items()) {
        // Операторы сравнения 
        if (op == "$eq") { if (value != arg) return false; }
        else if (op == "$ne") { if (value == arg) return false; }
        else if (op == "$gt") { if (value <= arg) return false; }
        else if (op == "$lt") { if (value >= arg) return false; }
        else if (op == "$gte") { if (value < arg) return false; }
        else if (op == "$lte") { if (value > arg) return false; }
        else if (op == "$in") { 
            bool found = false;
            for (const auto& item : arg) {
                if (item == value) { found = true; break; }
            }
            if (!found) return false;
        }
        else if (op == "$not") { // 
            if (checkCondition(value, arg)) return false; 
        }
        // Если это не оператор, а вложенный объект, рекурсивно не идем в рамках этой простой реализации,
        // считаем, что структура плоская или сравниваем объекты целиком.
    }
    return true;
}

bool matchDocument(const json& doc, const json& query) {
    if (query.empty()) return true; // Пустой фильтр возвращает всё

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

    // Проверка полей документа
    for (auto& [key, condition] : query.items()) {
        if (key[0] == '$') continue; // Пропускаем операторы верхнего уровня

        // Если ключа нет в документе, считаем, что он null/undefined
        json val = doc.contains(key) ? doc[key] : json(); 
        
        if (!checkCondition(val, condition)) {
            return false;
        }
    }
    return true;
}

// Класс Коллекции

class Collection {
    string name;
    string path;
    int tuples_limit;

    // Генерация уникального ключа (UUID-like)
    string generateId() {
        mt19937 gen(rd());
        return to_string(chrono::system_clock::now().time_since_epoch().count()) + "_" + to_string(gen());
    }

    // Получение списка файлов данных отсортированных по номеру (1.json, 2.json...)
    vector<int> getFileIndexes() {
        vector<int> indexes;
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
    Collection(string newName, string newPath, int limit) : name(newName),
                                                            path(newPath),
                                                            tuples_limit(limit) {
        if (!filesystem::exists(path)) {
            filesystem::create_directories(path);
            // Создаем первый пустой файл 
            ofstream out(path + "/1.json");
            out << "{}";
            out.close();
        }
    }

    // insert
    void insert(json document) {
        // Добавление по уникальному ключу
        string id;
        if (document.contains("_id")) id = document["_id"];
        else id = generateId(); // Генерируем, если нет
        
        document["_id"] = id; // Убеждаемся, что id внутри тоже есть

        // Логика "поиска конца" и лимитов 
        auto indexes = getFileIndexes();
        int lastIdx = indexes.back();
        string filePath = path + "/" + to_string(lastIdx) + ".json";
        
        json fileData;
        if (filesystem::file_size(filePath) > 0) {
            ifstream in(filePath);
            in >> fileData;
            in.close();
        }

        if (fileData.size() >= tuples_limit) {
            // Создаем новый файл
            filePath = path + "/" + to_string(++lastIdx) + ".json";
            fileData = json::object();
        }

        fileData[id] = document; // Храним как {id: doc} 

        ofstream out(filePath);
        out << fileData.dump(4);
        out.close();
    }

    // find
    json find(const json& query, const json& projection = nullptr) {
        json result = json::array();
        auto indexes = getFileIndexes();

        // Чтение файлов последовательно
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

            // chunk - это объект {"id1": {...}, "id2": {...}}
            for (auto& [key, doc] : chunk.items()) {
                if (matchDocument(doc, query)) {
                    // Проекция
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

    // update
    void update(const json& query, const json& updateOps, bool multi = false) {
        auto indexes = getFileIndexes();
        bool updatedOne = false;

        for (int idx : indexes) {
            if (!multi && updatedOne) break; // Если update_one уже выполнил работу

            string fpath = path + "/" + to_string(idx) + ".json";
            ifstream in(fpath);
            json chunk; 
            if (in.good()) in >> chunk;
            in.close();

            bool fileChanged = false;
            for (auto& [key, doc] : chunk.items()) {
                if (matchDocument(doc, query)) {
                    // Применяем операторы обновления 
                    if (updateOps.contains("$set")) {
                        for (auto& [k, v] : updateOps["$set"].items()) doc[k] = v;
                    }
                    if (updateOps.contains("$inc")) {
                        for (auto& [k, v] : updateOps["$inc"].items()) {
                            if (doc.contains(k)) doc[k] = doc[k].get<int>() + v.get<int>(); // Упрощено для int
                        }
                    }
                    if (updateOps.contains("$push")) { // 
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

    // delete
    void remove(const json& query, bool multi = false) {
        auto indexes = getFileIndexes();
        bool deletedOne = false;

        for (int idx : indexes) {
            if (!multi && deletedOne) break;

            string fpath = path + "/" + to_string(idx) + ".json";
            ifstream in(fpath);
            json chunk;
            if (in.good()) in >> chunk;
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
                out << chunk.dump(4); // Перезаписываем файл без удаленных строк
                out.close();
            }
        }
    }
};

// Класс СУБД

class DBMS {
    string schemaName;
    int tuplesLimit;
    map<string, Collection*> collections;

public:
    DBMS(const string& configPath) {
        // Чтение конфигурации
        ifstream f(configPath);
        if (!f.is_open()) {
            cerr << "Config file not found. Creating default schema.json..." << endl;
            // Создаем пример схемы для первого запуска
            json defaultSchema = {
                {"name", "MyDatabase"},
                {"tuples_limit", 5}, // Маленький лимит для тестов
                {"structure", {
                    {"users", {}},
                    {"products", {}}
                }}
            };
            ofstream out(configPath);
            out << defaultSchema.dump(4);
            out.close();
            f.open(configPath);
        }

        json config;
        f >> config;
        
        schemaName = config["name"];
        tuplesLimit = config["tuples_limit"];

        // Создание директорий схемы
        if (!filesystem::exists(schemaName)) {
            filesystem::create_directory(schemaName);
        }

        // Инициализация коллекций
        for (auto& [colName, schema] : config["structure"].items()) {
            string colPath = schemaName + "/" + colName;
            collections[colName] = new Collection(colName, colPath, tuplesLimit);
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

// Main (Демонстрация)

int main() {
    // 1. Инициализация СУБД
    DBMS db("schema.json");
    
    Collection* users = db.getCollection("users");
    if (!users) return 1;

    // 2. Insert
    cout << "Inserting users..." << endl;
    users->insert({{"name", "Alice"}, {"age", 25}, {"status", "active"}});
    users->insert({{"name", "Bob"}, {"age", 30}, {"status", "inactive"}});
    users->insert({{"name", "Charlie"}, {"age", 35}, {"status", "active"}});
    // Добавим еще, чтобы проверить переполнение файла (лимит 5)
    users->insert({{"name", "Dave"}, {"age", 20}, {"status", "warn"}});
    users->insert({{"name", "Eve"}, {"age", 40}, {"status", "active"}});
    users->insert({{"name", "Frank"}, {"age", 50}, {"status", "error"}}); // Должен попасть в 2.json

    // 3. Find с операторами
    cout << "\nFind users with age > 25:" << endl;
    json query1 = {
        {"age", {{"$gt", 25}}}
    };
    json res1 = users->find(query1);
    cout << res1.dump(4) << endl;

    // 4. Update
    cout << "\nUpdating Alice status to 'super_active'..." << endl;
    users->update(
        {{"name", "Alice"}}, 
        {{"$set", {{"status", "super_active"}}}}, 
        false // update_one
    );

    // 5. Delete
    cout << "\nDeleting Bob..." << endl;
    users->remove({{"name", "Bob"}});

    // Проверка результата
    cout << "\nAll users after modifications:" << endl;
    cout << users->find({}).dump(4) << endl;

    return 0;
}