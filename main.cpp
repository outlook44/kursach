/**
 * @file main.cpp
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @brief Главный файл серверного приложения
 * @details Точка входа в программу, обработка параметров и запуск сервера
 */

#include "connection.h"
#include "interface.h"

/**
 * @brief Главная функция серверного приложения
 * @param[in] argc Количество аргументов командной строки
 * @param[in] argv Массив аргументов командной строки
 * @return 0 при успешном выполнении, 1 при ошибке параметров
 */
int main(int argc, const char** argv)
{
    UserInterface userinterface; // Объект для работы с пользовательским интерфейсом
    
    // Парсинг аргументов командной строки
    if(!userinterface.Parser(argc, argv)){
        cout << userinterface.getDescription() << endl;
        return 1;
    }
    
    // Получение параметров и запуск сервера
    Params params = userinterface.getParams();
    Connection::connection(&params);
    
    return 0;
}