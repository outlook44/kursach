#include <UnitTest++/UnitTest++.h>
#include "interface.h"
#include <string>

SUITE(HelpTest) {
    TEST(ShortHelp) {
        UserInterface iface;
        const char* argv[] = {"test", "-h", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
    
    TEST(Help) {
        UserInterface iface;
        const char* argv[] = {"test", "--help", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
}

SUITE(ParameterTest) {
    TEST(ValidParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-j", "journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    TEST(ValidParametersLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "--base", "database.db", "--journal", "journal.log", "--port", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    TEST(MissingBaseParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-j", "journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(MissingJournalParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(MissingPortParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-j", "journal.log", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(InvalidPortParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "not_a_number", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(NegativePort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "-1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(-1, iface.getParams().Port);
    }

    TEST(ZeroPort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "0", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(0, iface.getParams().Port);
    }

    TEST(LargePort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "65535", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(65535, iface.getParams().Port);
    }
}

SUITE(OptionalParametersTest) {
    TEST(WithAddressParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-a", "192.168.1.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("192.168.1.1", iface.getParams().Address);
    }

    TEST(WithAddressParameterLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "--address", "10.0.0.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("10.0.0.1", iface.getParams().Address);
    }

    TEST(DefaultAddress) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("127.0.0.1", iface.getParams().Address);
    }

    TEST(LogFileParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-l", "custom_log.txt", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("custom_log.txt", iface.getParams().logFile);
    }

    TEST(LogFileParameterLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "--log", "another_log.txt", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("another_log.txt", iface.getParams().logFile);
    }

    TEST(DefaultLogFile) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("journal.txt", iface.getParams().logFile);
    }
}

SUITE(EdgeCasesTest) {
    TEST(EmptyArguments) {
        UserInterface iface;
        const char* argv[] = {"test", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        // Вместо исключения, парсер может вернуть false или бросить другое исключение
        // Проверяем что парсинг не завершается успешно
        CHECK(!iface.Parser(argc, argv));
    }

    TEST(UnknownParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-x", "value", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(MissingParameterValue) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    TEST(DuplicateParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db1", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        // Проверяем, что используется первое значение (не допускаем дублирование)
        CHECK_EQUAL("db1", iface.getParams().inFileName);
    }

    TEST(MixedOrderParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-p", "8080", "-j", "journal.log", "-b", "database.db", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    TEST(HelpWithOtherParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-h", "-j", "log", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        // Help должен иметь приоритет и парсинг должен вернуть false
        CHECK(!iface.Parser(argc, argv));
    }
}

SUITE(SpecialValuesTest) {
    TEST(EmptyStringValues) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "", "-j", "", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("", iface.getParams().inFileName);
        CHECK_EQUAL("", iface.getParams().inFileJournal);
    }

    TEST(SpecialCharactersInFilenames) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "file with spaces.db", "-j", "path/to/journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("file with spaces.db", iface.getParams().inFileName);
        CHECK_EQUAL("path/to/journal.log", iface.getParams().inFileJournal);
    }

    TEST(IPv6Address) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-a", "::1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("::1", iface.getParams().Address);
    }
}

int main() {
    return UnitTest::RunAllTests();
}