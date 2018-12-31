#!/bin/bash

set -e

if [ ! -d src ]; then
  echo "ERROR: Please run release.sh from project root. Aborting"
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
  echo "ERROR: Missing required option -k gpg_key_id"
  exit 1
fi

version=$(cat ./VERSION)

# Locate artifacts
win_installer="$(find ./dist/artifacts -name *.msi)"
win_bundle="$(find ./dist/artifacts -name *.zip)"
osx_bundle="$(find ./dist/artifacts -name *.app.zip)"
linux_installer="$(find ./dist/artifacts -name *.deb)"
linux_bundle="$(find ./dist/artifacts -name *.tar.gz)"

do_checks() {
  branch=$(git branch --list develop)

  if [ ! "${branch::1}" == "*" ]; then
    echo "ERROR: Not on develop branch. Aborting."
    exit 1
  fi

  if [ -z $win_installer ]; then
    echo "WARNING: Could not find windows installer in dist/artifacts"
  fi

  if [ -z $win_bundle ]; then
    echo "WARNING: Could not find windows bundle in dist/artifacts"
  fi

  if [ -z $osx_bundle ]; then
    echo "WARNING: Could not find OS X bundle in dist/artifacts"
  fi

  if [ -z $linux_installer ]; then
    echo "WARNING: Could not find debian installer in dist/artifacts"
  fi

  if [ -z $linux_bundle ]; then
    echo "WARNING: Could not find linux bundle in dist/artifacts"
  fi
}

show_prompt() {
  echo "====================================================================="
  echo " Performing release of PRO OFFICE CALCULATOR $version rev $revision"
  echo " GPG key: $gpg_key"
  echo " Snapshot build: $snapshot"
  echo "====================================================================="
  echo

  read -p "Continue? " -n 1 -r
  echo

  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
  fi
}

commit_version() {
  echo "********************** COMMITTING VERSION FILE **********************"

  git add ./VERSION
  git commit -m "Updated VERSION" || echo "INFO: VERSION file not changed"
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
    echo "INFO: Building snapshot release"
    ./release/build_deb.sh -rsp -k $gpg_key -n $revision
  else
    echo "INFO: Building stable release"
    ./release/build_deb.sh -rspx -k $gpg_key -n $revision
  fi

  changes_file="$(find .. -name procalc*.changes)"

  if [ $snapshot == true ]; then
    dput ppa:rjinman/snapshots "$changes_file"
  else
    dput ppa:rjinman/ppa "$changes_file"
  fi

  git add ./debian/changelog
  git commit -m "Updated debian/changelog" || echo "INFO: changelog not updated"

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

  full_version="$(get_tarball_version ..)"

  win_installer_name="ProOfficeCalculator_${full_version}.msi"
  win_bundle_name="ProOfficeCalculator_${full_version}.zip"
  osx_bundle_name="ProOfficeCalculator_${full_version}.app.zip"
  linux_installer_name="ProOfficeCalculator_${full_version}.deb"
  linux_bundle_name="ProOfficeCalculator_${full_version}.tar.gz"

  # Rename artifacts
  mv "$win_installer" "./dist/artifacts/$win_installer_name" || :
  mv "$win_bundle" "./dist/artifacts/$win_bundle_name" || :
  mv "$osx_bundle" "./dist/artifacts/$osx_bundle_name" || :
  mv "$linux_installer" "./dist/artifacts/$linux_installer_name" || :
  mv "$linux_bundle" "./dist/artifacts/$linux_bundle_name" || :

  # TODO: Add all artifacts to manifest
  cat > ./dist/artifacts/manifest <<EOF
<artifacts>
  <windows>${win_installer_name}</windows>
  <osx>${osx_bundle_name}</osx>
  <linux>${linux_bundle_name}</linux>
</artifacts>
EOF

  aws s3 cp ./dist/artifacts/ s3://proofficecalculator.com/downloads/ --recursive
}

do_checks
show_prompt
commit_version
build_deb_source_package
tag_master
tar_linux_bundle
upload_artifacts
