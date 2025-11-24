/**
 * @file connection.h
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для функций сетевого соединения
 * @details Содержит объявления функций для работы с сетевыми соединениями и аутентификации
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
#define BUFFER_SIZE 1024 ///< Размер буфера для сетевого обмена
using namespace std;

/**
 * @brief Основная функция установки соединения и обработки клиентов
 * @param[in] p Указатель на параметры соединения
 * @return 0 при успешном выполнении, 1 при ошибке
 * @throw system_error при ошибках сетевого взаимодействия
 */
int connection(const Params* p);