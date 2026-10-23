# AppImage AppArmor profile

`antimony-appimage.apparmor` lets the Antimony AppImage start its Chromium sandbox on distros that restrict unprivileged user namespaces (Ubuntu 23.10 and later, including 24.04 and 26.04).

## Install (once, as root)

```
sudo install -m 0644 antimony-appimage.apparmor /etc/apparmor.d/antimony-appimage
sudo apparmor_parser -r /etc/apparmor.d/antimony-appimage
```

Remove:

```
sudo apparmor_parser -R /etc/apparmor.d/antimony-appimage
sudo rm /etc/apparmor.d/antimony-appimage
```

## What it grants

`userns` only, for executables matching `/tmp/.mount_*/usr/bin/antimony`, the path the AppImage runtime mounts. The profile is `flags=(unconfined)`, so it does not restrict file access. Known weakness: any executable named `antimony` under `/tmp/.mount_*` gets the same grant.

The deb and rpm do not need this. They install their own profile, `antimony-stable`.

## Test status

- Negative control (profile unloaded, run as the ubuntu user): Chromium exits with "No usable sandbox", and the kernel logs an AppArmor DENIED for `sys_admin`.
- With the profile loaded: the browser runs in init's user namespace, and the zygotes and renderers run in their own sandbox namespace. No AppImage denials in that window.
- Tested on the arm64 AppImage (Chromium 156.0.8078.47) on Ubuntu 26.04 with `kernel.apparmor_restrict_unprivileged_userns=1`. Not yet tested on the x86_64 AppImage.
