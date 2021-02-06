# Plugin providing garbage collection hook to TJS

This plugin adds garbage collection hook function in Kirikiri2 / 吉里吉里2 / KirikiriZ / 吉里吉里Z

## Building

After cloning submodules and placing `ncbind` and `tp_stub` in the parent directory, a simple `make` will generate `systemExGarbageCollectionHook.dll`.

## How to use

After `Plugins.link("systemExGarbageCollectionHook.dll");` is used, the `addGarbageCollectionHook` and `removeGarbageCollectionHook` function will be exposed under the `System` class.

## License

This project is licensed under the MIT license. Please read the `LICENSE` file for more information.
