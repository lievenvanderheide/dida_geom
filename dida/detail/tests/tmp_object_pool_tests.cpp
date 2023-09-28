#include "dida/detail/tmp_object_pool.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <set>

namespace dida::detail
{

class TestObj
{
public:
  TestObj(std::set<TestObj*>& deallocated_objects, int tag) : deallocated_objects_(deallocated_objects), tag_(tag)
  {
  }

  ~TestObj()
  {
    deallocated_objects_.insert(this);
  }

  int tag() const
  {
    return tag_;
  }

private:
  std::set<TestObj*>& deallocated_objects_;

  int tag_;
};

TEST_CASE("TmpObjectPool")
{
  size_t num_elements = GENERATE(3, 4, 5, 15, 16, 17);

  std::set<TestObj*> allocated_objects, deallocated_objects;

  {
    TmpObjectPool<TestObj, 4> pool;

    for (size_t i = 0; i < num_elements; i++)
    {
      TestObj* obj = pool.alloc(deallocated_objects, i);
      CHECK(obj->tag() == i);
      allocated_objects.insert(obj);
    }

    CHECK(deallocated_objects.empty());
  }

  CHECK(allocated_objects == deallocated_objects);
}

} // namespace dida::detail