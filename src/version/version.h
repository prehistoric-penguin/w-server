#pragma once

#include <string>

namespace w {

class VersionMessage {
 public:
  static std::string LastCommitAuthor() { return author_; }
  static std::string LastCommitHash() { return commit_hash_; }
  static std::string LastCommitDate() { return date_; }
  static std::string LastCommitMessage() { return commit_message_; }

 private:
  static std::string author_;
  static std::string commit_hash_;
  static std::string date_;
  static std::string commit_message_;
};

} // namespace w
