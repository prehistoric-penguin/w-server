#!/bin/bash
target_file=$1

COMMIT=`git log -1 --decorate=short | grep commit | head -1`
AUTHOR=`git log -1 --decorate=short | grep Author | head -1`
DATE=`git log -1 --decorate=short | grep Date | head -1`
MESSAGE=`git log -1 --decorate=short | tail -1`
GENERATE_DATE=`date -R`

echo "// An auto generated file,do not modify it." > $target_file
echo "// @Generate date:"$GENERATE_DATE >> $target_file
echo "" >> $target_file
echo "#include \"version.h\"" >> $target_file
echo "" >> $target_file
echo "namespace w {" >> $target_file
echo "std::string VersionMessage::author_ = \"$AUTHOR\";" >> $target_file
echo "std::string VersionMessage::commit_hash_ = \"$COMMIT\";" >> $target_file
echo "std::string VersionMessage::date_ = \"$DATE\";" >> $target_file
echo "std::string VersionMessage::commit_message_ = \"$MESSAGE\";" >> $target_file
echo "} // namespace w" >> $target_file
exit 0
