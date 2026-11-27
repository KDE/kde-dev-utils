# Qt Permission API Simulator

This allows testing applications using the `QPermission` API on Linux outside of
a Flatpak sandbox and without accessing or changing the permission store.

## Interactive use

```
qpermission-simulator <app>
```

When the application requests a permission a message box is shown to select whether
the permission was granted or rejected.

## Non-interactive use

```
qpermission-simulator [--grant|--deny] <app>
```

The app will receive the specified response when requesting a permission.
