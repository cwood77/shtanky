#include "textTable.hpp"

namespace cmn {

textTable::~textTable()
{
   for(auto it=m_cells.begin();it!=m_cells.end();++it)
      for(auto jit=it->second.begin();jit!=it->second.end();++jit)
         delete jit->second;
}

std::stringstream& textTable::operator()(size_t r, size_t c)
{
   std::stringstream*& pStream = m_cells[r][c];
   if(!pStream)
      pStream = new std::stringstream();
   return *pStream;
}

void textTable::compileAndWrite(std::ostream& stream)
{
   compiledCells c;
   c.compile(m_cells);

   writer w(c,stream);
   for(auto it=c.cells().begin();it!=c.cells().end();++it)
      w.writeLine(it->first,it->second);
}

void textTable::compiledCells::compile(const std::map<size_t,std::map<size_t,std::stringstream*> >& cells)
{
   for(auto row=cells.begin();row!=cells.end();++row)
   {
      for(auto cell=row->second.begin();cell!=row->second.end();++cell)
      {
         auto s = cell->second->str();
         m_cells[row->first][cell->first] = s;

         auto width = s.length();
         auto& maxW = m_colWidths[cell->first];
         if(width > maxW)
            maxW = width + 1; // add one so columns have a 'border'

         if(cell->first > m_lastCol)
            m_lastCol = cell->first;
      }
   }
}

std::string textTable::compiledCells::leftJustify(size_t col, const std::string& text) const
{
   if(m_lastCol == col)
      return text;

   auto w = m_colWidths.find(col)->second;
   auto l = text.length();
   if(l < w)
   {
      return text + std::string(w-l,' ');
   }
   else
      return text;
}

void textTable::writer::writeLine(size_t row, const std::map<size_t,std::string>& cols)
{
   for(;m_currRow<row;m_currRow++)
      m_stream << std::endl;

   size_t last = (--(cols.end()))->first;
   for(size_t i=0;i<=last;i++)
   {
      auto it = cols.find(i);
      if(it != cols.end())
         m_stream << m_cc.leftJustify(i,it->second);
      else
         m_stream << m_cc.leftJustify(i,"");
   }
}

} // namespace cmn
