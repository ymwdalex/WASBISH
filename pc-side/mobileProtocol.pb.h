// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: mobileProtocol.proto

#ifndef PROTOBUF_mobileProtocol_2eproto__INCLUDED
#define PROTOBUF_mobileProtocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace mobileProtocol {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_mobileProtocol_2eproto();
void protobuf_AssignDesc_mobileProtocol_2eproto();
void protobuf_ShutdownFile_mobileProtocol_2eproto();

class ucMsg;

enum CmdType {
  VIBRATION = 0
};
bool CmdType_IsValid(int value);
const CmdType CmdType_MIN = VIBRATION;
const CmdType CmdType_MAX = VIBRATION;
const int CmdType_ARRAYSIZE = CmdType_MAX + 1;

const ::google::protobuf::EnumDescriptor* CmdType_descriptor();
inline const ::std::string& CmdType_Name(CmdType value) {
  return ::google::protobuf::internal::NameOfEnum(
    CmdType_descriptor(), value);
}
inline bool CmdType_Parse(
    const ::std::string& name, CmdType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<CmdType>(
    CmdType_descriptor(), name, value);
}
// ===================================================================

class ucMsg : public ::google::protobuf::Message {
 public:
  ucMsg();
  virtual ~ucMsg();
  
  ucMsg(const ucMsg& from);
  
  inline ucMsg& operator=(const ucMsg& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const ucMsg& default_instance();
  
  void Swap(ucMsg* other);
  
  // implements Message ----------------------------------------------
  
  ucMsg* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ucMsg& from);
  void MergeFrom(const ucMsg& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required .mobileProtocol.CmdType type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline mobileProtocol::CmdType type() const;
  inline void set_type(mobileProtocol::CmdType value);
  
  // repeated int64 vPattern = 2;
  inline int vpattern_size() const;
  inline void clear_vpattern();
  static const int kVPatternFieldNumber = 2;
  inline ::google::protobuf::int64 vpattern(int index) const;
  inline void set_vpattern(int index, ::google::protobuf::int64 value);
  inline void add_vpattern(::google::protobuf::int64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      vpattern() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_vpattern();
  
  // @@protoc_insertion_point(class_scope:mobileProtocol.ucMsg)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  int type_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > vpattern_;
  friend void  protobuf_AddDesc_mobileProtocol_2eproto();
  friend void protobuf_AssignDesc_mobileProtocol_2eproto();
  friend void protobuf_ShutdownFile_mobileProtocol_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static ucMsg* default_instance_;
};
// ===================================================================


// ===================================================================

// ucMsg

// required .mobileProtocol.CmdType type = 1;
inline bool ucMsg::has_type() const {
  return _has_bit(0);
}
inline void ucMsg::clear_type() {
  type_ = 0;
  _clear_bit(0);
}
inline mobileProtocol::CmdType ucMsg::type() const {
  return static_cast< mobileProtocol::CmdType >(type_);
}
inline void ucMsg::set_type(mobileProtocol::CmdType value) {
  GOOGLE_DCHECK(mobileProtocol::CmdType_IsValid(value));
  _set_bit(0);
  type_ = value;
}

// repeated int64 vPattern = 2;
inline int ucMsg::vpattern_size() const {
  return vpattern_.size();
}
inline void ucMsg::clear_vpattern() {
  vpattern_.Clear();
}
inline ::google::protobuf::int64 ucMsg::vpattern(int index) const {
  return vpattern_.Get(index);
}
inline void ucMsg::set_vpattern(int index, ::google::protobuf::int64 value) {
  vpattern_.Set(index, value);
}
inline void ucMsg::add_vpattern(::google::protobuf::int64 value) {
  vpattern_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
ucMsg::vpattern() const {
  return vpattern_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
ucMsg::mutable_vpattern() {
  return &vpattern_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace mobileProtocol

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< mobileProtocol::CmdType>() {
  return mobileProtocol::CmdType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_mobileProtocol_2eproto__INCLUDED