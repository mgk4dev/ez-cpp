
#include <ez/async/Schedule.hpp>
#include <ez/flow/Exporter.hpp>
#include <ez/flow/Flow.hpp>

#include <ez/Result.hpp>

#include "Extensions.hpp"

#include <boost/program_options.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace ez;

struct Arguments {
    bool print_help = false;
    std::filesystem::path program_path;
    bool export_ = false;
    bool eval = false;
    size_t instance_count = 1;
};

Result<Arguments, std::string> parse_arguments(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::variables_map arguments;
    po::options_description usage{"Usage"};

    Arguments result;

    struct {
        const char* input_file = "input-file";
        const char* output = "output";
        const char* help = "help,h";
        const char* export_ = "export";
        const char* eval = "eval";
        const char* instance_count = "instance-count";

    } option_name;

    try {
        po::positional_options_description positional_args;
        positional_args.add(option_name.input_file, -1);

        // clang-format off
        usage.add_options()
            (option_name.help, "produce help message")
            (option_name.input_file, po::value<std::string>(), "Input file")
            (option_name.export_, "Export to program to JSON")
            (option_name.eval, "Evaluate the program")
            (option_name.instance_count, po::value<size_t>(), "Instance count")

        ;
        // clang-format on

        po::store(
            po::command_line_parser(argc, argv).options(usage).positional(positional_args).run(),
            arguments);
        po::notify(arguments);
    }
    catch (const std::exception& exception) {
        return Fail{exception.what()};
    }

    if (arguments.count(option_name.help)) {
        std::cout << usage << "\n";
        result.print_help = true;
        return result;
    }

    if (arguments.count(option_name.input_file)) {
        result.program_path = arguments[option_name.input_file].as<std::string>();
    }
    else {
        return Fail{"Input file not set"};
    }

    result.eval = arguments.count(option_name.eval);
    result.export_ = arguments.count(option_name.export_);

    if (arguments.count(option_name.instance_count)) {
        result.instance_count = arguments[option_name.instance_count].as<size_t>();
    }

    return result;
}

Result<std::string, std::string> read_file(const std::filesystem::path& path)
{
    std::ifstream file;
    file.open(path, std::ios_base::in);

    if (!file.is_open()) return Fail{"failed to open file"};

    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

int main(int argc, char** argv)
{
    auto arg_result = parse_arguments(argc, argv);

    Logger logger;
    logger.log_impl = [](LogLevel level, const std::string& msg) {
        std::cout << [level] {
            switch (level) {
                case LogLevel::Info: return "[INFO   ] ";
                case LogLevel::Warning: return "[WARNING] ";
                case LogLevel::Error: return "[ERROR  ] ";
                case LogLevel::Trace: return "[TRACE  ] ";
            }
            std::terminate();
        }() << msg << "\n";
    };

    if (!arg_result) {
        std::cout << "Program option error: " << arg_result.error() << std::endl;
        return -1;
    }

    const Arguments& arguments = arg_result.value();

    auto file_contents = read_file(arguments.program_path);

    if (!file_contents) {
        logger.error("Failed to read input file {}", file_contents.error());
        return -1;
    }

    if (arguments.export_) {
        auto program_result = flow::parse(file_contents.value(), arguments.program_path.string());

        if (!program_result) {
            logger.error("Failed to read input file {}", program_result.error_message);
            logger.error("{}", program_result.error_message);
            return -1;
        }

        flow::JsonExporter exporter{std::cout};

        exporter.run(program_result);
    }

    if (!arguments.eval) return 0;

    io::Context io_context;
    io::WorkGuard guard{io_context};
    io::ThreadPool thread_pool{1};
    flow::Engine engine{io_context};

    engine.set_logger(logger);
    flow::ext::setup_engine(engine, io_context, thread_pool);

    for (size_t i = 0; i < arguments.instance_count; ++i)
        engine.eval(file_contents.value(), arguments.program_path.string(), i);

    io_context.run();

    return 0;
}
