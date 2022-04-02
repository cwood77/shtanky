#pragma once
#include <fstream>
#include <string>
#include <vector>

namespace shtasm {

class lexor {
public:
   explicit lexor(const std::string& file) : m_file(file.c_str()), m_line(1) {}

   bool isDone() const { return !m_file.good(); }
   std::string getNextLine();
   size_t getLineNumber() const { return m_line; }

private:
   std::ifstream m_file;
   unsigned long m_line;
};

class parser {
public:
   explicit parser(lexor& l) : m_l(l) {}

   const lexor& getLexor() const { return m_l; }
   void parseLine(std::string& label, std::vector<std::string>& words, std::string& comment);

private:
   void eatWhitespace(const char*& pThumb);
   std::string trimTrailingWhitespace(const std::string& w);
   bool shaveOffPart(const char*& pThumb, char delim, std::string& part);

   lexor& m_l;
};

} // namespace shtasm
