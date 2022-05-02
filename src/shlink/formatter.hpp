#pragma once

namespace cmn { class iObjWriter; }

namespace shlink {

class iSymbolIndex;
class layout;
class segmentBlock;

class appFormatter {
public:
   explicit appFormatter(cmn::iObjWriter& w) : m_w(w) {}

   void write(const layout& l, iSymbolIndex& x);

private:
   void write(unsigned long flags, const segmentBlock& s);

   cmn::iObjWriter& m_w;
};

} // namespace shlink
