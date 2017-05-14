//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-4-6 Zou xu <zouivex@gmail.com>
//
#ifndef YAML_H_
#define YAML_H_

#include <type_traits>
#include <common.h>

namespace yaml {
    // config item base class
    class YamlItem {
    public:
        enum ValueType {
            kNull, kScalar, kList, kMap
        };

        YamlItem() = default;  // null
        virtual ~YamlItem() = default;

        ValueType type() const { return type_; }

    protected:
        YamlItem(ValueType type) : type_(type) {}

        ValueType type_ = kNull;
    };

    class YamlValue : public YamlItem {
    public:
        YamlValue() : YamlItem(kScalar) {}

        YamlValue(bool value);

        YamlValue(int value);

        YamlValue(double value);

        YamlValue(const char *value);

        YamlValue(const std::string &value);

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

    class YamlList : public YamlItem {
    public:
        using Sequence = std::vector<an<YamlItem>>;
        using Iterator = Sequence::iterator;

        YamlList() : YamlItem(kList) {}

        an<YamlItem> GetAt(size_t i) const;

        an<YamlValue> GetValueAt(size_t i) const;

        bool SetAt(size_t i, an<YamlItem> element);

        bool Insert(size_t i, an<YamlItem> element);

        bool Append(an<YamlItem> element);

        bool Resize(size_t size);

        bool Clear();

        size_t size() const;

        Iterator begin();

        Iterator end();

    protected:
        Sequence seq_;
    };

// limitation: map keys have to be strings, preferably alphanumeric
    class YamlMap : public YamlItem {
    public:
        using Map = std::map<std::string, an<YamlItem>>;
        using Iterator = Map::iterator;

        YamlMap() : YamlItem(kMap) {}

        bool HasKey(const std::string &key) const;

        an<YamlItem> Get(const std::string &key) const;

        an<YamlValue> GetValue(const std::string &key) const;

        bool Set(const std::string &key, an<YamlItem> element);

        bool Clear();

        Iterator begin();

        Iterator end();

    protected:
        Map map_;
    };

    class YamlData;

    class YamlListEntryRef;

    class YamlMapEntryRef;

    class YamlItemRef {
    public:
        YamlItemRef(const an<YamlData> &data) : data_(data) {
        }

        operator an<YamlItem>() const {
            return GetItem();
        }

        YamlListEntryRef operator[](size_t index);

        YamlMapEntryRef operator[](const std::string &key);

        bool IsNull() const;

        bool IsValue() const;

        bool IsList() const;

        bool IsMap() const;

        bool ToBool() const;

        int ToInt() const;

        double ToDouble() const;

        std::string ToString() const;

        an<YamlList> AsList();

        an<YamlMap> AsMap();

        void Clear();

        // list
        bool Append(an<YamlItem> item);

        size_t size() const;

        // map
        bool HasKey(const std::string &key) const;

        bool modified() const;

        void set_modified();

    protected:
        virtual an<YamlItem> GetItem() const = 0;

        virtual void SetItem(an<YamlItem> item) = 0;

        an<YamlData> data_;
    };

    namespace {

        template<class T>
        an<YamlItem> AsYamlItem(const T &x, const std::false_type &) {
            return New<YamlValue>(x);
        };

        template<class T>
        an<YamlItem> AsYamlItem(const T &x, const std::true_type &) {
            return x;
        };

    }  // namespace

    class YamlListEntryRef : public YamlItemRef {
    public:
        YamlListEntryRef(an<YamlData> data,
                           an<YamlList> list, size_t index)
                : YamlItemRef(data), list_(list), index_(index) {
        }

        template<class T>
        YamlListEntryRef &operator=(const T &x) {
            SetItem(AsYamlItem(x, std::is_convertible<T, an<YamlItem>>()));
            return *this;
        }

    protected:
        an<YamlItem> GetItem() const {
            return list_->GetAt(index_);
        }

        void SetItem(an<YamlItem> item) {
            list_->SetAt(index_, item);
            set_modified();
        }

    private:
        an<YamlList> list_;
        size_t index_;
    };

    class YamlMapEntryRef : public YamlItemRef {
    public:
        YamlMapEntryRef(an<YamlData> data,
                          an<YamlMap> map, const std::string &key)
                : YamlItemRef(data), map_(map), key_(key) {
        }

        template<class T>
        YamlMapEntryRef &operator=(const T &x) {
            SetItem(AsYamlItem(x, std::is_convertible<T, an<YamlItem>>()));
            return *this;
        }

    protected:
        an<YamlItem> GetItem() const {
            return map_->Get(key_);
        }

        void SetItem(an<YamlItem> item) {
            map_->Set(key_, item);
            set_modified();
        }

    private:
        an<YamlMap> map_;
        std::string key_;
    };

    inline YamlListEntryRef YamlItemRef::operator[](size_t index) {
        return YamlListEntryRef(data_, AsList(), index);
    }

    inline YamlMapEntryRef YamlItemRef::operator[](const std::string &key) {
        return YamlMapEntryRef(data_, AsMap(), key);
    }

// Yaml class

    class Yaml : public YamlItemRef {
    public:
        // CAVEAT: Yaml instances created without argument will NOT
        // be managed by ConfigComponent
        Yaml();

        virtual ~Yaml();

        // instances of Yaml with identical file_name share a copy of config data
        // that could be reloaded by ConfigComponent once notified changes to the file
        explicit Yaml(const std::string &file_name);

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

        an<YamlItem> GetItem(const std::string &key);

        an<YamlValue> GetValue(const std::string &key);

        an<YamlList> GetList(const std::string &key);

        an<YamlMap> GetMap(const std::string &key);

        // setters
        bool SetBool(const std::string &key, bool value);

        bool SetInt(const std::string &key, int value);

        bool SetDouble(const std::string &key, double value);

        bool SetString(const std::string &key, const char *value);

        bool SetString(const std::string &key, const std::string &value);

        // setter for adding / replacing items to the tree
        bool SetItem(const std::string &key, an<YamlItem> item);

        template<class T>
        Yaml &operator=(const T &x) {
            SetItem(AsYamlItem(x, std::is_convertible<T, an<YamlItem>>()));
            return *this;
        }

    protected:
        an<YamlItem> GetItem() const;

        void SetItem(an<YamlItem> item);
    };

}  // namespace yaml

#endif  // YAML_H_
