# <img src="logo.png" width="40"> NinKaz's Editor Utils

A small collection of editor utilities to help make my life a little easier when creating.

This mod is available on the [geode index](https://geode-sdk.org/mods/ninkaz.editor_utils) for Windows, Mac, and Android. After installing Geode, search for it in the in-game mod browser and click install.

## Features

- **Multi-Edit Triggers:** edit a property across multiple triggers at once
- **Improved Select Filter:** filter objects based on group ID, color, HSV, scale, Z order, and Z layer
- **Custom Paste State:** choose which attributes to paste when pasting object states
- **Distribute Groups:** randomly distribute a set of groups between a set of objects
- **Hide Trigger UI:** quickly hide the trigger UI to view your changes

## Build instructions

This mod can be built just like any other Geode mod. For more information, see the [geode docs](https://docs.geode-sdk.org/getting-started/cpp-stuff/).

```sh
# Assuming you have the Geode CLI set up already
geode build
```

Some Geode-specific macros don't compile on MSVC. If you're having issues, try using Clang.
