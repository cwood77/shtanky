#include "instr.hpp"
#include "scriptWriter.hpp"

void scriptWriter::populateReservedLabels(scriptState& s)
{
   s.reserveLabel("end");
   s.reserveLabel("run");
   s.reserveLabel("bless");
   s.reserveLabel("unbless");
}

void scriptWriter::run(script& s, std::ostream& out, bool skipChecks)
{
   out << "@echo off" << std::endl;
   out << std::endl;

   out
      << "if %1 == run goto %1" << std::endl
      << "if %1 == bless goto %1" << std::endl
      << "if %1 == unbless goto %1" << std::endl
      << "if %1 == clean goto %1" << std::endl
      << "echo unknown command" << std::endl
      << "goto end" << std::endl
   ;
   out << std::endl;

   out << ":run" << std::endl;
   s.get(kStreamClean).playback(out);
   if(!skipChecks && s.getTotalProgressSize())
   {
      std::string label = "[ -- testing -- ";
      if(s.getTotalProgressSize() >= (label.length()+1))
      {
         label += std::string(s.getTotalProgressSize()-label.length()-1,' ');
         out << "echo " << label << "]" << std::endl;
      }
      else
         out << "echo -- testing --" << std::endl;
   }
   s.get(kStreamCmd).playback(out);
   if(!skipChecks)
   {
      s.get(kStreamCheck).stopProgressDisplayForOutput();
      s.get(kStreamCheck).playback(out);
   }
   if(s.hadSkips())
      out << "echo passed, but with some tests SKIPPED" << std::endl;
   else
      out << "echo pass" << std::endl;
   out << "goto end" << std::endl << std::endl;

   out << ":bless" << std::endl;
   s.get(kStreamBless).playback(out);
   out << "goto end" << std::endl << std::endl;

   out << ":unbless" << std::endl;
   s.get(kStreamUnbless).playback(out);
   out << "goto end" << std::endl << std::endl;

   out << ":clean" << std::endl;
   s.get(kStreamClean).playback(out);
   out << "goto end" << std::endl << std::endl;

   out << ":end" << std::endl;
}
