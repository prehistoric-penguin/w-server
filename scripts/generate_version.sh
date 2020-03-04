#!/bin/bash
target_file=$1

COMMIT=`git log -1 --decorate=short | grep commit | head -1`
AUTHOR=`git log -1 --decorate=short | grep Author | head -1`
DATE=`git log -1 --decorate=short | grep Date | head -1`
MESSAGE=`git log -1 --decorate=short | tail -1`
GENERATE_DATE=`date -R`

cat > $target_file <<- EOM
// WARN: Auto generate file, pleaes don't modify it
// @Generate data: $GENERATE_DATE

"#include version.h" 

namespace w{
std::string VersionMessage::author_ = "$AUTHOR";
std::string VersionMessage::commit_hash_ = "$COMMIT"; 
std::string VersionMessage::date_ = "$DATE";
std::string VersionMessage::commit_message_ = "$MESSAGE";
} // namespace w

EOM
exit 0
