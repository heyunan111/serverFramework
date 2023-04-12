#pragma once

#include<string>
#include <map>

namespace hyn::ini {


class Value {
public:
    Value() = default;

    explicit Value(bool value);

    explicit Value(int value);

    explicit Value(double value);

    explicit Value(const char *value);

    explicit Value(std::string value);

    ~Value() = default;

    Value &operator=(bool value);

    Value &operator=(int value);

    Value &operator=(double value);

    Value &operator=(const std::string &value);

    operator bool();

    operator int();

    operator double();

    operator std::string();

    operator std::string() const;

private:
    std::string m_value;
};


class IniFile {
public:
    IniFile() = default;

    explicit IniFile(const std::string &filename);

    ~IniFile() = default;

    bool load(const std::string &filename);

    void save(const std::string &filename);

    void show();

    void clear();

    // read values in different formats
    Value &get(const std::string &section, const std::string &key);

    // set values in different formats
    void set(const std::string &section, const std::string &key, bool value);

    void set(const std::string &section, const std::string &key, int value);

    void set(const std::string &section, const std::string &key, double value);

    void set(const std::string &section, const std::string &key, const std::string &value);

    bool has(const std::string &section);

    bool has(const std::string &section, const std::string &key);

    void remove(const std::string &section);

    void remove(const std::string &section, const std::string &key);

    std::string to_string();

    typedef std::map<std::string, Value>
            Section;

    Section &operator[](const std::string &key) {
        return m_inifile[key];
    }

private:
    static std::string trim(std::string s);

private:
    std::string m_filename;

    std::map<std::string, Section> m_inifile;
};


} // namespace hyn::ini



