/**
 * @file interface.cpp
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация пользовательского интерфейса
 * @details Реализует функциональность парсинга и валидации параметров командной строки
 */

#include "interface.h"

/**
 * @brief Конструктор класса UserInterface
 * @details Инициализирует опции командной строки с обязательными параметрами и значениями по умолчанию
 */
UserInterface::UserInterface() : desc("Allowed options")
{
    // добавление параметров в парсер командной строки
    desc.add_options()
    ("help,h", "Show help") ///< Опция для вывода справки
    ("log,l", po::value<string>(&params.logFile)->default_value("journal.txt"), "Set log file") ///< Файл логирования (по умолчанию journal.txt)
    ("base,b", po::value<std::string>(&params.inFileName)->required(),"Set input data base name") ///< Обязательный параметр: файл базы пользователей
    ("journal,j", po::value<std::string>(&params.inFileJournal)->required(),"Set journal file name") ///< Обязательный параметр: файл журнала
    ("port,p", po::value<int>(&params.Port)->required(), "Set port") ///< Обязательный параметр: порт сервера
    ("address,a", po::value<string>(&params.Address)->default_value("127.0.0.1"), "Set address"); ///< Адрес сервера (по умолчанию 127.0.0.1)
}

/**
 * @brief Парсинг аргументов командной строки
 * @param[in] argc Количество аргументов
 * @param[in] argv Массив аргументов
 * @return true если парсинг успешен, false если требуется показать справку
 * @throw exception при ошибках парсинга или отсутствии обязательных параметров
 */
bool UserInterface::Parser(int argc, const char** argv)
{
    // Если нет аргументов или только --help, показываем справку
    if (argc == 1) {
        return false;
    }
    // парсинг командной строки
    po::store(po::parse_command_line(argc, argv, desc), vm);
    // обрабатываем --help до вызова notify
    if (vm.count("help"))
    return false;
    // проверка обязательных параметров и присвоение значений
    po::notify(vm);
    return true;
}

/**
 * @brief Получение описания параметров
 * @return Строка с описанием поддерживаемых опций
 */
std::string UserInterface::getDescription()
{
    std::ostringstream ss;
    ss << desc;
    return ss.str();
}