#pragma once

namespace cmn { class iObjWriter; }

namespace shlink {

class layout;
class segmentBlock;

class formatter {
public:
   explicit formatter(cmn::iObjWriter& w) : m_w(w) {}

   void write(const layout& l);

private:
   void write(unsigned long flags, const segmentBlock& s);

   cmn::iObjWriter& m_w;
};

} // namespace shlink
