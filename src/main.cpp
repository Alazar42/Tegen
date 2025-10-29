#include "package_manager.hpp"

using json = nlohmann::json; // alias for ease of use

int main(int argc, char const *argv[]) {
    PackageManager manager;

    // Handle the -h flag for help
    if (argc == 2 && std::string(argv[1]) == "-h") {
        std::cout << "Usage: Tegen <command> [args]" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  init              Initialize a new TegenConfig.json in the current directory." << std::endl;
        std::cout << "  install <package> Install a package and add it to dependencies." << std::endl;
        std::cout << "  list              List all dependencies from TegenConfig.json." << std::endl;
        std::cout << "  build             Build the project using CMake." << std::endl;
        std::cout << "  run               Run the built project." << std::endl;
        std::cout << "  -h                Show this help message." << std::endl;
        return 0;
    }

    if (argc < 2) {
        std::cerr << "Usage: Tegen <command> [args]" << std::endl;
        std::cerr << "Run 'Tegen -h' for a list of available commands." << std::endl;
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "init") {
            manager.init();
        } else if (command == "install") {
            if (argc < 3) {
                std::cerr << "Error: Please specify a package to install." << std::endl;
                return 1;
            }
            std::string package = argv[2];
            manager.install(package);
        } else if (command == "list") {
            manager.listDependencies();
        } else if (command == "build") {
            manager.build();
        } else if (command == "run") {
            manager.run();
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            std::cerr << "Run 'Tegen -h' for help." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
