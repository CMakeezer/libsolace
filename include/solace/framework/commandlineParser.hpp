/*
*  Copyright 2016 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * libSolace: Command line parser
 *	@file		solace/framework/commandlineParser.hpp
 *	@author		$LastChangedBy: $
 *	@date		$LastChangedDate: $
 *	@brief		Command line parser
 *	ID:			$Id: $
 ******************************************************************************/
#pragma once
#ifndef SOLACE_FRAMEWORK_COMMANDLINEPARSER_HPP
#define SOLACE_FRAMEWORK_COMMANDLINEPARSER_HPP

#include "solace/types.hpp"
#include "solace/string.hpp"
#include "solace/result.hpp"
#include "solace/unit.hpp"
#include "solace/error.hpp"
#include "solace/array.hpp"
#include "solace/version.hpp"

#include "solace/utils.hpp"
#include "solace/delegate.hpp"


namespace Solace { namespace Framework {

/**
 * Comman line parser
 * This is a helper class to handle processing of command line arguments.
 *
 * @example
 * \code{.cpp}
 int main(int argc, argv) {

  CommandlineParser("My application",
            {
                // Custom handler example:
                CommandlineParser::printVersion("my_app", Version(1, 2, 3, "dev")),
                CommandlineParser::printHelp(),

                // Regular argument of integal type
                { 's', "size", "Buffer size", &settings.bufferSize },
                { 'u', "userName", "User name", &settings.userName }
            },
            {
                // Mandatory arguments support:
                { "Mandatory argument", &settings.param },
            })
            .parse(argc, argv)
            .then(...consume parsing results...
            .orElse(...handle errors...);
    ...

    \endcode


 * Note: Commandline parsing is meant ot be performed at the start of an application so that
 * memory allocation limits / strategy can be passed via options.
 * Thus it is essential that parser don't allocate any memory during parsing.
 *
 */
class CommandlineParser {
public:

    /**
     * Command line parsing context.
     * This object represents the state of current parsing.
     * It designed to be used by a callback function to get access to the parameters and parser object itself.
     * It also can be used to communicate back to the parser an interaption.
     */
    struct Context {
        /// Number of arguments passed to parse method.
        const uint argc;

        /// Individual command line arguments the parse method has been given.
        const char** argv;

        /// Name of the option / argument being parsed.
        const char* const name;

        /// Current option raw value.
        const char* const value;

        /// Reference to the instance of the parser that invokes the callback.
        const CommandlineParser& parser;

        bool isStopRequired;

        Context(uint inArgc, const char *inArgv[],
                const char* inName, const char* inValue, const CommandlineParser& self) :
            argc(inArgc),
            argv(inArgv),
            name(inName),
            value(inValue),
            parser(self),
            isStopRequired(false)
        {}

        void stopParsing() noexcept {
            isStopRequired = true;
        }

    };

    /**
     * Argument proccessing policy for custom callbacks
     */
    enum class OptionArgument : byte {
        Required,          //!< Argument is required. It is an error if the option is given without an argument.
        Optional,          //!< Argument is optional. It is not an error to have option with or without an agrument.
        NotRequired        //!< Argument is not expected. It is an error to give an option with an agrument.
    };


    /**
     * An optional argument / flag object used by command line parser.
     */
    class Option {
    public:
        Option(char shortName, const char* longName, const char* description, String* value);
        Option(char shortName, const char* longName, const char* description, int8* value);
        Option(char shortName, const char* longName, const char* description, uint8* value);
        Option(char shortName, const char* longName, const char* description, int16* value);
        Option(char shortName, const char* longName, const char* description, uint16* value);
        Option(char shortName, const char* longName, const char* description, int32* value);
        Option(char shortName, const char* longName, const char* description, uint32* value);
        Option(char shortName, const char* longName, const char* description, int64* value);
        Option(char shortName, const char* longName, const char* description, uint64* value);
        Option(char shortName, const char* longName, const char* description, float32* value);
        Option(char shortName, const char* longName, const char* description, float64* value);
        Option(char shortName, const char* longName, const char* description, bool* value);

        /// Common constructor:
        template<typename F,
                 typename R = typename std::result_of<F(Context&)>::type
                 >
        Option(char shortName, const char* longName, const char* description, OptionArgument expectsArgument,
               F&& f) :
            _longName(longName),
            _description(description),
            _shortName(shortName),
            _expectsArgument(expectsArgument),
            _callback(std::forward<F>(f))
        {}

        Option(const Option& rhs) noexcept :
            _longName(rhs._longName),
            _description(rhs._description),
            _shortName(rhs._shortName),
            _expectsArgument(rhs._expectsArgument),
            _callback(rhs._callback)
        {}

        Option(Option&& rhs) noexcept :
            _longName(std::move(rhs._longName)),
            _description(std::move(rhs._description)),
            _shortName(std::move(rhs._shortName)),
            _expectsArgument(rhs._expectsArgument),
            _callback(std::move(rhs._callback))
        {}

        Option& operator= (const Option& rhs) noexcept {
            Option(rhs).swap(*this);

            return *this;
        }

        Option& operator= (Option&& rhs) noexcept {
            return swap(rhs);
        }

        Option& swap(Option& rhs) noexcept {
            using std::swap;
            swap(_shortName, rhs._shortName);
            swap(_longName, rhs._longName);
            swap(_description, rhs._description);
            swap(_callback, rhs._callback);
            swap(_expectsArgument, rhs._expectsArgument);

            return (*this);
        }

        bool operator == (const Option& other) const noexcept;

        bool isMatch(const char* agr, char shortPrefix) const noexcept;

        Optional<Error> match(Context& c) const;

        char        shortName() const noexcept      { return _shortName; }
        const char* name() const noexcept           { return _longName; }
        const char* description() const noexcept    { return _description; }

        OptionArgument getArgumentExpectations() const noexcept { return _expectsArgument; }

    private:


        const char*                         _longName;
        const char*                         _description;
        char                                _shortName;

        OptionArgument                      _expectsArgument;
        std::function<Optional<Error> (Context&)>    _callback;
    };


    /** Mandatory argument
     * This class represent a mandtory argument to be expected by a parser.
     * It is a pasring error if no mandatory arguments is provided.
     */
    class Argument {
    public:
        Argument(const char* name, const char* description, String* value);
        Argument(const char* name, const char* description, int8* value);
        Argument(const char* name, const char* description, uint8* value);
        Argument(const char* name, const char* description, int16* value);
        Argument(const char* name, const char* description, uint16* value);
        Argument(const char* name, const char* description, int32* value);
        Argument(const char* name, const char* description, uint32* value);
        Argument(const char* name, const char* description, int64* value);
        Argument(const char* name, const char* description, uint64* value);
        Argument(const char* name, const char* description, float32* value);
        Argument(const char* name, const char* description, float64* value);
        Argument(const char* name, const char* description, bool* value);

        template<typename F>
        Argument(const char* name, const char* description,
                 F&& callback) :
            _name(name),
            _description(description),
            _callback(std::forward<F>(callback))
        {}

        Argument(const Argument& rhs) noexcept :
            _name(rhs._name),
            _description(rhs._description),
            _callback(rhs._callback)
        {}

        Argument(Argument&& rhs) noexcept :
            _name(std::move(rhs._name)),
            _description(std::move(rhs._description)),
            _callback(std::move(rhs._callback))
        {}

        Argument& operator= (const Argument& rhs) noexcept {
            Argument(rhs).swap(*this);

            return *this;
        }

        Argument& operator= (Argument&& rhs) noexcept {
            return swap(rhs);
        }

        Argument& swap(Argument& rhs) noexcept {
            std::swap(_name, rhs._name);
            std::swap(_description, rhs._description);
            std::swap(_callback, rhs._callback);

            return (*this);
        }

        bool operator == (const Argument& other) const noexcept;

        const char* name() const noexcept {
            return _name;
        }

        Optional<Error> match(Context& c) const;

    private:
        const char*                                 _name;
        const char*                                 _description;
        std::function<Optional<Error>(Context&)>    _callback;
    };


    /**
     * Command for CLI
     */
    class Command {
    public:

        ~Command() = default;

        Command(const Command& rhs) noexcept :
            _name(rhs._name),
            _description(rhs._description),
            _callback(rhs._callback),
            _options(rhs._options)
        {}

        Command(Command&& rhs) noexcept :
            _name(std::move(rhs._name)),
            _description(std::move(rhs._description)),
            _callback(std::move(rhs._callback)),
            _options(std::move(rhs._options))
        {}

        template<typename F>
        Command(const char* name, const char* description,
                 F&& callback) :
            _name(name),
            _description(description),
            _callback(std::forward<F>(callback)),
            _options()
        {}

        template<typename F>
        Command(const char* name, const char* description,
                 F&& callback,
                const std::initializer_list<Option>& options) :
            _name(name),
            _description(description),
            _callback(std::forward<F>(callback)),
            _options(options)
        {}


        Command& operator= (const Command& rhs) noexcept {
            Command(rhs).swap(*this);

            return *this;
        }

        Command& operator= (Command&& rhs) noexcept {
            return swap(rhs);
        }

        Command& swap(Command& rhs) noexcept {
            std::swap(_name, rhs._name);
            std::swap(_description, rhs._description);
            std::swap(_callback, rhs._callback);
            std::swap(_options, rhs._options);

            return (*this);
        }

//        bool operator== (const Option& other) const noexcept;

        bool isMatch(const char* arg) const noexcept;
        Optional<Error> match(Context& cntx) const;

        const char* name() const noexcept           { return _name; }
        const char* description() const noexcept    { return _description; }
        const Array<Option>&   options() const noexcept { return _options; }

    private:
        const char*                                 _name;
        const char*                                 _description;
        std::function<Optional<Error>(Context&)>    _callback;

        /// Commad line options / flags that the command accepts.
        Array<Option>   _options;
    };


public:

    //!< Default prefix for flags and options
    static const char DefaultPrefix;

public:

    ~CommandlineParser() = default;

    /**
     * Construct default command line parser that expects no arguments.
     * @param appDescription Human readable application description used by 'help'-type commands.
     * Note: it is a parsing error to provide any flag/option to a default constructed parser,
     * thus any flags encountered by a parser constructed via this particular constructor - will result in an error.
     * That includes '--help' and '--version' options (which makes this constructor of limited use).
     */
    CommandlineParser(const char* appDescription);

    CommandlineParser(const char* appDescription,
                      const std::initializer_list<Option>& options);

    CommandlineParser(const char* appDescription,
                      const std::initializer_list<Option>& options,
                      const std::initializer_list<Argument>& arguments);

//    CommandlineParser(const char* appDescription,
//                      const std::initializer_list<Option>& options,
//                      const std::initializer_list<Command>& commands);

    CommandlineParser(const CommandlineParser& rhs) :
        _prefix(rhs._prefix),
        _description(rhs._description),
        _options(rhs._options),
        _arguments(rhs._arguments),
        _commands(rhs._commands)
    {}

    CommandlineParser(CommandlineParser&& rhs) noexcept :
        _prefix(exchange(rhs._prefix, DefaultPrefix)),
        _description(exchange(rhs._description, nullptr)),
        _options(std::move(rhs._options)),
        _arguments(std::move(rhs._arguments)),
        _commands(std::move(rhs._commands))
    {}

    CommandlineParser& operator= (const CommandlineParser& rhs) noexcept {
        CommandlineParser(rhs).swap(*this);

        return *this;
    }

    CommandlineParser& operator= (CommandlineParser&& rhs) noexcept {
        return swap(rhs);
    }

    CommandlineParser& swap(CommandlineParser& rhs) noexcept {
        using std::swap;
        swap(_prefix, rhs._prefix);
        swap(_description, rhs._description);
        swap(_options, rhs._options);
        swap(_arguments, rhs._arguments);
        swap(_commands, rhs._commands);

        return (*this);
    }


    /**
     * Parse command line arguments and process all the flags.
     * @param argc Number of command line arguments including name of the program at argv[0]
     * @param argv An array of string that represent command line argument tokens.
     * @return Result of parsing: Either a pointer to the parser or an error.
     */
    Result<const CommandlineParser*, Error>
    parse(int argc, const char* argv[]) const;


    /**
     * Add an option to print application version.
     * @param appName Name of the application to print along with version.
     * @param appVersion Application version to be printed.
     * @return A parser option that when given by a user will result in a printing of the version info.
     */
    static Option printVersion(const char* appName, const Version& appVersion);

    /**
     * Add an option to print application help summary.
     * @return A parser option that when given by a user will result in a printing of
     * short options summary.
     */
    static Option printHelp();

    /**
     * Get prefix used to identify flags and options.
     * @return prefix for flags and options.
     */
    char optionPrefix() const noexcept { return _prefix; }

    /**
     * Set prefix used to identify flags and options.
     * @param prefixChar A new value for the prefix character.
     * @return Reference to this for fluent interface.
     */
    CommandlineParser& optionPrefix(char prefixChar) noexcept {
        _prefix = prefixChar;
        return *this;
    }

    /**
     * Get human readable description of the application, dispayed by help and version commands.
     * @return Human readable application description string.
     */
    const char* description() const noexcept { return _description; }

    /**
     * Set human-readable application description, to be dispayed by 'help' and 'version' commands.
     * @param desc New value for human-readable application description string.
     * @return Reference to this for fluent interface.
     */
    CommandlineParser& description(const char* desc) noexcept {
        _description = desc;

        return *this;
    }

    const Array<Option>& options() const noexcept       { return _options; }
    CommandlineParser& options(Array<Option>&& options) {
        _options = std::move(options);

        return *this;
    }

//    const Array<Argument>& arguments() const noexcept   { return _arguments; }
    const Array<Command>& commands() const noexcept     { return _commands; }
    CommandlineParser& commands(std::initializer_list<Command> commands) {
//        CommandlineParser& commands(const Array<Command>& commands) {
//        _commands = std::move(commands);
        _commands = commands;

        return *this;
    }

public:

private:

    /// Option prefix
    char            _prefix;

    /// Human readable description of the application.
    const char*     _description;

    /// Commad line options / flags that application accepts.
    Array<Option>   _options;

    /// Mandatory arguments application requires.
    Array<Argument> _arguments;

    /// Mandatory commands application expects.
    Array<Command>  _commands;
};




inline void swap(CommandlineParser::Option& lhs, CommandlineParser::Option& rhs) noexcept {
    lhs.swap(rhs);
}

inline void swap(CommandlineParser::Argument& lhs, CommandlineParser::Argument& rhs) noexcept {
    lhs.swap(rhs);
}

inline void swap(CommandlineParser& lhs, CommandlineParser& rhs) noexcept {
    lhs.swap(rhs);
}

}  // End of namespace Framework
}  // End of namespace Solace
#endif  // SOLACE_FRAMEWORK_COMMANDLINEPARSER_HPP
