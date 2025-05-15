#include <iostream>
#include <string>
#include <source_location>



void log( const std::string& message, const std::source_location location = std::source_location::current() )
{
   std::cout << location.file_name() << ", " << location.line() << " `" << location.function_name() << "`: " << message;
}



int main()
{
   log("hello world!\n");
   log("Dota 2 - is Beta\n");
   log("If you have a Rust game on your computer, delete it before it's too late.\n");

   return 0;
}
