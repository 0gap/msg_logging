//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_TEST_BINDING_H
#define MSG_TEST_BINDING_H

#include <iostream>
#include <vector>
#include <absl/strings/str_split.h>
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

struct Binding
{
  std::string protocol;
  std::string host;
  int64_t port;
  std::string full_string;
  std::string starred_string;
  explicit Binding(std::string b)
  {
    auto it = std::remove(b.begin(), b.end(), '/');
    b.erase(it, b.end());
    std::vector<std::string> parts = absl::StrSplit(b, ':');

    protocol = parts[0];
    host = parts[1];
    LOG(INFO) << "Creating binding from " << b;
    if (parts.size() > 2)
    {
      LOG(INFO) << "Port string: " << parts[2];
      std::stringstream s(parts[2]);
      s >> port;
      LOG(INFO) << "To finally get: " << protocol << "://" << host << ":"
                << port;
      full_string = protocol + "://" + host + ":" + absl::StrCat(port);
      starred_string = protocol + "://*:" + absl::StrCat(port);
    }
    else
    {
      full_string = protocol + "://" + host;
      starred_string = protocol + "://*";
      LOG(INFO) << "To finally get: " << protocol << "://" << host;
    }
  }
  [[nodiscard]] std::string star_host() const
  {
    //    return protocol + "://*:" + std::to_string(port);
    return starred_string;
  }

  operator std::string() const
  {
    return full_string;
//    return protocol + "://" + host + ":" + absl::StrCat(port);
  }
};

#endif//MSG_TEST_BINDING_H
