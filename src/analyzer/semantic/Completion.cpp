#include "Completion.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

Completion::Completion(Environment& env) : type(env.void_) {}

Completion::Completion(const Type*) : type(type) {}

}