#ifndef PRINT_OPTIONS_HPP
#define PRINT_OPTIONS_HPP

struct PrintOptions {
	bool debug = false;
	bool condensed = false;
	bool sections = false;
    bool parenthesis = false;

    PrintOptions add_parenthesis(bool enable = true) {
        auto new_options = *this;
        new_options.parenthesis = enable;
        return new_options;
    }
};

#endif