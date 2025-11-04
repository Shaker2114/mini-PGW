#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <httplib.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

    serv.listen("0.0.0.0", 8080);
}
