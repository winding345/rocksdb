#include "skiplist/libpmemobj++/make_persistent.hpp"
#include "skiplist/libpmemobj++/make_persistent_array.hpp"
#include "skiplist/libpmemobj++/p.hpp"
#include "skiplist/libpmemobj++/persistent_ptr.hpp"
#include "skiplist/libpmemobj++/pool.hpp"
#include "skiplist/libpmemobj++/transaction.hpp"

#include "fixed_range_chunk_based_nvm_write_cache.h"

#include <ex_common.h>

namespace rocksdb {

    using p_range::pmem_hash_map;
    using p_range::p_node;

    FixedRangeChunkBasedNVMWriteCache::FixedRangeChunkBasedNVMWriteCache(const string &file, uint64_t pmem_size) {
        //bool justCreated = false;
        if (file_exists(file.c_str()) != 0) {
            pop_ = pmem::obj::pool<PersistentInfo>::create(file.c_str(), "FixedRangeChunkBasedNVMWriteCache", pmem_size,
                                                           CREATE_MODE_RW);
            //justCreated = true;

        } else {
            pop_ = pmem::obj::pool<PersistentInfo>::open(file.c_str(), "FixedRangeChunkBasedNVMWriteCache");
        }

        pinfo_ = pop_.root();
        if (!pinfo_->inited_) {
            transaction::run(pop_, [&] {
                // TODO 配置
                pinfo_->range_map = make_persistent<p_range::pmem_hash_map>();
                pinfo_->range_map_->tabLen = 0;
                pinfo_->range_map_->tab = make_persistent<p_node[]>(p_map->tabLen);
                pinfo_->range_map_->loadFactor = 0.75f;
                pinfo_->range_map_->threshold = p_map->tabLen * p_map->loadFactor;
                pinfo_->range_map_->size = 0;

                pinfo_->allocator_ = make_persistent<PersistentAllocator>();

                pinfo_->inited_ = true;
            });
        }



//  if (file_exists(file_path) != 0) {
//    if ((pop = pmemobj_create(file_path, POBJ_LAYOUT_NAME(range_mem),
//                              PMEMOBJ_MIN_POOL, 0666)) == NULL) {
//      perror("failed to create pool\n");
//      return 1;
//    }
//  } else {
//    if ((pop = pmemobj_open(file_path,
//                            POBJ_LAYOUT_NAME(range_mem))) == NULL) {
//      perror("failed to open pool\n");
//      return 1;
//    }
//  }
    }

    FixedRangeChunkBasedNVMWriteCache::~FixedRangeChunkBasedNVMWriteCache() {
        if (pop)
            pop.close();
//    pmemobj_close(pop);
    }

    Status FixedRangeChunkBasedNVMWriteCache::Get(const InternalKeyComparator &internal_comparator, const Slice &key,
                                                  std::string *value) {
        std::string prefix = (*vinfo_->internal_options_->prefix_extractor_)(key.data(), key.size());
        auto found_tab = vinfo_->prefix2range.find(prefix);
        if(found_tab == vinfo_->prefix2range.end()){
            // not found
            return Status::NotFound("no this range");
        }else{
            // found
            FixedRangeTab* tab = found_tab->second;
            return tab->Get(internal_comparator, key, value);
        }
    }

    void FixedRangeChunkBasedNVMWriteCache::addCompactionRangeTab(FixedRangeTab *tab) {
        range_queue_.pu
    }

    uint64_t FixedRangeChunkBasedNVMWriteCache::NewRange(const std::string &prefix) {
        // ( const void * key, int len, unsigned int seed );
//  uint64_ _hash = CityHash64WithSeed(prefix, prefix.size(), 16);
        //persistent_ptr<p_range::pmem_hash_map> p_map = pop.root();
        size_t bufSize = 1 << 26; // 64 MB
        uint64_t _hash;
        _hash = vinfo_->range_map_->put(pop_, prefix, bufSize);
//  return _hash;

        FixedRangeTab *range = new FixedRangeTab(p_map);
        vinfo_->prefix2range.insert({prefix, range});
    }

    void FixedRangeChunkBasedNVMWriteCache::AppendToRange(rocksdb::FixedRangeTab *tab, const char *bloom_data,
                                                          const rocksdb::Slice &chunk_data,
                                                          const rocksdb::Slice &new_start,
                                                          const rocksdb::Slice &new_end) {
        tab->Append(pop_, bloom_data, chunk_data, new_start, new_end);
    }

} // namespace rocksdb

