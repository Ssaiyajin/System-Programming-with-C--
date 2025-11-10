#include "lib/MultiMap.hpp"
#include <cassert>
#include <new>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
MultiMap::Node::Node(value_type value)
    : value(move(value))
// Constructor
{
}
//---------------------------------------------------------------------------
MultiMap::Iterator::Iterator(Node** bucket, Node** end, Node* node)
    : bucket(bucket),
      end(end),
      node(node)
// Constructor
{
    advance();
}
//---------------------------------------------------------------------------
void MultiMap::Iterator::advance()
// Advance to the next entry
{
    while (!node && (bucket != end)) {
        ++bucket;
        node = (bucket == end) ? nullptr : *bucket;
    }
}
//---------------------------------------------------------------------------
MultiMap::Iterator::reference MultiMap::Iterator::operator*()
// Dereference
{
    assert(bucket != end);
    return node->value;
}
//---------------------------------------------------------------------------
MultiMap::Iterator::pointer MultiMap::Iterator::operator->()
// Pointer to member
{
    assert(bucket != end);
    return &node->value;
}
//---------------------------------------------------------------------------
MultiMap::Iterator& MultiMap::Iterator::operator++()
// Pre-increment
{
    assert(bucket != end);
    node = node->next;

    advance();

    return *this;
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::Iterator::operator++(int)
// Post-increment
{
    assert(bucket != end);

    Iterator it(*this);
    operator++();
    return it;
}
//---------------------------------------------------------------------------
bool MultiMap::Iterator::operator==(const Iterator& other) const
// Equality comparison
{
    return (bucket == other.bucket) && (end == other.end) && (node == other.node);
}
//---------------------------------------------------------------------------
MultiMap::MultiMap(uint32_t bucketCount)
    : hashTable(bucketCount)
// Constructor
{
}
//---------------------------------------------------------------------------
MultiMap::~MultiMap()
// Destructor
{
    for (auto* node : hashTable) {
        while (node) {
            auto* next = node->next;
            node->~Node();
            nodeAllocator.deallocate(node, 1);
            node = next;
        }
    }
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::insert(value_type value)
// Insert a value
{
    auto hash = value.first;
    auto slot = hash % hashTable.size();

    auto* node = new (nodeAllocator.allocate(1)) Node(move(value));

    node->next = hashTable[slot];
    hashTable[slot] = node;

    return {&hashTable[slot], &hashTable[hashTable.size()], node};
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::findFirst(uint32_t key)
// Find the first value with the specified key
{
    auto hash = key;
    auto slot = hash % hashTable.size();

    auto* node = hashTable[slot];

    while (node && (node->value.first != key))
        node = node->next;

    return node ? Iterator{&hashTable[slot], &hashTable[hashTable.size()], node} : end();
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::findNext(Iterator& it)
// Find the next value with the specified key
{
    assert(it != end());

    auto key = it.node->value.first;
    auto* node = it.node->next;

    while (node && (node->value.first != key))
        node = node->next;

    return node ? Iterator(it.bucket, it.end, node) : end();
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::begin()
// Return an iterator pointing to the first element
{
    return {&hashTable[0], &hashTable[hashTable.size()], hashTable[0]};
}
//---------------------------------------------------------------------------
MultiMap::Iterator MultiMap::end()
// Return an iterator pointing to the first element
{
    return {&hashTable[hashTable.size()], &hashTable[hashTable.size()], nullptr};
}
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------
