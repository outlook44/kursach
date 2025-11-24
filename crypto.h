/**
 * @file crypto.h
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для криптографических функций
 * @details Содержит объявления функций для хеширования с использованием библиотеки CryptoPP
 */

#pragma once
#include <string>
#include <string>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
using namespace std;
namespace CPP = CryptoPP;

/**
 * @brief Функция аутентификации с использованием SHA224 хеширования
 * @param[in] salt Соль для хеширования
 * @param[in] pass Пароль пользователя
 * @return Хеш-строка в hex-формате
 * @details Использует алгоритм SHA224 для создания хеша от конкатенации соли и пароля
 */
string auth(string salt, string pass);