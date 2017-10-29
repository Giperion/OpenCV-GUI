@echo off
SetLocal EnableDelayedExpansion
(set PATH=F:\QT_BUILD\qt-everywhere-opensource-src-5.9.0\qtbase\bin;!PATH!)
if defined QT_PLUGIN_PATH (
    set QT_PLUGIN_PATH=F:\QT_BUILD\QT-EVERYWHERE-OPENSOURCE-SRC-5.9.0\QTBASE\plugins;!QT_PLUGIN_PATH!
) else (
    set QT_PLUGIN_PATH=F:\QT_BUILD\QT-EVERYWHERE-OPENSOURCE-SRC-5.9.0\QTBASE\plugins
)
F:\QT_BUILD\qt-everywhere-opensource-src-5.9.0\qtbase\bin\uic.exe %*
EndLocal
