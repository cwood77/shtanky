#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class patch; } }

namespace shlink {

class iObjectProvider;
class iSymbolIndex;

// during the layout operation, keeps track of what is left and what has already been done
// specifically, layoutProgress asks for only reachable objects (i.e. it prunes)
class layoutProgress {
public:
   void seedRequiredObject(const std::string& oName) { m_unplaced.insert(oName); }

   bool isDone() const { return m_unplaced.size() == 0; }
   std::string getUnplacedObjectName() const { return *m_unplaced.begin(); }

   void markObjectPlaced(cmn::objfmt::obj& o);

private:
   std::set<std::string> m_placed;
   std::set<std::string> m_unplaced;
};

// a flat list of all the objects in a given segment
class segmentBlock {
public:
   segmentBlock() : offset(0), m_size(0) {}

   unsigned long offset; // unset until all objects have been placed
   size_t getSize() const { return m_size; }

   unsigned long append(cmn::objfmt::obj& o);

   unsigned char *getHeadPtr() { return &m_bytes[0]; }
   const unsigned char *getHeadPtr() const { return &m_bytes[0]; }

private:
   std::vector<unsigned char> m_bytes;
   size_t m_size;
};

class layout {
public:
   // after all reachable objects are placed, segments are tallied and laid into contiguous
   // space
   void place(cmn::objfmt::obj& o);
   void markDonePlacing();
   void reportSymbols(const std::set<std::string>& demanded,
      iObjectProvider& d, iSymbolIndex& indx);

   // fixes-up all patches in the segments to resolve symbols
   void link(iObjectProvider& d);

   // for writing
   const std::map<unsigned long,segmentBlock>& getSegments() const { return m_segments; }

private:
   void link(iObjectProvider& d, cmn::objfmt::obj& refer);

   unsigned long totalOffsetOfObj(cmn::objfmt::obj& o);
   unsigned long totalOffsetOfRIP(cmn::objfmt::obj& o, cmn::objfmt::patch& p);
   long *laidOutSitePtr(cmn::objfmt::obj& o, cmn::objfmt::patch& p);

   void patchRipRelDWord(
      cmn::objfmt::obj& refer,
      cmn::objfmt::obj& refee,
      cmn::objfmt::patch& referPatch);

   std::map<cmn::objfmt::obj*,unsigned long> m_objPlacements;
   std::map<unsigned long,segmentBlock> m_segments;
};

} // namespace shlink
