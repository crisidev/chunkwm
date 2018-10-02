## BitBar Notify Plugin

If you use [BitBar](https://github.com/matryer/bitbar) as a viewer for Chunkwm status, you may experience a high CPU usage updating the widget every X seconds.

This small plugin notify BitBar to refresh the chunkwm script when the desktop or the monitor is changed.

### Installation and use
The plugin can be installed either manually compiling it from its folder or via brew:

```
brew install chunkwm --with-bitbar-notifier
```

Once installed, it can be used just loading it inside `.chunkwmrc`:
```
chunkc core::load bitbar_notifier.so
```

### Variables
The plugin registers a new CVAR `bitbar_update_script` which contains a string matching the BitBar script name to update upon notification.

The CVAR is default to `"chunkwm.*?.sh"` and can be changed using

```
chunkc set bitbar_update_script "myscript.120s.py"
```

This feature is based on BitBar [refresh plugin](https://github.com/matryer/bitbar/blob/master/Docs/URLScheme.md#refreshplugin).
