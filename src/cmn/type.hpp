#pragma once

// example needs
// - offsets in field access codegen
//   - measure (field+size) struct decls in lh files
//   - lookup (nested fields)
//   - varRef nodes can create
//     - really, it's the decl (e.g. arg) that creates
//       (or maybe the type?)
//   - fieldAccessNodes can publish parts of a type

namespace cmn {
namespace type {

class iType {
public:
   const size_t getSize() = 0;
};

class iStructType {
public:
   const iType& getField(const std::string& name) const = 0;
};

class table {
public:
};

class typeBuilder {
public:
   static typeBuilder *createString();
   static typeBuilder *createVoid();
   static typeBuilder *createClass(const std::string& name);
   static typeBuilder *createPtr();
   typeBuilder& array();
   typeBuilder& addMember(const std::string& name, typeBuilder& ty);
   iType& finish();
};

class nodeCache {
};

} // namespace type
} // namespace cmn
