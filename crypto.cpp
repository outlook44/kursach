/**
 * @file crypto.cpp
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация криптографических функций
 * @details Реализует функцию хеширования с использованием алгоритма SHA224
 */

#include "crypto.h"

/**
 * @brief Функция аутентификации с использованием SHA224 хеширования
 * @param[in] salt Соль для хеширования
 * @param[in] pass Пароль пользователя
 * @return Хеш-строка в hex-формате
 * @details Вычисляет SHA224 хеш от конкатенации соли и пароля, результат возвращает в hex-формате
 */
string auth(string salt, string pass){
    CPP::SHA224 sha224; ///< Объект для вычисления SHA224 хеша
    string hash; ///< Результирующий хеш
    
    CPP::StringSource(
        string(salt).append(pass), ///< Конкатенация соли и пароля
        true,
        new CPP::HashFilter(
            sha224, ///< Использование SHA224 для хеширования
            new CPP::HexEncoder( ///< Кодирование результата в hex
                new CPP::StringSink(hash)))
    );

    return hash; ///< Возврат хеша в hex-формате
}