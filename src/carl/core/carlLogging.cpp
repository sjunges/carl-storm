/**
 * @file carlLogging.cpp
 */

#include "carlLogging.h"

namespace carl {
namespace logging {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

}  // namespace logging
}  // namespace carl
