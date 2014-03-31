Advanced Fast User Switching
============================


Description
-----------
Contrary to popular belief it is possible for *Windows XP* to support fast user
switching on domain-joined clients, not out-of-the-box though.
The trick is disconnect the session when it gets locked, which will display the
logon dialog instead of the lock screen and is precisely what the program does.
In addition it uses the [OEM link](http://support.microsoft.com/kb/555437) to
display an icon that allows users to switch.

Build
-----
In order to build every target you need [WiX](http://wixtoolset.org/) and the
usual build environment that comes with Microsoft Visual Studio. Simply run
`nmake` which will compile that the source and assemble a *Windows Installer*
package in `bin\Debug`.
To build a signed release package run `nmake Configuration=Release`. (A code
signing certificate needs to be installed.)

Install
-------
Simply deploy the *Windows Installer* package with your favorite deployment tool.
You can also look at the `FUSingEx.wxs` file and perform the necessary registry
changes yourself. Keep in mind that a reboot is required.
