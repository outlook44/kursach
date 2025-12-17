/**
 * @file connection.h
 * @author Веселов А.Н.
 * @brief Заголовочный файл класса сетевого соединения
 */

#pragma once
#include "errno.h"
#include "crypto.h"
#include "interface.h"
#include <system_error>
#include <netinet/in.h>
#include <memory>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>

/// Размер буфера для сетевого обмена
#define BUFFER_SIZE 1024

using namespace std;

/**
 * @class Connection
 * @brief Класс для управления сетевыми соединениями
 */
class Connection
{
public:
    /**
     * @brief Устанавливает соединение и обрабатывает клиентов
     * @param[in] p Параметры соединения
     * @return 0 при успехе, 1 при ошибке
     * @throw system_error при сетевых ошибках
     */
    static int connection(const Params* p);
};