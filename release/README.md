# Pro Office Calculator

## Releases

This document details the release process for Pro Office Calculator.

All work is merged to the `develop` branch during development.

When the current state of `develop` is ready for release, these are the steps that need to happen.

* If releasing a stable (non-snapshot) build, bump the version number in the VERSION file to match
the stable version (i.e. no snapshot suffix). Leave unchanged for snapshot releases.
* Build the project and create distributable packages for OS X, Windows, and Linux, making changes
and pushing new commits if necessary. Every time a new commit is made, rebuild on all other systems
to ensure each platform's build is up-to-date.
* Transfer the Windows and Mac artifacts to the `artifacts` directory on the main Linux workstation.

Run `release.sh`, which will do the following:

* Run the build_deb.sh script.
* Commit the new changelog and VERSION file.
* Merge develop into master. If this is a stable build (i.e. non-snapshot), tag the master branch
with the version number prefixed with the letter v, e.g. v1.0.15. The annotation for the tag should
have the form `Pro Office Calculator v1.0.15`.
* Upload the source package to the launchpad PPAs. If the build is a snapshot, upload only to the
snapshot PPA; if it's a stable build, upload to both PPAs.
* Name the Windows and OS X artifacts with the version number, including any snapshot suffix, e.g.
ProOfficeCalculator_1.0.15+16SNAPSHOT20180719015812.msi.
* Upload artifacts to S3.
* Upload a manifest file to S3 so the website can know the versions stored in the bucket and which
is the latest, etc.
