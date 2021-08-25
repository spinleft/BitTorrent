#ifndef UTIL_BENCODING_HPP
#define UTIL_BENCODING_HPP

#define BUFFER_SIZE 1024

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "defs.hpp"

namespace util {
// util::

class Bencoding_node {
public:
    Bencoding_node() : type(NONE) {}
    Bencoding_node(type_t type) : type(type) {}
    type_t getType() { return type; }
    virtual bool empty() { return true; }
    virtual size_t size() { return 0; }
    virtual std::string &getKey(int n) {}
    virtual Bencoding_node *getValue(int n) { return 0; }
    virtual std::vector<de_t *> &getDict() {}
    virtual Bencoding_node *getEntry(int n) { return 0; }
    virtual std::vector<Bencoding_node *> &getList() {}
    virtual std::string &getString() {}
    virtual long getInt() { return 0; }
    virtual ~Bencoding_node() {};

private:
    type_t type;
};
class Dictionary_entry {
public:
    Dictionary_entry() : key(), value(0) {}
    Dictionary_entry(std::string &key) : key(key), value(0) {}
    Dictionary_entry(std::string &key, Bencoding_node *value) : key(key), value(value) {}
    ~Dictionary_entry() {
        if (value)
            delete value;
    }
    std::string key;
    class Bencoding_node *value;
};
class Bencoding_dictionary : public Bencoding_node {
public:
    Bencoding_dictionary() : Bencoding_node(DICT) {}
    bool empty() { return dict.empty(); }
    size_t size() { return dict.size(); }
    std::string &getKey(int n) { return dict[n]->key; }
    Bencoding_node *getValue(int n) { return dict[n]->value; }
    std::vector<de_t *> &getDict() { return dict; }
    ~Bencoding_dictionary() {
        for (int i = 0; i < dict.size(); ++i)
            if (dict[i]->value)
                delete dict[i]->value;
    }

private:
    std::vector<de_t *> dict;
};
class Bencoding_list : public Bencoding_node {
public:
    Bencoding_list() : Bencoding_node(LIST) {}
    bool empty() { return list.empty(); }
    size_t size() { return list.size(); }
    Bencoding_node *getEntry(int n) { return list[n]; }
    std::vector<Bencoding_node *> &getList() { return list; }
    ~Bencoding_list() {
        for (int i = 0; i < list.size(); ++i)
            if (list[i])
                delete list[i];
    }

private:
    std::vector<Bencoding_node *> list;
};
class Bencoding_string : public Bencoding_node {
public:
    Bencoding_string() : Bencoding_node(STR) {}
    Bencoding_string(char *str) : Bencoding_node(STR), str(str) {}
    Bencoding_string(std::string str) : Bencoding_node(STR), str(str) {}
    bool empty() { return str.empty(); }
    size_t size() { return str.length(); }
    std::string &getString() { return str; }
    ~Bencoding_string() = default;

private:
    std::string str;
};
class Bencoding_int : public Bencoding_node {
public:
    Bencoding_int() : Bencoding_node(INT) {}
    Bencoding_int(long integer) : Bencoding_node(INT), integer(integer) {}
    long getInt() { return integer; }
    ~Bencoding_int() = default;

private:
    long integer;
};

type_t readType(char flag)
{
    if (flag >= '0' && flag <= '9')
        return STR;
    else if (flag == 'd')
        return DICT;
    else if (flag == 'l')
        return LIST;
    else if (flag == 'i')
        return INT;
    else if (flag == 'e')
        return END;
    return NONE;
}

Bdict *decode(const char *filename)
{
    std::ifstream file;
    char flag;
    Bdict *root;

    file.open(filename);
    if (!file) {
        std::cout << "Could not open file: " << filename << std::endl;
        file.close();
        return 0;
    }
    if (!file.get(flag)) {
        std::cout << "File " << filename << " empty!" << std::endl;
        file.close();
        return 0;
    }
    if (readType(flag) == DICT)
        root = readDict(file);
    else{
        std::cout << "decode(): root must be dictionary." << std::endl;
        file.close();
        return 0;
    }
    file.close();
    return root;
}

de_t *readDictEntry(std:: ifstream &file, int &status)
{
    char flag;
    de_t *entry;

    status = 0;
    if (!file.get(flag)) {
        std::cout << "readDictEntry(): read file fail." << std::endl;
        status = -1;
        return 0;
    }
    if (readType(flag) == END)
        return 0;
    entry = new de_t();
    // read key
    if (readType(flag) == STR) {
        if(readStr(file, entry->key) < 0) {
            std::cout << "readDictEntry(): read key fail." << std::endl;
            delete entry;
            status = -1;
            return 0;
        }
    }
    else{
        std::cout << "readDictEntry(): key must be string." << std::endl;
        delete entry;
        status = -1;
        return 0;
    }
    // read value
    if (!file.get(flag)) {
        std::cout << "readDictEntry(): value does not exist." << std::endl;
        delete entry;
        status = -1;
        return 0;
    }
    if (readType(flag) == STR)
        entry->value = readStr(file);
    else if (readType(flag) == INT)
        entry->value = readInt(file);
    else if (readType(flag) == LIST)
        entry->value = readList(file);
    else if (readType(flag) == DICT)
        entry->value = readDict(file);
    else{
        std::cout << "readDictEntry(): value does not exist or unkown." << std::endl;
        delete entry;
        status = -1;
        return 0;
    }
    return entry;
}

Bdict *readDict(std::ifstream &file)
{
    char flag;
    int status;
    de_t *entry;
    Bdict *node;

    node = new Bdict();
    status = 0;
    while (true) {
        entry = readDictEntry(file, status);
        if (entry)
            node->getDict().push_back(entry);
        else if (status < 0) {
            delete node;
            return 0;
        } else
            break;
    }
    return node;
}

Blist *readList(std::ifstream &file)
{
    char flag;
    Blist *node;
    Bnode *entry;

    node = new Blist();
    while (true) {
        if (!file.get(flag)) {
            delete node;
            return 0;
        }
        // read entry
        if (readType(flag) == END)
            return node;
        else if (readType(flag) == STR)
            node->getList().push_back(readStr(file));
        else if (readType(flag) == INT)
            node->getList().push_back(readInt(file));
        else if (readType(flag) == LIST)
            node->getList().push_back(readList(file));
        else if (readType(flag) == DICT)
            node->getList().push_back(readDict(file));
        else{
            std::cout << "readList(): entry don't exists or unkown." << std::endl;
            delete node;
            return 0;
        }
    }
    return 0;
}

Bstr *readStr(std::ifstream &file)
{
    size_t len = 0, l;
    char c;
    char buffer[BUFFER_SIZE];
    Bstr *node;

    file.seekg(-1, std::ios::cur);
    while (true)
    {
        if (!file.get(c))
            return 0;
        if (c >= '0' && c <= '9')
            len = 10 * len + c - '0';
        else if (c == ':') {
            node = new Bstr();
            while (len) {
                l = len < BUFFER_SIZE ? len : BUFFER_SIZE;
                if (!file.read(buffer, l)) {
                    std::cout << "readStr: read file error." << std::endl;
                    return 0;
                }
                node->getString().append(buffer, l);
                len -= l;
            }
            return node;
        } else
            return 0;
    }
    return 0;
}

int readStr(std::ifstream &file, std::string &str)
{
    size_t len = 0, l;
    char c;
    char buffer[BUFFER_SIZE];

    file.seekg(-1, std::ios::cur);
    while (true)
    {
        if (!file.get(c))
            return -1;
        if (c >= '0' && c <= '9')
            len = 10 * len + c - '0';
        else if (c == ':') {
            while (len) {
                l = len < BUFFER_SIZE ? len : BUFFER_SIZE;
                if (!file.read(buffer, l)) {
                    std::cout << "readStr: read file error." << std::endl;
                    return 0;
                }
                str.append(buffer, l);
                len -= l;
            }
            return 0;
        } else
            return -1;
    }
    return -1;
}

Bint *readInt(std::ifstream &file)
{
    long sign = 1, integer = 0;
    char c;
    Bint *node;

    while (true)
    {
        if (!file.get(c))
            return 0;
        if (c == '-')
            sign = -1;
        else if (c >= '0' && c <= '9')
            integer = 10 * integer + c - '0';
        else if (c == 'e') {
            node = new Bint(sign * integer);
            return node;
        } else
            return 0;
    }
    return 0;
}

} // namespace util

#endif // UTIL_BENCODING_HPP