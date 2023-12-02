#include <ostream>
// Reference: https://stackoverflow.com/a/17469726 by Joel Sjögren
namespace Color {
    enum Code {
        FG_RED          = 31,
        FG_GREEN        = 32,
        FG_BLUE         = 34,
        FG_YELLOW       = 33,
        FG_MAGENTA      = 35,
        FG_CYAN         = 36,
        FG_LIGHT_GREY   = 37,
        FG_DARK_GREY    = 90,
        FG_LIGHT_RED    = 91,
        FG_LIGHT_GREEN  = 92,
        FG_LIGHT_YELLOW = 93,
        FG_LIGHT_BLUE   = 94,
        FG_LIGHT_MAGENTA= 95,
        FG_LIGHT_CYAN   = 96,
        FG_WHITE        = 97,
        FG_DEFAULT      = 39,
        BG_RED          = 41,
        BG_GREEN        = 42,
        BG_BLUE         = 44,
        BG_DEFAULT      = 49,
        RESET_ALL       = 0,
        REST_BOLD_BRIGHT= 21,
        RESET_DIM       = 22,
        RESET_UNDERLINE = 24,
        RESET_BLINK     = 25,
        RESET_REVERSE   = 27,
        RESET_HIDDEN    = 28,
        BOLD            = 1,
        DIM             = 2,
        UNDERLINED      = 4,
        BLINK           = 5,
        REVERSE         = 7,
        HIDDEN          = 8
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}
