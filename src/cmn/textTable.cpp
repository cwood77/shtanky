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
   for(auto it=cells.begin();it!=cells.end();++it)
   {
      for(auto jit=it->second.begin();jit!=it->second.end();++jit)
      {
         auto s = jit->second->str();
         m_cells[it->first][jit->first] = s;

         auto width = s.length();
         auto& maxW = m_colWidths[jit->first];
         if(width > maxW)
            maxW = width + 1; // add one so columns have a 'border'

         if(jit->first > m_lastCol)
            m_lastCol = jit->first;
      }
   }
}

std::string textTable::compiledCells::indent(size_t firstCol) const
{
   std::stringstream indent;

   for(auto it=m_colWidths.begin();it!=m_colWidths.end()&&it->first<firstCol;++it)
      indent << std::string(it->second,' ');

   return indent.str();
}

std::string textTable::compiledCells::pad(size_t col, const std::string& text) const
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

   m_stream << m_cc.indent(cols.begin()->first);
   for(auto it=cols.begin();it!=cols.end();++it)
      m_stream << m_cc.pad(it->first,it->second);
}

} // namespace cmn
