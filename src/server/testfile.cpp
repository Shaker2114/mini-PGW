#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <httplib.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <format>

// listen(), accept(), connect(), shutdown(), send(), recv() - только для TCP протокола
// для UDP - socket, bind, sendto, recvfrom, close

int main(int argc, char** argv)
{
    // json::parse("invalidJson"); - кидает исключение если происходит парсинг невалидного json

    /*
    Основные понятия spdlog:
    1) Logger - главный объект для логирования.
    2) Sink - куда пишем логи (файл, консоль, сеть)
    3) Level - уровень важности и детализации сообщения.
    [Code] → [Logger] → [Sinks] → [Файл, консоль, и т.д.]

    Если задать глобальный уровень явно, то он определяет нижнюю границу уровней, которые
    будут видны при логировании.
    */
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    spdlog::sink_ptr file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("filename.log", true);
    
    spdlog::logger _logger("Server", {console_sink, file_sink});
    // spdlog::logger _logger("Client", {console_sink, file_sink});
    _logger.set_level(spdlog::level::info);

    spdlog::set_default_logger(std::make_shared<spdlog::logger>(_logger));
    spdlog::info("{}", "Hello Logger!");

    _logger.debug("Logger created!");
    _logger.info("Logger created!");
    _logger.warn("Logger created!");
    _logger.error("Logger created!");
    _logger.critical("Logger created!");

    // spdlog::set_level(spdlog::level::debug); // теперь debug сообщения будут видны
    // logger.set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v"); - изменение паттерна логгера

    // базовое логирование - создаёт логгер автоматически
    // spdlog::debug("Debug message");
    // spdlog::info("Info message");
    // spdlog::warn("Warning!");
    // spdlog::error("Error!");
    // spdlog::critical("Critical error!");

    // Уровень из конфига определяет какие сообщения видны.

    httplib::Server serv;
    
    serv.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello, Web World!", "text/plain");
    });

    serv.Get("/check_subscriber", [](const httplib::Request& req, httplib::Response& res){
        std::string imsi = req.get_param_value("imsi");
        std::string response_content = "Полученный imsi: " + imsi + "\n";
        res.set_content(response_content, "text/plain");
    });

    // serv.listen("0.0.0.0", 8080);

    /*
    1) class std::chrono::system_clock - общесистемные настенные часы реального времени. Могут быть не монотонными,
    потому что на многих системах можно в любой момент скорректировать системеное время. Имеют отличную от steady_clock
    временную эпоху. Только этот класс можно переводить в C-style time. 
    Главный статический метод: std::chrono::system_clock::now() - возвращает time_point обозначающий текущий момент
    времени. 
    
    2) std::chrono::steady_clock - представляет монотонные часы. Точки времени этих часов не могут уменьшаться по
    мере того, как физическое время движется вперед, а время между тиками этих часов постоянно. Эти часы не связаны
    со временем на настенных часах (например, это может быть время с момента последней перезагрузки) и наиболее
    подходят для измерения интервалов.
    Метод: std::chrono::steady_clock::now()
    
    3) std::chrono::high_resolution_clock - Класс std::chrono::high_resolution_clock представляет часы с наименьшим
    периодом тика, предоставляемым реализацией. Это может быть псевдоним std::chrono::system_clock или
    std::chrono::steady_clock, либо третьи, независимые часы.
    Метод: std::chrono::high_resolution_clock::now()

    4) class std::chrono::time_point<class Clock, Class Duration = typename Clock::duration> - представляет собой
    момент времени, временную точку. Он реализован так, как будто хранит значение типа Duration, указывающее временной
    интервал с начала эпохи часов.

    5) class std::chrono::duration<class Rep, class Period = std::ratio<1>> - промежуток времени. Представляет собой
    временной интервал. содержит в себе количество тиков типа Rep и период тиков, где период тиков это рац. дробь
    времени компиляции, представляющая время в секундах между тиками (по умолчанию 1). Единственное что хранит 
    в себе duration - кол-во тиков типа Rep. Если Rep - floating point, то промежуток может представлять дробь
    тиков, период тоже является частью промежутка и используется только при преобразованиями между разными про-
    межутками.
    */
    auto start = std::chrono::steady_clock::now();

    std::cout << std::chrono::steady_clock::now().time_since_epoch() << std::endl;
    
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    
    std::chrono::duration<double> duration = (end - start);
    
    std::cout << duration << "s.\n";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration) << "\n";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration) << "\n";
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(duration) << std::endl;

    auto current_time_point = std::chrono::system_clock::now(); // by default - in nano seconds and in utc
    current_time_point += std::chrono::hours(3); // utc+3
    auto time_in_seconds = std::chrono::time_point_cast<std::chrono::seconds>(current_time_point); // seconds/utc+3
    std::string current_data_time = std::format("{:%Y-%m-%d %H:%M:%S}", time_in_seconds);
    std::cout << current_data_time << std::endl;
}
