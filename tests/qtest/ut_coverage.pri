
defineReplace(srcList) {
    LIST=$$1
    for(item,LIST) {
        ITEMLIST=$$item $${ITEMLIST}
    }
    return($${ITEMLIST})
}

QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
LIBS += -lgcov

coverage.depends = all
coverage.commands = gcov -b -o ${OBJECTS_DIR} $$srcList($$TEST_SOURCES)

QMAKE_CLEAN += *.gcda *.gcno *.gcov
QMAKE_DISTCLEAN += *.gcda *.gcno *.gcov

QMAKE_EXTRA_TARGETS += coverage
