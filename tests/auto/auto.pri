CONFIG += qtestlib testcase

harmattan {
    INSTALLS += target qmlfiles
    qmlfiles.files = $$OTHER_FILES
    qmlfiles.path = $$INSTALL_PREFIX/lib/qt-components-tests/auto/meego/$$TARGET
    target.path = $$INSTALL_PREFIX/lib/qt-components-tests/auto/meego/$$TARGET
}

