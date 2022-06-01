/** @file console_colors.h
* Defines constant macros representing strings which allows to modify the font and background colors of consoles.
* This constants are strings that should be concatenated to the string to output to stdin/stderr.
* ```c
* //Example:
* printf(CONSOLE_FOREGROUND_BRIGHT_RED "Error: Couldn't allocate memory!" CONSOLE_COLOR_RESET);
* ```
* To remove every colored output, define `COLTI_NO_COLORED_OUTPUT` before including the file.
*/

#ifndef HG_COLTI_CONSOLE_COLORS
#define HG_COLTI_CONSOLE_COLORS

#ifndef COLTI_NO_COLORED_OUTPUT

/******************************************************
FOREGROUND COLORS FOR CONSOLE
******************************************************/

/// @brief Sets the console font color to black
#define CONSOLE_FOREGROUND_BLACK			"\x1B[30m"
/// @brief Sets the console font color to red
#define CONSOLE_FOREGROUND_RED				"\x1B[31m"
/// @brief Sets the console font color to green
#define CONSOLE_FOREGROUND_GREEN			"\x1B[32m"
/// @brief Sets the console font color to yellow
#define CONSOLE_FOREGROUND_YELLOW			"\x1B[33m"
/// @brief Sets the console font color to blue
#define CONSOLE_FOREGROUND_BLUE				"\x1B[34m"
/// @brief Sets the console font color to magenta
#define CONSOLE_FOREGROUND_MAGENTA			"\x1B[35m"
/// @brief Sets the console font color to cyan
#define CONSOLE_FOREGROUND_CYAN				"\x1B[36m"
/// @brief Sets the console font color to white
#define CONSOLE_FOREGROUND_WHITE			"\x1B[37m"

/// @brief Sets the console font color to bright black
#define CONSOLE_FOREGROUND_BRIGHT_BLACK		"\x1B[90m"
/// @brief Sets the console font color to bright red
#define CONSOLE_FOREGROUND_BRIGHT_RED		"\x1B[91m"
/// @brief Sets the console font color to bright green
#define CONSOLE_FOREGROUND_BRIGHT_GREEN		"\x1B[92m"
/// @brief Sets the console font color to bright yellow
#define CONSOLE_FOREGROUND_BRIGHT_YELLOW	"\x1B[93m"
/// @brief Sets the console font color to bright blue
#define CONSOLE_FOREGROUND_BRIGHT_BLUE		"\x1B[94m"
/// @brief Sets the console font color to bright magenta
#define CONSOLE_FOREGROUND_BRIGHT_MAGENTA	"\x1B[95m"
/// @brief Sets the console font color to bright cyan
#define CONSOLE_FOREGROUND_BRIGHT_CYAN		"\x1B[96m"
/// @brief Sets the console font color to bright white
#define CONSOLE_FOREGROUND_BRIGHT_WHITE		"\x1B[97m"

/******************************************************
BACKGROUND COLORS FOR CONSOLE
******************************************************/

/// @brief Sets the console background color to black
#define CONSOLE_BACKGROUND_BLACK			"\x1B[40m"
/// @brief Sets the console background color to red
#define CONSOLE_BACKGROUND_RED				"\x1B[41m"
/// @brief Sets the console background color to green
#define CONSOLE_BACKGROUND_GREEN			"\x1B[42m"
/// @brief Sets the console background color to yellow
#define CONSOLE_BACKGROUND_YELLOW			"\x1B[43m"
/// @brief Sets the console background color to blue
#define CONSOLE_BACKGROUND_BLUE				"\x1B[44m"
/// @brief Sets the console background color to magenta
#define CONSOLE_BACKGROUND_MAGENTA			"\x1B[45m"
/// @brief Sets the console background color to cyan
#define CONSOLE_BACKGROUND_CYAN				"\x1B[46m"
/// @brief Sets the console background color to white
#define CONSOLE_BACKGROUND_WHITE			"\x1B[47m"
/// @brief Sets the console background color to bright black
#define CONSOLE_BACKGROUND_BRIGHT_BLACK		"\x1B[100m"
/// @brief Sets the console background color to bright red
#define CONSOLE_BACKGROUND_BRIGHT_RED		"\x1B[101m"
/// @brief Sets the console background color to bright green
#define CONSOLE_BACKGROUND_BRIGHT_GREEN		"\x1B[102m"
/// @brief Sets the console background color to bright yellow
#define CONSOLE_BACKGROUND_BRIGHT_YELLOW	"\x1B[103m"
/// @brief Sets the console background color to bright blue
#define CONSOLE_BACKGROUND_BRIGHT_BLUE		"\x1B[104m"
/// @brief Sets the console background color to bright magenta
#define CONSOLE_BACKGROUND_BRIGHT_MAGENTA	"\x1B[105m"
/// @brief Sets the console background color to bright cyan
#define CONSOLE_BACKGROUND_BRIGHT_CYAN		"\x1B[106m"
/// @brief Sets the console background color to bright white
#define CONSOLE_BACKGROUND_BRIGHT_WHITE		"\x1B[107m"

/******************************************************
OTHER MODIFIERS FOR CONSOLE
******************************************************/

/// @brief Resets the font and background color to its default
#define CONSOLE_COLOR_RESET					"\x1B[0m"
/// @brief Reverses the font and background color
#define CONSOLE_COLOR_REVERSE				"\x1B[7m"
/// @brief Sets the font to bold
#define CONSOLE_FONT_BOLD					"\x1B[2m"
/// @brief Sets the font to underline
#define CONSOLE_FONT_UNDERLINE				"\x1B[4m"

// 3 italic on Windows
// 5 animates on Windows
// 8 inverts color?
// 9 strike-through
#else

//Define the macros that are replaced by nothing
#define CONSOLE_FOREGROUND_BLACK
#define CONSOLE_FOREGROUND_RED
#define CONSOLE_FOREGROUND_GREEN
#define CONSOLE_FOREGROUND_YELLOW
#define CONSOLE_FOREGROUND_BLUE
#define CONSOLE_FOREGROUND_MAGENTA
#define CONSOLE_FOREGROUND_CYAN
#define CONSOLE_FOREGROUND_WHITE
#define CONSOLE_FOREGROUND_BRIGHT_BLACK
#define CONSOLE_FOREGROUND_BRIGHT_RED
#define CONSOLE_FOREGROUND_BRIGHT_GREEN
#define CONSOLE_FOREGROUND_BRIGHT_YELLOW
#define CONSOLE_FOREGROUND_BRIGHT_BLUE
#define CONSOLE_FOREGROUND_BRIGHT_MAGENTA
#define CONSOLE_FOREGROUND_BRIGHT_CYAN
#define CONSOLE_FOREGROUND_BRIGHT_WHITE
#define CONSOLE_BACKGROUND_BLACK
#define CONSOLE_BACKGROUND_RED
#define CONSOLE_BACKGROUND_GREEN
#define CONSOLE_BACKGROUND_YELLOW
#define CONSOLE_BACKGROUND_BLUE
#define CONSOLE_BACKGROUND_MAGENTA
#define CONSOLE_BACKGROUND_CYAN
#define CONSOLE_BACKGROUND_WHITE
#define CONSOLE_BACKGROUND_BRIGHT_BLACK
#define CONSOLE_BACKGROUND_BRIGHT_RED
#define CONSOLE_BACKGROUND_BRIGHT_GREEN
#define CONSOLE_BACKGROUND_BRIGHT_YELLOW
#define CONSOLE_BACKGROUND_BRIGHT_BLUE
#define CONSOLE_BACKGROUND_BRIGHT_MAGENTA
#define CONSOLE_BACKGROUND_BRIGHT_CYAN
#define CONSOLE_BACKGROUND_BRIGHT_WHITE
#define CONSOLE_COLOR_RESET
#define CONSOLE_COLOR_REVERSE
#define CONSOLE_FONT_BOLD
#define CONSOLE_FONT_UNDERLINE

#endif

#endif //HG_COLTI_CONSOLE_COLORS