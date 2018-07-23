#include "Utils.h"
#include "json\writer.h"
#include <string>

std::string serializeRapidjsonDocument(const rapidjson::Document &doc) {
  rapidjson::StringBuffer buffer;
  buffer.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString());
}

std::string intToStr(int i) {
  char buf[32] = {};
  sprintf(buf, "%d", i);
  return std::string(buf);
}

bool writeFile(const char * path, const char * data, int length) {
  FILE *file = fopen(path, "wb");
  if (file) {
    fwrite(data, sizeof(char), length, file);
    printf("Saved %d bytes in %s\n", length, path);
    fclose(file);
  } else {
    printf("Save failed\n");
    return false;
  }
  return true;
}

std::string joinStrings(const std::vector<std::string>& strs, const std::string & delim) {
  std::string ret;
  for (int i = 0; i < strs.size(); ++i) {
    if (i == 0) ret += strs[i];
    else ret += delim + strs[i];
  }
  return ret;
}

void splitString(const std::string &str, const std::string &delim, std::vector<std::string>& res) {
  if (str.empty()) return;
  res.clear();
  int offset = 0;
  std::string::size_type find_res = 0;
  while ((find_res = str.find(delim, offset)) != std::string::npos) {
    res.push_back(str.substr(offset, find_res - offset));
    offset = find_res + delim.size();
  }
  res.push_back(str.substr(offset));
}
