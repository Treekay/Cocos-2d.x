#pragma once
#include <string>
#include <vector>
#include "json\document.h"

// 将Document转成字符串
std::string serializeRapidjsonDocument(const rapidjson::Document &doc);
// 将int转为字符串 (在C++11其实应该使用std::to_string代替)
std::string intToStr(int i);
// 写二进制文件
bool writeFile(const char *path, const char *data, int length);
// 类似javascript的array.join(str)、python的str.join(array)
std::string joinStrings(const std::vector<std::string> &strs, const std::string &delim);
// 分割字符串，结果会被保存在res中
void splitString(const std::string &str, const std::string &delim, std::vector<std::string>& res);