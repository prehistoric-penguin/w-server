#include "version_handler.h"

#include <folly/dynamic.h>
#include <folly/FBString.h>
#include <folly/json.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "version/version.h"

namespace w {

void VersionHandler::onEOM() noexcept {
  auto dyobj = folly::dynamic::object("commit_date", VersionMessage::LastCommitDate())
      ("author", VersionMessage::LastCommitAuthor())
      ("commit_hash", VersionMessage::LastCommitHash())
      ("commit_message", VersionMessage::LastCommitMessage());

  proxygen::ResponseBuilder(downstream_)
      .status(200, "OK")
      .body(folly::toJson(std::move(dyobj)))
      .sendWithEOM();
}

} // namespace w
