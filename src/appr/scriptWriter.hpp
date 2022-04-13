#pragma once
#include <ostream>

class script;
class scriptState;

class scriptWriter {
public:
   static void populateReservedLabels(scriptState& s);
   static void run(script& s, std::ostream& out);
};
