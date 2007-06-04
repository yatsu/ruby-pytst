require 'mkmf'

$CPPFLAGS += " -Iinclude"

# We must use c++ as linker instead of cc.
CONFIG["CC"] = "c++" # Fedora Core 5
CONFIG["LDSHARED"] = CONFIG["LDSHARED"].sub(/^cc\s/, "c++ ") # Mac OX X 10.4 + Fink

create_makefile("pytst")
