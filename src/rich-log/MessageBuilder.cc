#include "MessageBuilder.hh"

#include <clean-core/assert.hh>

#include <rich-log/logger.hh>

using namespace rlog;

void MessageBuilder::append_formatted(cc::string_view fmt, cc::span<cc::string const> args)
{
    size_t arg_i = 0;
    for (size_t i = 0; i < fmt.size(); ++i)
    {
        if (i + 1 < fmt.size() && fmt[i] == '{' && fmt[i + 1] == '}')
        {
            CC_ASSERT(arg_i < args.size() && "more escape sequences than provided arguments");
            _msg += args[arg_i];
            ++arg_i;
            ++i;
        }
        else
        {
            _msg += fmt[i];
        }
    }
    CC_ASSERT(arg_i == args.size() && "less escape sequences than provided arguments");
}

MessageBuilder::~MessageBuilder()
{
    // TODO: send message to logger
    // DEBUG: console for now
    auto* const stream = _use_err_stream ? stderr : stdout;
    print_prefix_to_stream(_severity, _domain, stream);
    std::fprintf(stream, "%s\n", _msg.c_str());
    std::fflush(stream);
}
