# Pro Office Calculator

## Releases

This document details the release process for Pro Office Calculator.

All work is merged to the `develop` branch during development.

When the current state of `develop` is ready for release, these are the steps that need to happen.

* If releasing a stable (non-snapshot) build, bump the version number in the VERSION file to match
the stable version (i.e. no snapshot suffix). Leave unchanged for snapshot releases.
* Build the project and create distributable packages for OS X, Windows, and Linux, making changes
and pushing new commits if necessary. Every time a new commit is made, may need to rebuild on all
other systems to ensure each platform's build is up-to-date.
* Transfer all compiled artifacts to the `dist/artifacts` directory on the main Linux
workstation. This includes all installers and zipped standalone bundles.

The `dist` directory should now look something like as follows (the artifact names are unimportant
as the release script will rename them).

```
    $ tree -L 3

    .
    ├── artifacts
    │   ├── manifest
    │   ├── ProOfficeCalculator_1.0.12.app.zip
    │   ├── ProOfficeCalculator_1.0.12.deb
    │   ├── ProOfficeCalculator_1.0.12.msi
    │   ├── ProOfficeCalculator_1.0.12.tar.gz
    │   └── ProOfficeCalculator_1.0.12.zip
    └── bundles
        ├── linux
        │   ├── AppRun -> procalc/bin/procalc
        │   ├── procalc
        │   ├── procalc.desktop
        │   └── procalc.png
        ├── osx
        │   └── procalc.app
        └── windows
            └── procalc
```

Run `release.sh`, which will do the following:

* Commit the VERSION file.
* Run the build_deb.sh script.
* Commit the new changelog.
* Merge develop into master. If this is a stable build (i.e. non-snapshot), tag the master branch
with the version number prefixed with the letter v, e.g. v1.0.15. The annotation for the tag should
have the form `Pro Office Calculator v1.0.15`.
* Upload the source package to the launchpad PPAs. If the build is a snapshot, upload to the
snapshot PPA; if it's a stable build, upload to the non-snapshot PPA.
* Name the artifacts with the version number, including any snapshot suffix, e.g.
ProOfficeCalculator_1.0.15+16SNAPSHOT20180719015812.msi.
* Upload artifacts to S3.
* Upload a manifest file to S3 so the website can know the versions stored in the bucket and which
is the latest, etc.


## Deployment

Pro Office Calculator is currently deployed to:

* Ubuntu PPAs `ppa:rjinman/ppa` and `ppa:rjinman/snapshots`
* Official website http://proofficecalculator.com
* Itch.io https://robjinman.itch.io/pro-office-calculator
* Game Jolt https://gamejolt.com/games/pro_office_calculator/358406
* Indie DB https://www.indiedb.com/games/pro-office-calculator
* Steam https://store.steampowered.com/app/914430/Pro_Office_Calculator

Deployments to the PPAs and the website are performed by the release script. Deployments to other
destinations involve manually uploading the artifacts via the relevant websites.


### Steam

Install the Steam SDK somewhere (e.g. /opt/steamworks), then from the project root log into the
console like so

```
    rlwrap /opt/steamworks/sdk/tools/ContentBuilder/builder_linux/steamcmd.sh +login account password
```

And at the prompt, run the following, specifying the full path to the app build file

```
    run_app_build /absolute/path/to/steam/app_build_914430.vdf
```

Then log into Steamworks and set the build as the default.
