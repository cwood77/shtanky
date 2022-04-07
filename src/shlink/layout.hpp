#pragma once
#include <set>
#include <string>
#include <vector>

namespace cmn { namespace objfmt { class obj; } }

namespace shlink {

// during the layout operation, keeps track of what is left and what has already been done
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

   unsigned long append(cmn::objfmt::obj& o);

private:
   std::vector<unsigned char> m_bytes;
   size_t m_size;
};

#if 0
class layout {
public:
   void place(cmn::objfmt::obj& o);

   void markDonePlacing();

   void link();

private:
   void link(cmn::objfmt::obj& o);

   std::map<cmn::objfmt::obj*,unsigned long> m_objPlacements;
   std::map<unsigned long,segmentBucket> m_segments;
};
#endif

} // namespace shlink
