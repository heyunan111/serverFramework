#include"../include/iniFile.h"
#include <sstream>
#include <utility>
#include <iostream>
#include <fstream>


hyn::ini::Value::Value(bool value) {
    m_value = value ? "true" : "false";
}

hyn::ini::Value::Value(int value) {
    *this = value;
}

hyn::ini::Value::Value(double value) {
    *this = value;
}

hyn::ini::Value::Value(const char *value) : m_value(value) {
}

hyn::ini::Value::Value(std::string value) : m_value(std::move(value)) {
}

hyn::ini::Value &hyn::ini::Value::operator=(bool value) {
    m_value = value ? "true" : "false";
    return *this;
}

hyn::ini::Value &hyn::ini::Value::operator=(int value) {
    std::stringstream ss;
    ss << value;
    m_value = ss.str();
    return *this;
}

hyn::ini::Value &hyn::ini::Value::operator=(double value) {
    std::stringstream ss;
    ss << value;
    m_value = ss.str();
    return *this;
}

hyn::ini::Value &hyn::ini::Value::operator=(const std::string &value) {
    m_value = value;
    return *this;
}

hyn::ini::Value::operator bool() {
    return m_value == "true";
}

hyn::ini::Value::operator int() {
    return std::atoi(m_value.c_str());
}

hyn::ini::Value::operator double() {
    return std::atof(m_value.c_str());
}

hyn::ini::Value::operator std::string() {
    return m_value;
}

hyn::ini::Value::operator std::string() const {
    return m_value;
}


bool hyn::ini::IniFile::load(const std::string &filename) {
    m_filename = filename;
    m_inifile.clear();

    std::string name;
    std::string line;

    std::ifstream fin(filename);
    if (fin.fail()) {
        printf("loading file failed: %s is not found.\n", m_filename.c_str());
        return false;
    }
    while (std::getline(fin, line)) {
        line = trim(line);
        if ('[' == line[0]) // it is a section
        {
            int pos = line.find_first_of(']');
            if (-1 != pos) {
                name = trim(line.substr(1, pos - 1));
                m_inifile[name];
            }
        } else if ('#' == line[0]) // it is a comment
        {
            continue;
        } else {
            int pos = line.find_first_of('=');
            if (pos > 0) {
                //add new key to the last section in the storage
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1, line.size() - pos - 1));
                auto it = m_inifile.find(name);
                if (it == m_inifile.end()) {
                    printf("parsing error: section=%s key=%s\n", name.c_str(), key.c_str());
                    return false;
                }
                m_inifile[name][key] = value;
            }
        }
    }
    return true;
}

std::string hyn::ini::IniFile::trim(std::string s) {
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" \r\n"));
    s.erase(s.find_last_not_of(" \r\n") + 1);
    return s;
}

hyn::ini::Value &hyn::ini::IniFile::get(const std::string &section, const std::string &key) {
    return m_inifile[section][key];
}

void hyn::ini::IniFile::set(const std::string &section, const std::string &key, bool value) {
    m_inifile[section][key] = value;
}

void hyn::ini::IniFile::set(const std::string &section, const std::string &key, int value) {
    m_inifile[section][key] = value;
}

void hyn::ini::IniFile::set(const std::string &section, const std::string &key, double value) {
    m_inifile[section][key] = value;
}

void hyn::ini::IniFile::set(const std::string &section, const std::string &key, const std::string &value) {
    m_inifile[section][key] = value;
}

bool hyn::ini::IniFile::has(const std::string &section) {
    return m_inifile.find(section) != m_inifile.end();
}

bool hyn::ini::IniFile::has(const std::string &section, const std::string &key) {
    auto it = m_inifile.find(section);
    if (it != m_inifile.end()) {
        return it->second.find(key) != it->second.end();
    }
    return false;
}

void hyn::ini::IniFile::clear() {
    m_inifile.clear();
}

void hyn::ini::IniFile::remove(const std::string &section) {
    m_inifile.erase(section);
}

void hyn::ini::IniFile::remove(const std::string &section, const std::string &key) {
    auto it = m_inifile.find(section);
    if (it != m_inifile.end()) {
        it->second.erase(key);
    }
}

hyn::ini::IniFile::IniFile(const std::string &filename) {
    load(filename);
}

void hyn::ini::IniFile::save(const std::string &filename) {
    std::ofstream fout(filename);
    fout << to_string();
    fout.close();
}

void hyn::ini::IniFile::show() {
    std::cout << to_string();
}

std::string hyn::ini::IniFile::to_string() {
    std::stringstream ss;
    for (auto &it: m_inifile) {
        ss << "[" << it.first << "]" << std::endl;
        for (auto &it1: it.second)
            ss << it1.first << " = " << static_cast<std::string>((it1.second)) << std::endl;
        ss << std::endl;
    }
    return ss.str();
}
