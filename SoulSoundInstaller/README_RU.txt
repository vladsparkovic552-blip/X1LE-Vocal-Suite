# SoulSound 2.9.0 — Windows VST3 installer

## One-click installer

The repository builds a graphical Windows installer automatically with GitHub Actions.

1. Publish the plugin archive in GitHub Releases. The asset name must be exactly:
   \`SoulSound_beta_2.9.0_1118_0004.zip\`
2. Open **Actions → Build SoulSound Installer**.
3. Open the newest successful run and download the **SoulSound-Setup-2.9.0** artifact.
4. Run \`SoulSound_Setup_2.9.0.exe\`. Windows will request administrator permission.
5. Restart FL Studio and run **Options → Manage plugins → Find installed plugins**.

The setup downloads the ZIP directly from the latest GitHub Release, checks that it contains the expected 64-bit VST3 bundle, backs up an existing SoulSound installation, and installs to \`%CommonProgramFiles%\VST3\`.

## Command-line fallback

You can also run \`INSTALL.bat\`. It performs the same download-and-install flow and requires the same published Release asset.

## Important

The Release asset must keep the exact filename above. The installer does not contain the plugin binary; it downloads it during installation. An internet connection is required.
