#pragma once
#include <cstddef>
#include <map>
#include <sstream>
#include <string>

namespace cmn {

class textTable {
public:
   ~textTable();

   std::stringstream& operator()(size_t r, size_t c);

   void compileAndWrite(std::ostream& stream);

private:
   class compiledCells {
   public:
      compiledCells() : m_lastCol(0) {}

      void compile(const std::map<size_t,std::map<size_t,std::stringstream*> >& cells);

      const std::map<size_t,std::map<size_t,std::string> >& cells() const { return m_cells; }

      std::string indentForMissingCols(size_t firstCol) const;
      std::string leftJustify(size_t col, const std::string& text) const;

   private:
      std::map<size_t,std::map<size_t,std::string> > m_cells;
      std::map<size_t,size_t> m_colWidths;
      size_t m_lastCol;
   };

   class writer {
   public:
      writer(const compiledCells& c, std::ostream& stream)
      : m_cc(c), m_stream(stream), m_currRow(0) {}

      void writeLine(size_t row, const std::map<size_t,std::string>& cols);

   private:
      const compiledCells& m_cc;
      std::ostream& m_stream;
      size_t m_currRow;
   };

   std::map<size_t,std::map<size_t,std::stringstream*> > m_cells;
};

class textTableLineWriter {
public:
   explicit textTableLineWriter(textTable& t, size_t lineNum = 0)
   : m_tt(t), m_line(0) {}

   std::stringstream& operator[](size_t colNum) { return m_tt(m_line,colNum); }

   void advanceLine() { m_line++; }

private:
   textTable& m_tt;
   size_t m_line;
};

} // namespace cmn
