// leveldb_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cassert>
#include <leveldb/db.h>
#include <leveldb/comparator.h>

class TwoPartComparator : public leveldb::Comparator
{
public:
    // Three-way comparison function:
    //   if a < b: negative result
    //   if a > b: positive result
    //   else: zero result
    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
        int a1, a2, b1, b2;
        ParseKey(a, &a1, &a2);
        ParseKey(b, &b1, &b2);
        if (a1 < b1) return -1;
        if (a1 > b1) return +1;
        if (a2 < b2) return -1;
        if (a2 > b2) return +1;
        return 0;
    }

    // Ignore the following methods for now:
    const char* Name() const { return "TwoPartComparator"; }
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
    void FindShortSuccessor(std::string*) const { }
};

int _tmain(int argc, _TCHAR* argv[])
{
    TwoPartComparator cmp;
    leveldb::DB* db;

    {
        leveldb::Options options;
        options.comparator = &cmp;
        options.create_if_missing = true;
        leveldb::Status status =
            leveldb::DB::Open(options, "/tmp/testdb", &db);
        assert(status.ok());
    }

    {
        leveldb::ReadOptions options;
        options.snapshot = db->GetSnapshot();
        // ... apply some updates to db ...
        leveldb::Iterator* iter = db->NewIterator(options);
        //... read using iter to view the state when the snapshot was created ...
        {
            for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
                std::cout << iter->key().ToString() << ": "  << iter->value().ToString() << std::endl;
            }
            assert(iter->status().ok());  // Check for any errors found during the scan
        }
        delete iter;
        db->ReleaseSnapshot(options.snapshot);
    }

    delete db;

    return 0;
}

