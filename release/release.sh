#!/bin/bash

set -e

if [ ! -d src ]; then
  echo "Please run release.sh from project root. Aborting"
  exit 1
fi

source ./release/functions.sh

snapshot=true
revision=1

while getopts "k:n:x" opt
do
  case $opt in
    k) gpg_key="$OPTARG"
       ;;
    x) snapshot=false
       ;;
    n) revision="$OPTARG"
       ;;
  esac
done

if [ -z $gpg_key ]; then
  echo "Missing required option -k gpg_key_id"
  exit 1
fi

version=$(cat ./VERSION)

branch=$(git branch --list develop)
if [ ! "${branch::1}" == "*" ]; then
  echo "Not on develop branch. Aborting"
  exit 1
fi

commit_version() {
  echo "********************** COMMITTING VERSION FILE **********************"

  git add ./VERSION
  git commit -m "Updated VERSION" || echo "VERSION file not changed"
}

build_deb_source_package() {
  echo "******************** BUILDING DEB SOURCE PACKAGE  *******************"

  # Remove previous changelog files
  i=0
  for path in $(find .. -name procalc*.changes); do
    mv "$path" "${path}${i}"
    ((++i))
  done

  if [ $snapshot == true ]; then
    echo "Building snapshot release"
    ./release/build_deb.sh -rsx -k $gpg_key -n $revision
  else
    echo "Building stable release"
    ./release/build_deb.sh -rs -k $gpg_key -n $revision
  fi

  changes_file="$(find .. -name procalc*.changes)"

  if [ $snapshot == true ]; then
    dput ppa:rjinman/snapshots "$changes_file"
  else
    dput ppa:rjinman/ppa "$changes_file"
  fi

  git add ./debian/changelog
  git commit -m "Updated debian/changelog" || echo "changelog not updated"

  git push
}

tag_master() {
  echo "************************** TAGGING MASTER ***************************"

  git checkout master
  git merge develop

  if [ $snapshot != true ]; then
    git tag -a "v$version" -m "Pro Office Calculator v$version"
  fi

  git push --follow-tags
  git checkout develop
}

tar_linux_bundle() {
  echo "********************** COMPRESSING LINUX BUNDLE *********************"

  tar -czf ./dist/artifacts/procalc.tar.gz ./dist/bundles/linux/procalc
}

upload_artifacts() {
  echo "************************ UPLOADING ARTIFACTS ************************"

  win_artifact="$(find ./dist/artifacts -name *.msi)"
  osx_artifact="$(find ./dist/artifacts -name *.app.zip)"
  linux_artifact="$(find ./dist/artifacts -name *.tar.gz)"

  full_version="$(get_tarball_version ..)"

  win_artifact_name="ProOfficeCalculator_${full_version}.msi"
  osx_artifact_name="ProOfficeCalculator_${full_version}.app.zip"
  linux_artifact_name="ProOfficeCalculator_${full_version}.tar.gz"

  mv "$win_artifact" "./dist/artifacts/$win_artifact_name" || :
  mv "$osx_artifact" "./dist/artifacts/$osx_artifact_name" || :
  mv "$linux_artifact" "./dist/artifacts/$linux_artifact_name" || :

  cat > ./dist/artifacts/manifest <<EOF
<artifacts>
  <windows>${win_artifact_name}</windows>
  <osx>${osx_artifact_name}</osx>
  <linux>${linux_artifact_name}</linux>
</artifacts>
EOF

  aws s3 cp ./dist/artifacts/ s3://proofficecalculator.com/downloads/ --recursive
}

echo "====================================================================="
echo " Performing release of PRO OFFICE CALCULATOR $version rev $revision"
echo "====================================================================="
echo ""

commit_version
build_deb_source_package
tag_master
tar_linux_bundle
upload_artifacts
