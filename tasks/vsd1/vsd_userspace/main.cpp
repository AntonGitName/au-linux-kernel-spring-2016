#include <iostream>
#include <string>

#include <bsd/stdlib.h>
#include <vsd_ioctl.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static const char* DEVICE_FILE_NAME = "/dev/vsd";
static const char* HELP_MSG = ""
    "  Usage: vsd_userspace <commands>                      \n"
    "  List of commands:                                    \n"
    "   - size_get    : prints current buffer size          \n"
    "   - size_set sz : sets new buffer size (with sz bytes)\n"
    "   - help        : prints this help mesage             \n";

void print_help(std::string error_msg = "") {
    if (error_msg.empty()) {
        std::cout << HELP_MSG;
    } else {
        std::cerr << error_msg << std::endl;
        std::cerr << HELP_MSG;
    }
}

enum task_type {
    GET,
    SET,
    HELP
};

class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv) : invalidArguments(false) {
        if (argc == 2) {
            if (argv[1] == std::string("size_get")) {
                task = GET;
            } else if (argv[1] == std::string("help")) {
                task = HELP;
            } else {
                invalidArguments = true;
                error_msg = "Wrong Argument: expected 'help' or 'size_get'";
            }
        } else if (argc == 3) {
            if (argv[1] == std::string("size_set")) {
                task = SET;
                parseSize(argv[2]);
            }
        } else {
            error_msg = "Wrong Argument: check out help for usage";
            invalidArguments = true;
        }
    }

    task_type get_type() const {
        return task;
    }

    int get_sz() const {
        return set_size;
    }

    std::string get_error() const {
        return error_msg;
    }

    bool isValid() const {
        return !invalidArguments;
    }

private:
    task_type task;
    int set_size;
    bool invalidArguments;
    std::string error_msg;

    void parseSize(std::string s) {
        size_t pos;
        set_size = std::stoi(s, &pos);
        if (pos != s.length()) {
            invalidArguments = true;
            error_msg = "Inavaild Argument: Could not parse size.";
        }
    }
};

int handler_get_task() {
    vsd_ioctl_set_size_arg_t sz;
    int fd = open(DEVICE_FILE_NAME, 0);
    int error;
    if (error = ioctl(fd, VSD_IOCTL_GET_SIZE, &sz)) {
        std::cerr << "Could not read vsd size\n";
    } else {
        std::cout << "vsd size: " << sz.size << "\n";
    }
    return error;
}

int handler_set_task(int size) {
    vsd_ioctl_set_size_arg_t sz;
    sz.size = size;
    int fd = open(DEVICE_FILE_NAME, 0);
    int error;
    if (error = ioctl(fd, VSD_IOCTL_SET_SIZE, &sz)) {
        std::cerr << "Could not write vsd size\n";
    } else {
        std::cout << "new vsd size: " << size << "\n";
    }
    return error;
}

int main(int argc, char **argv) {
    ArgumentParser ap(argc, argv);

    if (!ap.isValid()) {
        print_help(ap.get_error());
        return EXIT_FAILURE;
    }

    int code = EXIT_SUCCESS;
    if (ap.get_type() == HELP) {
        print_help();
    } else if (ap.get_type() == GET) {
        code = handler_get_task();
    } else {
        code = handler_set_task(ap.get_sz());
    }
    return code ? EXIT_FAILURE : EXIT_SUCCESS;
}
