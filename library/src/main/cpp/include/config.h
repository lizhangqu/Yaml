//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-4-6 Zou xu <zouivex@gmail.com>
//
#ifndef CONFIG_H_
#define CONFIG_H_

#include <type_traits>
#include <common.h>

namespace yaml {
    // config item base class
    class ConfigItem {
    public:
        enum ValueType {
            kNull, kScalar, kList, kMap
        };

        ConfigItem() = default;  // null
        virtual ~ConfigItem() = default;

        ValueType type() const { return type_; }

    protected:
        ConfigItem(ValueType type) : type_(type) {}

        ValueType type_ = kNull;
    };

    class ConfigValue : public ConfigItem {
    public:
        ConfigValue() : ConfigItem(kScalar) {}

        ConfigValue(bool value);

        ConfigValue(int value);

        ConfigValue(double value);

        ConfigValue(const char *value);

        ConfigValue(const std::string &value);

        // schalar value accessors
        bool GetBool(bool *value) const;

        bool GetInt(int *value) const;

        bool GetDouble(double *value) const;

        bool GetString(std::string *value) const;

        bool SetBool(bool value);

        bool SetInt(int value);

        bool SetDouble(double value);

        bool SetString(const char *value);

        bool SetString(const std::string &value);

        const std::string &str() const { return value_; }

    protected:
        std::string value_;
    };

    class ConfigList : public ConfigItem {
    public:
        using Sequence = std::vector<an<ConfigItem>>;
        using Iterator = Sequence::iterator;

        ConfigList() : ConfigItem(kList) {}

        an<ConfigItem> GetAt(size_t i) const;

        an<ConfigValue> GetValueAt(size_t i) const;

        bool SetAt(size_t i, an<ConfigItem> element);

        bool Insert(size_t i, an<ConfigItem> element);

        bool Append(an<ConfigItem> element);

        bool Resize(size_t size);

        bool Clear();

        size_t size() const;

        Iterator begin();

        Iterator end();

    protected:
        Sequence seq_;
    };

// limitation: map keys have to be strings, preferably alphanumeric
    class ConfigMap : public ConfigItem {
    public:
        using Map = std::map<std::string, an<ConfigItem>>;
        using Iterator = Map::iterator;

        ConfigMap() : ConfigItem(kMap) {}

        bool HasKey(const std::string &key) const;

        an<ConfigItem> Get(const std::string &key) const;

        an<ConfigValue> GetValue(const std::string &key) const;

        bool Set(const std::string &key, an<ConfigItem> element);

        bool Clear();

        Iterator begin();

        Iterator end();

    protected:
        Map map_;
    };

    class ConfigData;

    class ConfigListEntryRef;

    class ConfigMapEntryRef;

    class ConfigItemRef {
    public:
        ConfigItemRef(const an<ConfigData> &data) : data_(data) {
        }

        operator an<ConfigItem>() const {
            return GetItem();
        }

        ConfigListEntryRef operator[](size_t index);

        ConfigMapEntryRef operator[](const std::string &key);

        bool IsNull() const;

        bool IsValue() const;

        bool IsList() const;

        bool IsMap() const;

        bool ToBool() const;

        int ToInt() const;

        double ToDouble() const;

        std::string ToString() const;

        an<ConfigList> AsList();

        an<ConfigMap> AsMap();

        void Clear();

        // list
        bool Append(an<ConfigItem> item);

        size_t size() const;

        // map
        bool HasKey(const std::string &key) const;

        bool modified() const;

        void set_modified();

    protected:
        virtual an<ConfigItem> GetItem() const = 0;

        virtual void SetItem(an<ConfigItem> item) = 0;

        an<ConfigData> data_;
    };

    namespace {

        template<class T>
        an<ConfigItem> AsConfigItem(const T &x, const std::false_type &) {
            return New<ConfigValue>(x);
        };

        template<class T>
        an<ConfigItem> AsConfigItem(const T &x, const std::true_type &) {
            return x;
        };

    }  // namespace

    class ConfigListEntryRef : public ConfigItemRef {
    public:
        ConfigListEntryRef(an<ConfigData> data,
                           an<ConfigList> list, size_t index)
                : ConfigItemRef(data), list_(list), index_(index) {
        }

        template<class T>
        ConfigListEntryRef &operator=(const T &x) {
            SetItem(AsConfigItem(x, std::is_convertible<T, an<ConfigItem>>()));
            return *this;
        }

    protected:
        an<ConfigItem> GetItem() const {
            return list_->GetAt(index_);
        }

        void SetItem(an<ConfigItem> item) {
            list_->SetAt(index_, item);
            set_modified();
        }

    private:
        an<ConfigList> list_;
        size_t index_;
    };

    class ConfigMapEntryRef : public ConfigItemRef {
    public:
        ConfigMapEntryRef(an<ConfigData> data,
                          an<ConfigMap> map, const std::string &key)
                : ConfigItemRef(data), map_(map), key_(key) {
        }

        template<class T>
        ConfigMapEntryRef &operator=(const T &x) {
            SetItem(AsConfigItem(x, std::is_convertible<T, an<ConfigItem>>()));
            return *this;
        }

    protected:
        an<ConfigItem> GetItem() const {
            return map_->Get(key_);
        }

        void SetItem(an<ConfigItem> item) {
            map_->Set(key_, item);
            set_modified();
        }

    private:
        an<ConfigMap> map_;
        std::string key_;
    };

    inline ConfigListEntryRef ConfigItemRef::operator[](size_t index) {
        return ConfigListEntryRef(data_, AsList(), index);
    }

    inline ConfigMapEntryRef ConfigItemRef::operator[](const std::string &key) {
        return ConfigMapEntryRef(data_, AsMap(), key);
    }

// Config class

    class Config : public ConfigItemRef {
    public:
        // CAVEAT: Config instances created without argument will NOT
        // be managed by ConfigComponent
        Config();

        virtual ~Config();

        // instances of Config with identical file_name share a copy of config data
        // that could be reloaded by ConfigComponent once notified changes to the file
        explicit Config(const std::string &file_name);

        bool LoadFromStream(std::istream &stream);

        bool SaveToStream(std::ostream &stream);

        bool LoadFromFile(const std::string &file_name);

        bool SaveToFile(const std::string &file_name);

        // access a tree node of a particular type with "path/to/key"
        bool IsNull(const std::string &key);

        bool IsValue(const std::string &key);

        bool IsList(const std::string &key);

        bool IsMap(const std::string &key);

        bool GetBool(const std::string &key, bool *value);

        bool GetInt(const std::string &key, int *value);

        bool GetDouble(const std::string &key, double *value);

        bool GetString(const std::string &key, std::string *value);

        an<ConfigItem> GetItem(const std::string &key);

        an<ConfigValue> GetValue(const std::string &key);

        an<ConfigList> GetList(const std::string &key);

        an<ConfigMap> GetMap(const std::string &key);

        // setters
        bool SetBool(const std::string &key, bool value);

        bool SetInt(const std::string &key, int value);

        bool SetDouble(const std::string &key, double value);

        bool SetString(const std::string &key, const char *value);

        bool SetString(const std::string &key, const std::string &value);

        // setter for adding / replacing items to the tree
        bool SetItem(const std::string &key, an<ConfigItem> item);

        template<class T>
        Config &operator=(const T &x) {
            SetItem(AsConfigItem(x, std::is_convertible<T, an<ConfigItem>>()));
            return *this;
        }

    protected:
        an<ConfigItem> GetItem() const;

        void SetItem(an<ConfigItem> item);
    };

}  // namespace yaml

#endif  // CONFIG_H_
