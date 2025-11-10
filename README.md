# Mini-PGW

Упрощённая модель сетевого компонента PGW (Packet Gateway) для обработки UDP-запросов и управления сессиями абонентов.

## Возможности

- Обработка UDP-запросов с IMSI
- Управление сессиями абонентов по IMSI
- CDR-журналирование
- HTTP API для мониторинга и управления
- Чёрный список IMSI
- Graceful shutdown
- Многопоточная архитектура
- Логирование ключевых действий
- Чтение настроек из JSON-конфига

## Сборка

### Требования
- C++20 и выше
- CMake 3.20+
- Linux

### Сборка проекта
```bash
git clone <repository-url>
cd mini-PGW

cmake . && make
```

### Запуск
```bash
./pgw_server

./pgw_client <IMSI>
# Например: ./pgw_client 001010000000001
```

### Тестирование
```bash
make test

# или
./pgw_tests

# Запуск тестов конкретного модуля
./pgw_tests --gtest_filter="BCDConverterTest*"
./pgw_tests --gtest_filter="SessionManagerTest*"
```

## Конфигурация
```text
Сервер (configs/pgw_server_cfg.json)
{
    "udp_ip": "0.0.0.0",
    "udp_port": 9000,
    "session_timeout_sec": 30,
    "cdr_file": "cdr.log",
    "http_port": 8080,
    "graceful_shutdown_rate": 3,
    "log_file": "pgw.log",
    "log_level": "INFO",
    "blacklist": ["001010123456789", "001010000000001"]
}

Клиент (configs/pgw_client_cfg.json)
{
    "server_ip": "127.0.0.1",
    "server_port": 9000,
    "log_file": "client.log",
    "log_level": "INFO"
}
```

## HTTP API

Проверка статуса абонента
```bash
curl "http://0.0.0.0:8080/check_subscriber?imsi=001010000000001"
```
Ответ: active или not active

Graceful shutdown сервера
```bash
curl "http://0.0.0.0:8080/stop"
```
Ответ: Starting graceful shutdown...

## Протокол взаимодействия

### UDP запрос от клиента

- Формат: IMSI в BCD-кодировке
- Размер: до 100 байт

### UDP ответ от сервера

- Формат: ASCII строка
- Варианты: created (сессия создана) или rejected (отклонено)

### CDR записи

Формат: timestamp, IMSI, action

```text
01-12-2024 14:30:25, 001010000000001, session_created
01-12-2024 14:30:55, 001010000000001, session_timeout
01-12-2024 14:31:20, 001010000000002, session_created
01-12-2024 14:31:30, 001010000000002, session_ended
```

## Технологический стек

- Язык: C++20
- Сборка: CMake с FetchContent для зависимостей
- Тестирование: GoogleTest
- Логирование: spdlog с уровнями debug/info/warn/error/critical
- JSON: nlohmann/json для конфигурации
- HTTP: cpp-httplib для HTTP API
- Сети: Berkeley sockets для UDP
- Многопоточность: std::thread, std::atomic

## Особенности реализации

- Graceful shutdown: Постепенное завершение сессий с настраиваемой скоростью
- Валидация IMSI: Проверка формата и длины (1-15 цифр)
- Таймауты сессий: Автоматическое удаление неактивных сессий
- Чёрный список: Мгновенный отказ для заблокированных IMSI
- Non-blocking сокеты: Эффективная обработка запросов
- Изоляция тестов: Mock-объекты и временные файлы
