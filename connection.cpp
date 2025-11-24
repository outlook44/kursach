/**
 * @file connection.cpp
 * @author Веселов А.Н.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций сетевого соединения и аутентификации
 * @details Серверная часть приложения, обрабатывающая подключения клиентов, аутентификацию и вычисления
 */

#include "connection.h"
#include "log.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include <system_error>

using namespace std;

/**
 * @brief Поиск пользователя в файле по логину
 * @param[in] filename Имя файла с пользователями
 * @param[in] username Логин для поиска
 * @param[out] password Найденный пароль (выходной параметр)
 * @return true если пользователь найден, false если нет
 * @details Функция читает файл в формате "логин:пароль" и ищет указанного пользователя
 */
bool findUserInFile(const std::string& filename, const std::string& username, std::string& password) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не могу открыть файл " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем пустые строки и комментарии
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        size_t pos = line.find(':');
        if (pos == std::string::npos) {
            continue; // Пропускаем строки без разделителя
        }
        
        std::string file_user = line.substr(0, pos);
        std::string file_pass = line.substr(pos + 1);
        
        // Удаляем пробелы
        file_user.erase(0, file_user.find_first_not_of(" \t"));
        file_user.erase(file_user.find_last_not_of(" \t") + 1);
        file_pass.erase(0, file_pass.find_first_not_of(" \t"));
        file_pass.erase(file_pass.find_last_not_of(" \t") + 1);
        
        // Сравниваем логины
        if (file_user == username) {
            password = file_pass;
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}

/**
 * @brief Обработка передачи данных после успешной аутентификации
 * @param[in] s Основной сокет сервера
 * @param[in] client_socket Сокет клиента
 * @param[in] p Параметры соединения
 * @return 0 при успешном выполнении
 * @throw system_error при ошибках сетевого взаимодействия
 * @details Функция получает векторы от клиента, вычисляет сумму квадратов элементов и возвращает результат
 */
int datawrite(int s, int client_socket, const Params* p){
    // Получаем количество векторов
    uint32_t vectors_count; ///< Количество векторов для обработки
    ssize_t total_received = 0;
    char* buff = reinterpret_cast<char*>(&vectors_count);

    // Получаем количество векторов
    while (total_received < sizeof(vectors_count)) {
        ssize_t received = recv(client_socket, buff + total_received, sizeof(vectors_count) - total_received, 0);
        if (received <= 0) {
            std::string errorMsg = "Ошибка recv (количество векторов): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            close(client_socket);
            close(s);
            throw std::system_error(errno, std::generic_category());
        }
        total_received += received;
    }
    std::cout << "Количество векторов: " << vectors_count << std::endl;

    // Обрабатываем каждый вектор
    for (uint32_t vector_idx = 0; vector_idx < vectors_count; vector_idx++) {
        uint32_t vector_size; ///< Размер текущего вектора
        total_received = 0;
        buff = reinterpret_cast<char*>(&vector_size);
        
        while (total_received < sizeof(vector_size)) {
            ssize_t received = recv(client_socket, buff + total_received, sizeof(vector_size) - total_received, 0);
            if (received <= 0) {
                std::string errorMsg = "Ошибка recv (размер вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
                logError(p->logFile, errorMsg);
                close(client_socket);
                close(s);
                throw std::system_error(errno, std::generic_category());
            }
            total_received += received;
        }
        std::cout << "Вектор " << vector_idx << ", длина: " << vector_size << std::endl;

        int32_t result = 0; ///< Результат вычислений для вектора
        for (uint32_t elem_idx = 0; elem_idx < vector_size; elem_idx++) {
            int32_t element; ///< Элемент вектора
            total_received = 0;
            buff = reinterpret_cast<char*>(&element);
            
            while (total_received < sizeof(element)) {
                ssize_t received = recv(client_socket, buff + total_received, sizeof(element) - total_received, 0);
                if (received <= 0) {
                    std::string errorMsg = "Ошибка recv (элемент " + std::to_string(elem_idx) + " вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
                    logError(p->logFile, errorMsg);
                    close(client_socket);
                    close(s);
                    throw std::system_error(errno, std::generic_category());
                }
                total_received += received;
            }
            
            std::cout << "  Элемент " << elem_idx << ": " << element << std::endl;
            result = result + element * element; ///< Накопление суммы квадратов
        }
        
        std::cout << "Результат: " << result << std::endl;
        ssize_t send_result = send(client_socket, &result, sizeof(result), 0);
        if (send_result == -1) {
            std::string errorMsg = "Ошибка send (результат вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            close(client_socket);
            close(s);
            throw std::system_error(errno, std::generic_category());
        }
    }
    
    return 0;
}

/**
 * @brief Основная функция установки соединения и обработки клиентов
 * @param[in] p Указатель на параметры соединения
 * @return 0 при успешном выполнении, 1 при ошибке
 * @throw system_error при ошибках сетевого взаимодействия
 * @details Функция реализует серверный сокет, принимает подключения, выполняет аутентификацию и обработку данных
 */
int connection(const Params* p) {
    ifstream errFile(p->logFile);
    
    // Создание сокета
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        std::string errorMsg = "Ошибка создания сокета: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        throw std::system_error(errno, std::generic_category());
    }

    // Настройка адреса сервера
    std::unique_ptr<sockaddr_in> self_addr(new sockaddr_in);
    self_addr->sin_family = AF_INET; ///< Семейство адресов IPv4
    self_addr->sin_port = htons(p->Port); ///< Порт сервера
    self_addr->sin_addr.s_addr = inet_addr(p->Address.c_str()); ///< IP-адрес сервера

    // Привязка сокета к адресу
    int rc = bind(s, reinterpret_cast<const sockaddr*>(self_addr.get()), sizeof(sockaddr_in));
    if (rc == -1) {
        std::string errorMsg = "Ошибка bind: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    // Прослушивание входящих соединений
    rc = listen(s, 5);
    if (rc == -1) {
        std::string errorMsg = "Ошибка listen: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    // Принятие входящего соединения
    sockaddr_in client_addr; ///< Адрес клиента
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(s, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_socket == -1) {
        std::string errorMsg = "Ошибка accept: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    // 1. Получаем логин от клиента
    char buffer[BUFFER_SIZE]; ///< Буфер для приема данных
    ssize_t received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes == -1) {
        std::string errorMsg = "Ошибка recv (логин): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    if (received_bytes > 0) {
        buffer[received_bytes] = '\0';
        std::cout << "Получен логин от клиента: " << buffer << std::endl;
    }

    string client_login(buffer); ///< Логин клиента
    
    // 2. Ищем пользователя в файле
    string user_password; ///< Пароль пользователя
    if (!findUserInFile(p->inFileName, client_login, user_password)) {
        std::string errorMsg = "Пользователь не найден: " + client_login;
        logError(p->logFile, errorMsg);
        
        // Отправляем ошибку клиенту
        string message = "ERR_USER_NOT_FOUND";
        send(client_socket, message.c_str(), message.length(), 0);
        
        close(client_socket);
        close(s);
        return 1;
    }

    std::cout << "Найден пользователь: " << client_login << ", пароль: " << user_password << std::endl;

    // 3. Отправляем соль клиенту
    string salt = "HASHHASHHASHHASH"; ///< Соль для хеширования
    string message = salt;
    ssize_t sent_bytes = send(client_socket, message.c_str(), message.length(), 0);
    if (sent_bytes == -1) {
        std::string errorMsg = "Ошибка send (соль): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    // 4. Получаем хеш от клиента
    received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes == -1) {
        std::string errorMsg = "Ошибка recv (хеш): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    if (received_bytes > 0) {
        buffer[received_bytes] = '\0';
        std::cout << "Получен хеш от клиента: " << buffer << std::endl;
    }

    // 5. Проверяем хеш
    string client_hash(buffer); ///< Хеш, полученный от клиента
    string server_hash = auth(salt, user_password); ///< Хеш, вычисленный сервером

    if (client_hash == server_hash) {
        message = "OK";
        std::cout << "Аутентификация успешна для пользователя: " << client_login << std::endl;
    } else {
        message = "ERR";
        std::string errorMsg = "Ошибка аутентификации: неверный хеш для пользователя " + client_login;
        logError(p->logFile, errorMsg);
    }

    sent_bytes = send(client_socket, message.c_str(), message.length(), 0);
    if (sent_bytes == -1) {
        std::string errorMsg = "Ошибка send (результат аутентификации): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    // Если аутентификация не прошла, завершаем соединение
    if (message != "OK") {
        close(client_socket);
        close(s);
        return 1;
    }

    // Обработка данных после успешной аутентификации
    datawrite(s, client_socket, p);
    
    close(client_socket);
    close(s);
    return 0;
}