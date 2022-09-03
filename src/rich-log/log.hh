#pragma once

#include <rich-log/detail/api.hh>
#include <rich-log/detail/format.hh>
#include <rich-log/domain.hh>
#include <rich-log/location.hh>

/**
 *
 * general-purpose logging framework
 *
 * Quickstart:
 *
 *    // default logging has info verbosity and goes to Default domain
 *    // logging message is created using rlog::detail::format
 *    LOG("created %s vertices and {} faces", v_cnt, f_cnt);
 *
 *    // for quick debugging, use the following shortcut
 *    int x = 17;
 *    int y = 10;
 *    LOG_EXPR(x + y); // logs "x + y = 27"
 *
 *    // more sophisticated logging uses the long form with explicit domain and severity
 *    LOGD(MyDomain, Warning, "file '%s' is empty", filename);
 *
 *    // custom domains must be DECLARED before they can be used
 *    // this is very lightweight (APIs can declare domains in public headers just fine)
 *    // a domain is declared by:
 *    RICH_LOG_DECLARE_DOMAIN(MyDomain);
 *
 *    // custom domains can be nested:
 *    RICH_LOG_DECLARE_DOMAIN(MyDomain::SystemA::ComponentB);
 *
 *    // per default, all verbosities are enabled
 *    // a custom minimum compile-time verbosity level for your domain can be specified via
 *    RICH_LOG_DECLARE_DOMAIN_EX(MyDomain, Warning, extern);
 */

#define RICH_LOG_IMPL(Domain, Severity, CooldownSec, Formatter, ...)                                                                            \
    do                                                                                                                                          \
    {                                                                                                                                           \
        if constexpr (rlog::verbosity::Severity >= rlog::verbosity::type(Log::Domain::CompileTimeMinVerbosity))                                 \
        {                                                                                                                                       \
            if (rlog::verbosity::Severity >= Log::Domain::domain.min_verbosity)                                                                 \
            {                                                                                                                                   \
                static rlog::location _rlog_location = {CC_PRETTY_FUNC, __FILE__, __LINE__};                                                    \
                if (rlog::detail::do_log(Log::Domain::domain, rlog::verbosity::Severity, &_rlog_location, CooldownSec, Formatter(__VA_ARGS__))) \
                    CC_DEBUG_BREAK();                                                                                                           \
            }                                                                                                                                   \
        }                                                                                                                                       \
    } while (0) // force ;

/// writes an info log message to the Default domain using rlog::detail::format (printf AND pythonic syntax)
#define RICH_LOG(...) RICH_LOG_IMPL(Default, Info, 0, rlog::detail::format, __VA_ARGS__)
/// same as log but with Warning severity
#define RICH_LOG_WARN(...) RICH_LOG_IMPL(Default, Warning, 0, rlog::detail::format, __VA_ARGS__)
/// same as log but with Error severity
#define RICH_LOG_ERROR(...) RICH_LOG_IMPL(Default, Error, 0, rlog::detail::format, __VA_ARGS__)
/// writes a log message with given domain and severity using rlog::detail::format
#define RICH_LOGD(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, 0, rlog::detail::format, __VA_ARGS__)
/// same as RICH_LOGD but will only log once
#define RICH_LOGD_ONCE(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, -1, rlog::detail::format, __VA_ARGS__)
/// convenience wrapper for LOG("<expr> = %s", <expr>)
#define RICH_LOG_EXPR(...) RICH_LOG("%s = %s", #__VA_ARGS__, __VA_ARGS__)


#ifndef RICH_LOG_FORCE_MACRO_PREFIX

/// writes an info log message to the Default domain using rlog::detail::format (printf AND pythonic syntax)
#define LOG(...) RICH_LOG_IMPL(Default, Info, 0, rlog::detail::format, __VA_ARGS__)
/// same as log but with Warning severity
#define LOG_WARN(...) RICH_LOG_IMPL(Default, Warning, 0, rlog::detail::format, __VA_ARGS__)
/// same as log but with Error severity
#define LOG_ERROR(...) RICH_LOG_IMPL(Default, Error, 0, rlog::detail::format, __VA_ARGS__)
/// writes a log message with given domain and severity using rlog::detail::format
#define LOGD(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, 0, rlog::detail::format, __VA_ARGS__)
/// same as LOGD but will only log once
#define LOGD_ONCE(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, -1, rlog::detail::format, __VA_ARGS__)
/// convenience wrapper for LOG("<expr> = %s", <expr>)
#define LOG_EXPR(...) RICH_LOG("%s = %s", #__VA_ARGS__, __VA_ARGS__)

#endif

namespace rlog::detail
{
/// NOTE: loc is a pointer to the data segment (static lifetime)
/// TODO: we might be able to improve performance by providing a threadlocal stream_ref<char> to the formatter
/// returns true if we want to hit a breakpoint after logging
/// cooldown_sec limits log rate (0 means no limit, -1 means once, positive means minimum seconds before multiple logs)
RLOG_API bool do_log(rlog::domain_info const& domain, rlog::verbosity::type verbosity, location* loc, double cooldown_sec, cc::string_view message);
}
