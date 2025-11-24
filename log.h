/**
 * @file log.h
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для функций логирования
 * @details Содержит объявления функций для работы с системой логирования
 */

#include "connection.h"
#include "interface.h"
#include <fstream>
#include <system_error>
#include <chrono>
#include <iomanip>

/**
 * @brief Получение текущего времени в формате строки
 * @return Строка с текущим временем в формате "ГГГГ-ММ-ДД ЧЧ:ММ:СС.ммм"
 */
std::string getCurrentTime();

/**
 * @brief Запись ошибки в лог-файл
 * @param[in] logFile Имя файла для логирования
 * @param[in] errorMessage Сообщение об ошибке для записи
 */
void logError(const std::string& logFile, const std::string& errorMessage);