#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os, ctypes, platform
import cgi

console_encoding = sys.getfilesystemencoding()

class print_style:
    version = '1.0.1.0'
    engine = None

    FC_BLACK   = 0
    FC_BLUE    = 1
    FC_GREEN   = 2
    FC_CYAN    = 3
    FC_RED     = 4
    FC_MAGENTA = 5
    FC_YELLOW  = 6
    FC_WHITE   = 7

    BC_BLACK   = 8
    BC_BLUE    = 9
    BC_GREEN   = 10
    BC_CYAN    = 11
    BC_RED     = 12
    BC_MAGENTA = 13
    BC_YELLOW  = 14
    BC_WHITE   = 15

    FW_BOLD    = 16

    def __contains__(self, value):
        return False

''''' See https://msdn.microsoft.com/zh-cn/windows/apps/ms682088%28v=vs.100%29#_win32_character_attributes
        for color codes
'''

class Win32ConsoleColor:
    name = 'windows console'
    STD_INPUT_HANDLE        = -10
    STD_OUTPUT_HANDLE       = -11
    STD_ERROR_HANDLE        = -12

    FOREGROUND_BLACK        = 0x0
    FOREGROUND_BLUE         = 0x01 # text color contains blue.
    FOREGROUND_GREEN        = 0x02 # text color contains green.
    FOREGROUND_RED          = 0x04 # text color contains red.
    FOREGROUND_INTENSITY    = 0x08 # text color is intensified.

    BACKGROUND_BLUE         = 0x10 # background color contains blue.
    BACKGROUND_GREEN        = 0x20 # background color contains green.
    BACKGROUND_RED          = 0x40 # background color contains red.
    BACKGROUND_INTENSITY    = 0x80 # background color is intensified.

    COLOR_MAP = {
        print_style.FC_BLACK: FOREGROUND_BLACK,
        print_style.FC_BLUE: FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        print_style.FC_GREEN: FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        print_style.FC_CYAN: FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        print_style.FC_RED: FOREGROUND_RED | FOREGROUND_INTENSITY,
        print_style.FC_MAGENTA: FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        print_style.FC_YELLOW: FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        print_style.FC_WHITE: FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,

        print_style.BC_BLACK: FOREGROUND_BLACK,
        print_style.BC_BLUE: BACKGROUND_BLUE,
        print_style.BC_GREEN: BACKGROUND_GREEN,
        print_style.BC_CYAN: BACKGROUND_BLUE | BACKGROUND_GREEN,
        print_style.BC_RED: BACKGROUND_RED,
        print_style.BC_MAGENTA: BACKGROUND_RED | BACKGROUND_BLUE,
        print_style.BC_YELLOW: BACKGROUND_RED | BACKGROUND_GREEN,
        print_style.BC_WHITE: BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,

        print_style.FW_BOLD: BACKGROUND_INTENSITY
    }

    std_out_handle = None
    std_err_handle = None

    def get_cmd_color(self, handle=std_out_handle):
        return Win32ConsoleColor.FOREGROUND_RED | Win32ConsoleColor.FOREGROUND_GREEN | Win32ConsoleColor.FOREGROUND_BLUE

    def set_cmd_color(self, color, handle=std_out_handle):
        """(color) -> bit
        Example: set_cmd_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
        """
        bool = ctypes.windll.kernel32.SetConsoleTextAttribute(handle, color)
        return bool

    def stdout_with_color(self, options, text):
        style = Win32ConsoleColor.FOREGROUND_BLACK
        for opt in options:
            style = style | Win32ConsoleColor.COLOR_MAP[opt]
        if style == Win32ConsoleColor.FOREGROUND_BLACK:
            sys.stdout.write(text)
        else:
            old_style = self.get_cmd_color()
            self.set_cmd_color(style, self.std_out_handle)
            sys.stdout.write(text)
            self.set_cmd_color(old_style, self.std_out_handle)

    def stderr_with_color(self, options, text):
        style = Win32ConsoleColor.FOREGROUND_BLACK
        for opt in options:
            style = style | Win32ConsoleColor.COLOR_MAP[opt]
        if style == Win32ConsoleColor.FOREGROUND_BLACK:
            sys.stderr.write(text)
        else:
            old_style = self.get_cmd_color()
            self.set_cmd_color(style, self.std_err_handle)
            sys.stderr.write(text)
            self.set_cmd_color(old_style, self.std_err_handle)

class TermColor:
    name = 'terminal'
    COLOR_MAP = {
        print_style.FC_BLACK:   '30',
        print_style.FC_BLUE:    '34',
        print_style.FC_GREEN:   '32',
        print_style.FC_CYAN:    '36',
        print_style.FC_RED:     '31',
        print_style.FC_MAGENTA: '35',
        print_style.FC_YELLOW:  '33',
        print_style.FC_WHITE:   '37',

        print_style.BC_BLACK:   '40',
        print_style.BC_BLUE:    '44',
        print_style.BC_GREEN:   '42',
        print_style.BC_CYAN:    '46',
        print_style.BC_RED:     '41',
        print_style.BC_MAGENTA: '45',
        print_style.BC_YELLOW:  '43',
        print_style.BC_WHITE:   '47',

        print_style.FW_BOLD: '1'
    }

    def stdout_with_color(self, options, text):
        style = []
        for opt in options:
            style.append(TermColor.COLOR_MAP[opt])

        if len(style) > 0:
            sys.stdout.write('\033[' + ';'.join(style) + 'm' + text + '\033[0m')
        else:
            sys.stdout.write(text)

    def stderr_with_color(self, options, text):
        style = []
        for opt in options:
            style.append(TermColor.COLOR_MAP[opt])

        if len(style) > 0:
            sys.stderr.write('\033[' + ';'.join(style) + 'm' + text + '\033[0m')
        else:
            sys.stderr.write(text)

class HtmlColor:
    name = 'html css'
    COLOR_MAP = {
        print_style.FC_BLACK:   'color: Black;',
        print_style.FC_BLUE:    'color: Blue;',
        print_style.FC_GREEN:   'color: Green;',
        print_style.FC_CYAN:    'color: Cyan;',
        print_style.FC_RED:     'color: Red;',
        print_style.FC_MAGENTA: 'color: Magenta;',
        print_style.FC_YELLOW:  'color: Yellow;',
        print_style.FC_WHITE:   'color: White;',

        print_style.BC_BLACK:   'background-color: Black;',
        print_style.BC_BLUE:    'background-color: Blue;',
        print_style.BC_GREEN:   'background-color: Green;',
        print_style.BC_CYAN:    'background-color: Cyan;',
        print_style.BC_RED:     'background-color: Red;',
        print_style.BC_MAGENTA: 'background-color: Magenta;',
        print_style.BC_YELLOW:  'background-color: Yellow;',
        print_style.BC_WHITE:   'background-color: White;',

        print_style.FW_BOLD: 'font-weight: bold;'
    }

    def stdout_with_color(self, options, text):
        style = []
        for opt in options:
            style.append(HtmlColor.COLOR_MAP[opt])

        if len(style) > 0:
            sys.stdout.write('<span style="' + ' '.join(style) + '">' + cgi.escape(text) + '</span>')
        else:
            sys.stdout.write(cgi.escape(text))

    def stderr_with_color(self, options, text):
        style = []
        for opt in options:
            style.append(HtmlColor.COLOR_MAP[opt])

        if len(style) > 0:
            sys.stderr.write('<span style="' + ' '.join(style) + '">' + cgi.escape(text) + '</span>')
        else:
            sys.stderr.write(cgi.escape(text))

class NoneColor:
    name = 'none'
    def stdout_with_color(self, options, text):
        sys.stdout.write(text)

    def stderr_with_color(self, options, text):
        sys.stderr.write(text)


def cprintf_set_mode(mode_name='auto'):
    mode_name = mode_name.lower()
    if not mode_name or mode_name == 'auto':
        # set by environment variable
        if not os.getenv('CPRINTF_MODE') is None:
            cprintf_set_mode(os.getenv('CPRINTF_MODE'))
        elif 'windows' == platform.system().lower():
            ostype_name = os.getenv('OSTYPE')
            if not ostype_name is None:
                ostype_name = ostype_name.lower()
            if 'msys' == ostype_name or 'cygwin' == ostype_name:
                cprintf_set_mode('term')
                return
            term_name = os.getenv('TERM')
            if not term_name is None:
                term_name = term_name.lower()
                if 'xterm' == term_name[0:5] or 'vt' == term_name[0:2]:
                    cprintf_set_mode('term')
                    return
            cprintf_set_mode('win32_console')
        elif os.getenv('ANSI_COLORS_DISABLED') is None:
            cprintf_set_mode('term')
        else:
            cprintf_set_mode('none')

    elif mode_name == 'none':
        print_style.engine = NoneColor

    elif mode_name == 'term':
        print_style.engine = TermColor

    elif mode_name == 'win32_console':
        ''''' See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/windows_api_reference.asp
        for information on Windows APIs.'''
        Win32ConsoleColor.std_out_handle = ctypes.windll.kernel32.GetStdHandle(Win32ConsoleColor.STD_OUTPUT_HANDLE)
        Win32ConsoleColor.std_err_handle = ctypes.windll.kernel32.GetStdHandle(Win32ConsoleColor.STD_ERROR_HANDLE)

        print_style.engine = Win32ConsoleColor

    elif mode_name == 'html':
        print_style.engine = HtmlColor

    else:
        print_style.engine = NoneColor

def cprintf_unpack_text(fmt, text):
    if len(text) > 0:
        try:
            ret = fmt.format(*text)
            return ret
        except Exception:
            ret = fmt.decode('utf-8').encode(console_encoding).format(*text)
            return ret
    else:
        return fmt

def cprintf_stdout(options, fmt, *text):
    cp = print_style.engine()
    cp.stdout_with_color(options, cprintf_unpack_text(fmt, text))
    sys.stdout.flush()

def cprintf_stderr(options, fmt, *text):
    cp = print_style.engine()
    cp.stderr_with_color(options, cprintf_unpack_text(fmt, text))
    sys.stderr.flush()

cprintf_set_mode('auto')

""" run as a executable """
if __name__ == "__main__":
    from optparse import OptionParser
    usage = "usage: %prog [options...] <format message> [format parameters...]"
    parser = OptionParser(usage)
    parser.disable_interspersed_args()

    parser.add_option("-v", "--version", action="store_true", help="show version and exit", dest="version")
    parser.add_option("-c", "--color", action="append", help="set font color.(any of: black, blue, green, cyan, red, magenta, yellow, white)", metavar="<color>", dest="color")
    parser.add_option("-b", "--background-color", action="append", help="set background color.(any of: black, blue, green, cyan, red, magenta, yellow, white)", metavar="<background color>", dest="background_color")
    parser.add_option("-B", "--bold", action="append_const", help="set font weight to bold", const=print_style.FW_BOLD, dest="style")
    parser.add_option("-m", "--mode", action="store", help="set mode.(any of: auto, term, win32_console, none, html)", metavar="<output mode>", dest="mode")
    parser.add_option("-s", "--output-stream", action="store", help="set output stream.(any of: stdout, stderr)", metavar="<ostream>", dest="ostream", default="stdout")
    parser.add_option("-e", action="store_true", help="enable interpretation of backslash escapes(just like echo command in unix like system)", dest="interp_bse", default=False)
    parser.add_option("-E", action="store_false", help="disable interpretation of backslash escapes(just like echo command in unix like system)", dest="interp_bse")

    (options, left_args) = parser.parse_args()

    print_stream = 'stdout'
    print_options = []

    fc_list = ['FC_' + x.upper() for x in options.color or [] ]
    bk_list = ['BC_' + y.upper() for y in options.background_color or [] ]
    for style_list in [ fc_list, bk_list ]:
        for style_name in style_list:
            if style_name in print_style.__dict__ :
                print_options.append(print_style.__dict__[style_name])

    for style_code in options.style or []:
        print_options.append(style_code)

    if options.mode:
        cprintf_set_mode(options.mode)

    if options.version:
        print(print_style.version)
        print('Color Engine: ' + print_style.engine.name)
        exit(0)
        
    if len(left_args) > 0:
        if options.interp_bse:
            for i in range(0, len(left_args)):
                left_args[i] = eval(repr(left_args[i]).replace("\\\\", "\\"))
        if 'stdout' == options.ostream:
            cprintf_stdout(print_options, *left_args)
        else:
            cprintf_stderr(print_options, *left_args)
