# ThiefWizard

ThiefWizard (TW) is Thiefhand's software distribution tool. It supports downloading, unpacking, updating, and running simply packaged software (it's all tarballs under the hood) on any desktop platform. Currently it's primarily used for distributing internal demos and playtest builds as well as shipping our internal tools and keeping them up-to-date.

TW is built with GLAD, SDL3, Dear ImGui, libarchive, libcurl and tomlplusplus.

## Configuration

By default, ThiefWizard ships with no software downloadable. All software is configured through `software.toml`, which looks something like this:

```toml
[[software]]
name = "ThiefWizard" # Must match the metadata's configured name (see below)
archive_url = "https://some.website/thiefwizard.tar"
meta_url = "https://some.website/thiefwizard.toml"
```

The meta file specified in the above configuration should be formatted something like this:

```toml
[meta]
name = "ThiefWizard" # Must match the software's configured name
description = "Thiefhand's internal software management and distribution tool."
version = "0.0.4"
exe_path = "thiefwizard" # The path within the archive to the executable file.
```

The downloaded `.tar`s are unpacked, by default, into a folder called `install` next to the TW executable. This can be reconfigured by adding a local config file called `config.toml`:

```toml
[config]
install_to = "/wherever/your/heart/desires/"
```

## Credits

Written by Angus Goucher <gus.goucher@gmail.com>

