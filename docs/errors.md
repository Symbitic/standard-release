# Error Handling

To ease error reporting and detecting, all classes in Standard Release follow
a common format for handling errors. When used as an API, this approach allows
programmers to easily detect a fault. When used as a tool, it allows for
more accurate error reporting.

## Overview

The [Error](@ref Error) class forms the basis for all error handling in Standard
Release. All classes must include a public `error()` method that returns the
most recent error. If no error occurred during the last operation then the value
of `Error::Success` will be used.

All virtual classes should also include a private `setError` method, so that any
child classes can set the error state without needing to modify private
variables directly.

Standard Release should never throw exceptions. If an exception is thrown,
that indicates an internal error, and should be reported as a bug. We use
a dedicated error class instead of exceptions because exceptions interrupt
the control flow, and because they are very easy to misuse (i.e. simply
enclosing your main() function in a `try`/`catch` block).

