//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-04-06 Zou Xu <zouivex@gmail.com>
//
#include <cstdlib>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <yaml.h>

namespace yaml {

    class YamlData {
    public:
        YamlData() = default;

        ~YamlData();

        bool LoadFromStream(std::istream &stream);

        bool SaveToStream(std::ostream &stream);

        bool LoadFromFile(const std::string &file_name);

        bool SaveToFile(const std::string &file_name);

        an<YamlItem> Traverse(const std::string &key);

        bool modified() const { return modified_; }

        void set_modified() { modified_ = true; }

        an<YamlItem> root;

    protected:
        static an<YamlItem> ConvertFromYaml(const YAML::Node &yaml_node);

        static void EmitYaml(an<YamlItem> node,
                             YAML::Emitter *emitter,
                             int depth);

        static void EmitScalar(const std::string &str_value,
                               YAML::Emitter *emitter);

        std::string file_name_;
        bool modified_ = false;
    };

// YamlValue members

    YamlValue::YamlValue(bool value)
            : YamlItem(kScalar) {
        SetBool(value);
    }

    YamlValue::YamlValue(int value)
            : YamlItem(kScalar) {
        SetInt(value);
    }

    YamlValue::YamlValue(double value)
            : YamlItem(kScalar) {
        SetDouble(value);
    }

    YamlValue::YamlValue(const char *value)
            : YamlItem(kScalar), value_(value) {
    }

    YamlValue::YamlValue(const std::string &value)
            : YamlItem(kScalar), value_(value) {
    }

    bool YamlValue::GetBool(bool *value) const {
        if (!value || value_.empty())
            return false;
        std::string bstr = value_;
        boost::to_lower(bstr);
        if ("true" == bstr) {
            *value = true;
            return true;
        } else if ("false" == bstr) {
            *value = false;
            return true;
        } else
            return false;
    }

    bool YamlValue::GetInt(int *value) const {
        if (!value || value_.empty())
            return false;
        // try to parse hex number
        if (boost::starts_with(value_, "0x")) {
            char *p = NULL;
            unsigned int hex = std::strtoul(value_.c_str(), &p, 16);
            if (*p == '\0') {
                *value = static_cast<int>(hex);
                return true;
            }
        }
        // decimal
        try {
            *value = boost::lexical_cast<int>(value_);
        }
        catch (...) {
            return false;
        }
        return true;
    }

    bool YamlValue::GetDouble(double *value) const {
        if (!value || value_.empty())
            return false;
        try {
            *value = boost::lexical_cast<double>(value_);
        }
        catch (...) {
            return false;
        }
        return true;
    }

    bool YamlValue::GetString(std::string *value) const {
        if (!value) return false;
        *value = value_;
        return true;
    }

    bool YamlValue::SetBool(bool value) {
        value_ = value ? "true" : "false";
        return true;
    }

    bool YamlValue::SetInt(int value) {
        value_ = boost::lexical_cast<std::string>(value);
        return true;
    }

    bool YamlValue::SetDouble(double value) {
        value_ = boost::lexical_cast<std::string>(value);
        return true;
    }

    bool YamlValue::SetString(const char *value) {
        value_ = value;
        return true;
    }

    bool YamlValue::SetString(const std::string &value) {
        value_ = value;
        return true;
    }

// YamlList members

    an<YamlItem> YamlList::GetAt(size_t i) const {
        if (i >= seq_.size())
            return nullptr;
        else
            return seq_[i];
    }

    an<YamlValue> YamlList::GetValueAt(size_t i) const {
        return As<YamlValue>(GetAt(i));
    }

    bool YamlList::SetAt(size_t i, an<YamlItem> element) {
        if (i >= seq_.size())
            seq_.resize(i + 1);
        seq_[i] = element;
        return true;
    }

    bool YamlList::Insert(size_t i, an<YamlItem> element) {
        if (i > seq_.size()) {
            seq_.resize(i);
        }
        seq_.insert(seq_.begin() + i, element);
        return true;
    }

    bool YamlList::Append(an<YamlItem> element) {
        seq_.push_back(element);
        return true;
    }

    bool YamlList::Resize(size_t size) {
        seq_.resize(size);
        return true;
    }

    bool YamlList::Clear() {
        seq_.clear();
        return true;
    }

    size_t YamlList::size() const {
        return seq_.size();
    }

    YamlList::Iterator YamlList::begin() {
        return seq_.begin();
    }

    YamlList::Iterator YamlList::end() {
        return seq_.end();
    }

// YamlMap members

    bool YamlMap::HasKey(const std::string &key) const {
        return bool(Get(key));
    }

    an<YamlItem> YamlMap::Get(const std::string &key) const {
        auto it = map_.find(key);
        if (it == map_.end())
            return nullptr;
        else
            return it->second;
    }

    an<YamlValue> YamlMap::GetValue(const std::string &key) const {
        return As<YamlValue>(Get(key));
    }

    bool YamlMap::Set(const std::string &key, an<YamlItem> element) {
        map_[key] = element;
        return true;
    }

    bool YamlMap::Clear() {
        map_.clear();
        return true;
    }

    YamlMap::Iterator YamlMap::begin() {
        return map_.begin();
    }

    YamlMap::Iterator YamlMap::end() {
        return map_.end();
    }

// YamlItemRef members

    bool YamlItemRef::IsNull() const {
        auto item = GetItem();
        return !item || item->type() == YamlItem::kNull;
    }

    bool YamlItemRef::IsValue() const {
        auto item = GetItem();
        return item && item->type() == YamlItem::kScalar;
    }

    bool YamlItemRef::IsList() const {
        auto item = GetItem();
        return item && item->type() == YamlItem::kList;
    }

    bool YamlItemRef::IsMap() const {
        auto item = GetItem();
        return item && item->type() == YamlItem::kMap;
    }

    bool YamlItemRef::ToBool() const {
        bool value = false;
        if (auto item = As<YamlValue>(GetItem())) {
            item->GetBool(&value);
        }
        return value;
    }

    int YamlItemRef::ToInt() const {
        int value = 0;
        if (auto item = As<YamlValue>(GetItem())) {
            item->GetInt(&value);
        }
        return value;
    }

    double YamlItemRef::ToDouble() const {
        double value = 0.0;
        if (auto item = As<YamlValue>(GetItem())) {
            item->GetDouble(&value);
        }
        return value;
    }

    std::string YamlItemRef::ToString() const {
        std::string value;
        if (auto item = As<YamlValue>(GetItem())) {
            item->GetString(&value);
        }
        return value;
    }

    an<YamlList> YamlItemRef::AsList() {
        auto list = As<YamlList>(GetItem());
        if (!list)
            SetItem(list = New<YamlList>());
        return list;
    }

    an<YamlMap> YamlItemRef::AsMap() {
        auto map = As<YamlMap>(GetItem());
        if (!map)
            SetItem(map = New<YamlMap>());
        return map;
    }

    void YamlItemRef::Clear() {
        SetItem(nullptr);
    }

    bool YamlItemRef::Append(an<YamlItem> item) {
        if (AsList()->Append(item)) {
            set_modified();
            return true;
        }
        return false;
    }

    size_t YamlItemRef::size() const {
        auto list = As<YamlList>(GetItem());
        return list ? list->size() : 0;
    }

    bool YamlItemRef::HasKey(const std::string &key) const {
        auto map = As<YamlMap>(GetItem());
        return map ? map->HasKey(key) : false;
    }

    bool YamlItemRef::modified() const {
        return data_ && data_->modified();
    }

    void YamlItemRef::set_modified() {
        if (data_)
            data_->set_modified();
    }

// Yaml members

    Yaml::Yaml() : YamlItemRef(New<YamlData>()) {
    }

    Yaml::~Yaml() {
    }

    bool Yaml::LoadFromStream(std::istream &stream) {
        return data_->LoadFromStream(stream);
    }

    bool Yaml::SaveToStream(std::ostream &stream) {
        return data_->SaveToStream(stream);
    }

    bool Yaml::LoadFromFile(const std::string &file_name) {
        return data_->LoadFromFile(file_name);
    }

    bool Yaml::SaveToFile(const std::string &file_name) {
        return data_->SaveToFile(file_name);
    }

    bool Yaml::IsNull(const std::string &key) {
        auto p = data_->Traverse(key);
        return !p || p->type() == YamlItem::kNull;
    }

    bool Yaml::IsValue(const std::string &key) {
        auto p = data_->Traverse(key);
        return !p || p->type() == YamlItem::kScalar;
    }

    bool Yaml::IsList(const std::string &key) {
        auto p = data_->Traverse(key);
        return !p || p->type() == YamlItem::kList;
    }

    bool Yaml::IsMap(const std::string &key) {
        auto p = data_->Traverse(key);
        return !p || p->type() == YamlItem::kMap;
    }

    bool Yaml::GetBool(const std::string &key, bool *value) {
        ALOGI("read: %s", key.c_str());
        auto p = As<YamlValue>(data_->Traverse(key));
        return p && p->GetBool(value);
    }

    bool Yaml::GetInt(const std::string &key, int *value) {
        ALOGI("read: %s", key.c_str());
        auto p = As<YamlValue>(data_->Traverse(key));
        return p && p->GetInt(value);
    }

    bool Yaml::GetDouble(const std::string &key, double *value) {
        ALOGI("read: %s", key.c_str());
        auto p = As<YamlValue>(data_->Traverse(key));
        return p && p->GetDouble(value);
    }

    bool Yaml::GetString(const std::string &key, std::string *value) {
        ALOGI("read: %s", key.c_str());
        auto p = As<YamlValue>(data_->Traverse(key));
        return p && p->GetString(value);
    }

    an<YamlItem> Yaml::GetItem(const std::string &key) {
        ALOGI("read: %s", key.c_str());
        return data_->Traverse(key);
    }

    an<YamlValue> Yaml::GetValue(const std::string &key) {
        ALOGI("read: %s", key.c_str());
        return As<YamlValue>(data_->Traverse(key));
    }

    an<YamlList> Yaml::GetList(const std::string &key) {
        ALOGI("read: %s", key.c_str());
        return As<YamlList>(data_->Traverse(key));
    }

    an<YamlMap> Yaml::GetMap(const std::string &key) {
        ALOGI("read: %s", key.c_str());
        return As<YamlMap>(data_->Traverse(key));
    }

    bool Yaml::SetBool(const std::string &key, bool value) {
        return SetItem(key, New<YamlValue>(value));
    }

    bool Yaml::SetInt(const std::string &key, int value) {
        return SetItem(key, New<YamlValue>(value));
    }

    bool Yaml::SetDouble(const std::string &key, double value) {
        return SetItem(key, New<YamlValue>(value));
    }

    bool Yaml::SetString(const std::string &key, const char *value) {
        return SetItem(key, New<YamlValue>(value));
    }

    bool Yaml::SetString(const std::string &key, const std::string &value) {
        return SetItem(key, New<YamlValue>(value));
    }

    static inline bool IsListItemReference(const std::string &key) {
        return !key.empty() && key[0] == '@';
    }

    static size_t ResolveListIndex(an<YamlItem> p, const std::string &key,
                                   bool read_only = false) {
        //if (!IsListItemReference(key)) {
        //  return 0;
        //}
        an<YamlList> list = As<YamlList>(p);
        if (!list) {
            return 0;
        }
        const std::string kAfter("after");
        const std::string kBefore("before");
        const std::string kLast("last");
        const std::string kNext("next");
        size_t cursor = 1;
        unsigned int index = 0;
        bool will_insert = false;
        if (key.compare(cursor, kNext.length(), kNext) == 0) {
            cursor += kNext.length();
            index = list->size();
        } else if (key.compare(cursor, kBefore.length(), kBefore) == 0) {
            cursor += kBefore.length();
            will_insert = true;
        } else if (key.compare(cursor, kAfter.length(), kAfter) == 0) {
            cursor += kAfter.length();
            index += 1;  // after i == before i+1
            will_insert = true;
        }
        if (cursor < key.length() && key[cursor] == ' ') {
            ++cursor;
        }
        if (key.compare(cursor, kLast.length(), kLast) == 0) {
            cursor += kLast.length();
            index += list->size();
            if (index != 0) {  // when list is empty, (before|after) last == 0
                --index;
            }
        } else {
            index += std::strtoul(key.c_str() + cursor, NULL, 10);
        }
        if (will_insert && !read_only) {
            list->Insert(index, nullptr);
        }
        return index;
    }

    bool Yaml::SetItem(const std::string &key, an<YamlItem> item) {
        ALOGI("write: %s", key.c_str());
        if (key.empty() || key == "/") {
            data_->root = item;
            data_->set_modified();
            return true;
        }
        if (!data_->root) {
            data_->root = New<YamlMap>();
        }
        an<YamlItem> p(data_->root);
        std::vector<std::string> keys;
        boost::split(keys, key, boost::is_any_of("/"));
        size_t k = keys.size() - 1;
        for (size_t i = 0; i <= k; ++i) {
            YamlItem::ValueType node_type = YamlItem::kMap;
            size_t list_index = 0;
            if (IsListItemReference(keys[i])) {
                node_type = YamlItem::kList;
                list_index = ResolveListIndex(p, keys[i]);
                ALOGI("list index : %s == %d", keys[i].c_str(), list_index);
            }
            if (!p || p->type() != node_type) {
                return false;
            }
            if (i == k) {
                if (node_type == YamlItem::kList) {
                    As<YamlList>(p)->SetAt(list_index, item);
                } else {
                    As<YamlMap>(p)->Set(keys[i], item);
                }
                data_->set_modified();
                return true;
            } else {
                an<YamlItem> next;
                if (node_type == YamlItem::kList) {
                    next = As<YamlList>(p)->GetAt(list_index);
                } else {
                    next = As<YamlMap>(p)->Get(keys[i]);
                }
                if (!next) {
                    if (IsListItemReference(keys[i + 1])) {
                        ALOGI("creating list node for key: %s", keys[i + 1].c_str());
                        next = New<YamlList>();
                    } else {
                        ALOGI("creating map node for key: %s", keys[i + 1].c_str());
                        next = New<YamlMap>();
                    }
                    if (node_type == YamlItem::kList) {
                        As<YamlList>(p)->SetAt(list_index, next);
                    } else {
                        As<YamlMap>(p)->Set(keys[i], next);
                    }
                }
                p = next;
            }
        }
        return false;
    }

    an<YamlItem> Yaml::GetItem() const {
        return data_->root;
    }

    void Yaml::SetItem(an<YamlItem> item) {
        data_->root = item;
        set_modified();
    }

// YamlData members

    YamlData::~YamlData() {
        if (modified_ && !file_name_.empty())
            SaveToFile(file_name_);
    }

    bool YamlData::LoadFromStream(std::istream &stream) {
        if (!stream.good()) {
            ALOGE("failed to load config from stream.");
            return false;
        }
        try {
            YAML::Node doc = YAML::Load(stream);
            root = ConvertFromYaml(doc);
        }
        catch (YAML::Exception &e) {
            ALOGE("Error parsing YAML: %s", e.what());
            return false;
        }
        return true;
    }

    bool YamlData::SaveToStream(std::ostream &stream) {
        if (!stream.good()) {
            ALOGE("failed to save config to stream.");
            return false;
        }
        try {
            YAML::Emitter emitter(stream);
            EmitYaml(root, &emitter, 0);
        }
        catch (YAML::Exception &e) {
            ALOGE("Error emitting YAML: %s", e.what());
            return false;
        }
        return true;
    }

    bool YamlData::LoadFromFile(const std::string &file_name) {
        // update status
        file_name_ = file_name;
        modified_ = false;
        root.reset();
        if (!boost::filesystem::exists(file_name)) {
            ALOGW("nonexistent config file '%s'.", file_name.c_str());
            return false;
        }
        ALOGI("loading config file '%s'.", file_name.c_str());
        try {
            YAML::Node doc = YAML::LoadFile(file_name);
            root = ConvertFromYaml(doc);
        }
        catch (YAML::Exception &e) {
            ALOGE("Error parsing YAML: %s", e.what());
            return false;
        }
        return true;
    }

    bool YamlData::SaveToFile(const std::string &file_name) {
        // update status
        file_name_ = file_name;
        modified_ = false;
        if (file_name.empty()) {
            // not really saving
            return false;
        }

        ALOGI("saving config file '%s'", file_name.c_str());
        // dump tree
        std::ofstream out(file_name.c_str());
        return SaveToStream(out);
    }

    an<YamlItem> YamlData::Traverse(const std::string &key) {
        ALOGI("traverse: %s", key.c_str());
        if (key.empty() || key == "/") {
            return root;
        }
        std::vector<std::string> keys;
        boost::split(keys, key, boost::is_any_of("/"));
        // find the YAML::Node, and wrap it!
        an<YamlItem> p = root;
        for (auto it = keys.begin(), end = keys.end(); it != end; ++it) {
            YamlItem::ValueType node_type = YamlItem::kMap;
            size_t list_index = 0;
            if (IsListItemReference(*it)) {
                node_type = YamlItem::kList;
                list_index = ResolveListIndex(p, *it, true);
            }
            if (!p || p->type() != node_type) {
                return nullptr;
            }
            if (node_type == YamlItem::kList) {
                p = As<YamlList>(p)->GetAt(list_index);
            } else {
                p = As<YamlMap>(p)->Get(*it);
            }
        }
        return p;
    }

    an<YamlItem> YamlData::ConvertFromYaml(const YAML::Node &node) {
        if (YAML::NodeType::Null == node.Type()) {
            return nullptr;
        }
        if (YAML::NodeType::Scalar == node.Type()) {
            return New<YamlValue>(node.as<std::string>());
        }
        if (YAML::NodeType::Sequence == node.Type()) {
            auto config_list = New<YamlList>();
            for (auto it = node.begin(), end = node.end(); it != end; ++it) {
                config_list->Append(ConvertFromYaml(*it));
            }
            return config_list;
        } else if (YAML::NodeType::Map == node.Type()) {
            auto config_map = New<YamlMap>();
            for (auto it = node.begin(), end = node.end(); it != end; ++it) {
                std::string key = it->first.as<std::string>();
                config_map->Set(key, ConvertFromYaml(it->second));
            }
            return config_map;
        }
        return nullptr;
    }

    void YamlData::EmitScalar(const std::string &str_value,
                                YAML::Emitter *emitter) {
        if (str_value.find_first_of("\r\n") != std::string::npos) {
            *emitter << YAML::Literal;
        } else if (!boost::algorithm::all(str_value,
                                          boost::algorithm::is_alnum() ||
                                          boost::algorithm::is_any_of("_."))) {
            *emitter << YAML::DoubleQuoted;
        }
        *emitter << str_value;
    }

    void YamlData::EmitYaml(an<YamlItem> node,
                              YAML::Emitter *emitter,
                              int depth) {
        if (!node || !emitter) return;
        if (node->type() == YamlItem::kScalar) {
            auto value = As<YamlValue>(node);
            EmitScalar(value->str(), emitter);
        } else if (node->type() == YamlItem::kList) {
            if (depth >= 3) {
                *emitter << YAML::Flow;
            }
            *emitter << YAML::BeginSeq;
            auto list = As<YamlList>(node);
            for (auto it = list->begin(), end = list->end(); it != end; ++it) {
                EmitYaml(*it, emitter, depth + 1);
            }
            *emitter << YAML::EndSeq;
        } else if (node->type() == YamlItem::kMap) {
            if (depth >= 3) {
                *emitter << YAML::Flow;
            }
            *emitter << YAML::BeginMap;
            auto map = As<YamlMap>(node);
            for (auto it = map->begin(), end = map->end(); it != end; ++it) {
                if (!it->second || it->second->type() == YamlItem::kNull)
                    continue;
                *emitter << YAML::Key;
                EmitScalar(it->first, emitter);
                *emitter << YAML::Value;
                EmitYaml(it->second, emitter, depth + 1);
            }
            *emitter << YAML::EndMap;
        }
    }
}  // namespace yaml
