# Mini-PGW

Упрощённая модель сетевого компонента PGW (Packet Gateway) для обработки UDP-запросов и управления сессиями абонентов.

## Возможности

- Обработка UDP-запросов с IMSI
- Управление сессиями абонентов по IMSI
- CDR-журналирование (Call Detail Records)
- HTTP API для мониторинга и управления
- Чёрный список IMSI
- Graceful shutdown
- Многопоточная архитектура
- Логирование ключевых действий

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
```

### Тестирование
```bash
make tests

# или
./pgw_tests

# Запуск тестов конкретного модуля
./pgw_tests --gtest_filter="BCDConverterTest*"
./pgw_tests --gtest_filter="SessionManagerTest*"
```
