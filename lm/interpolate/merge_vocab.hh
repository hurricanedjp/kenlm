#ifndef LM_INTERPOLATE_MERGE_VOCAB_H
#define LM_INTERPOLATE_MERGE_VOCAB_H

#include "lm/word_index.hh"
#include "lm/interpolate/universal_vocab.hh"
#include "util/file_piece.hh"

#include <queue>
#include <stdint.h>
#include <vector>

namespace lm {
namespace interpolate {

struct ModelInfo
{
  int fd;
  size_t vocab_size;
};

class VocabFileReader
{
public:
  explicit VocabFileReader(const int fd, size_t model_num, uint64_t offset =0);

  VocabFileReader &operator++() { read(); return *this; }
  operator bool() const { return !eof_; }
  uint64_t operator*() const { return Value(); }

  // reads a new entry from the file
  bool read(void);

  uint64_t Value(void) const { return hash_value_; }
  size_t ModelNum(void) const { return model_num_; }
  WordIndex CurrentIndex(void) const { return current_index_; }

private:
  uint64_t hash_value_;
  WordIndex current_index_;
  bool eof_;
  size_t model_num_;
  util::FilePiece file_piece_;
};


class CompareFiles
{
public:
  bool operator()(const VocabFileReader* x,
                  const VocabFileReader* y)
  { return x->Value()> y->Value(); }
};

class MergeVocabIndex
{
private:
  typedef std::priority_queue<VocabFileReader*, std::vector<VocabFileReader*>,
                              CompareFiles> HeapType;
      
public:
  explicit MergeVocabIndex(std::vector<ModelInfo>& vocab_file_info,
                           UniversalVocab& universam_vocab);
  void MergeModels(void);

private:
  UniversalVocab& universal_vocab_;
  HeapType    file_heap_;
};

}} // namespaces
      
#endif // LM_INTERPOLATE_MERGE_VOCAB_H
