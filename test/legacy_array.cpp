#include "Test.hpp"
#include "../src/vm/value/LSArray.hpp"
#include "../src/vm/value/LSLegacyArray.hpp"

void Test::test_legacy_array() {

    auto vm = new ls::VM { getEnv(), getEnv().std };

    auto empty = new ls::LSLegacyArray(vm);

	header("Legacy array");

    section("General");

    action("Create legacy array [12, 5, 3, 7, 999]");
    auto a1 = new ls::LSLegacyArray(vm);
    a1->push(new ls::LSNumber(12));
    a1->push(new ls::LSNumber(5));
    a1->push(new ls::LSNumber(3));
    a1->push(new ls::LSNumber(7));
    a1->push(new ls::LSNumber(999));

    test("print", a1->to_string(), "[12, 5, 3, 7, 999]");
    test("size()", a1->size(), 5);
    test("get(0)", a1->get(0)->to_string(), "12");
    test("get(4)", a1->get(4)->to_string(), "999");


    section("set");
    action("set('hello', 79)");
    a1->set(new ls::Key("hello"), new ls::LSNumber(79));
    test("print", a1->to_string(), "[0 => 12, 1 => 5, 2 => 3, 3 => 7, 4 => 999, 'hello' => 79]");

    section("contains");

    test("contains(12)", a1->contains(new ls::LSNumber(12)), true);
    test("contains(15)", a1->contains(new ls::LSNumber(15)), false);

    section("containsKey");

    test("containsKey(0)", a1->containsKey(new ls::Key(0)), true);
    test("containsKey(1)", a1->containsKey(new ls::Key(1)), true);
    test("containsKey(99)", a1->containsKey(new ls::Key(99)), false);
    test("containsKey('hello')", a1->containsKey(new ls::Key("hello")), true);

    section("remove");
    action("remove('hello')");
    a1->remove(new ls::Key("hello"));
    test("print", a1->to_string(), "[12, 5, 3, 7, 999]");

    section("sort");

    action("sort(ASC)");
    a1->sort(ls::LSLegacyArray::ASC);

    test("print", a1->to_string(), "[3, 5, 7, 12, 999]");

    action("sort(DESC)");
    a1->sort(ls::LSLegacyArray::DESC);

    test("print", a1->to_string(), "[999, 12, 7, 5, 3]");

    action("set('hello', 79)");
    a1->set(new ls::Key("hello"), new ls::LSNumber(79));

    test("print", a1->to_string(), "[0 => 999, 1 => 12, 2 => 7, 3 => 5, 4 => 3, 'hello' => 79]");
    test("size()", a1->size(), 6);
    test("get('hello')", a1->get("hello")->to_string(), "79");


    auto a2 = new ls::LSLegacyArray(vm);
    a2->set(new ls::Key("lama"), new ls::LSNumber(5));
    a2->set(new ls::Key("poireau"), new ls::LSNumber(42));
    a2->set(new ls::Key("zeta"), new ls::LSNumber(123));
    a2->set(new ls::Key("alpha"), new ls::LSNumber(1993));

    test("print", a2->to_string(), "['lama' => 5, 'poireau' => 42, 'zeta' => 123, 'alpha' => 1993]");

    a2->sort(ls::LSLegacyArray::ASC_K);

    test("print", a2->to_string(), "['alpha' => 1993, 'lama' => 5, 'poireau' => 42, 'zeta' => 123]");


    section("reverse");
    a2->reverse();
    test("print", a2->to_string(), "['zeta' => 123, 'poireau' => 42, 'lama' => 5, 'alpha' => 1993]");

    section("insert");
    a2->insert(2, ls::LSNumber::get(1111));
    test("print", a2->to_string(), "['zeta' => 123, 'poireau' => 42, 0 => 1111, 'lama' => 5, 'alpha' => 1993]");
    a2->insert(0, ls::LSNumber::get(2222));
    test("print", a2->to_string(), "[0 => 2222, 'zeta' => 123, 'poireau' => 42, 1 => 1111, 'lama' => 5, 'alpha' => 1993]");

    section("removeIndex");
    action("a2.removeIndex(2)");
    a2->removeIndex(2);
    action("a2.removeIndex(2)");
    a2->removeIndex(2);
    test("print", a2->to_string(), "[0 => 2222, 'zeta' => 123, 'lama' => 5, 'alpha' => 1993]");

    action("empty.removeIndex(0)");
    empty->removeIndex(0);
    test("empty.print()", empty->to_string(), "[]");

    const size_t S = 613;
    section("large legacy array");
    auto exe_start = std::chrono::high_resolution_clock::now();
    auto large = new ls::LSLegacyArray(vm);
    vm->enable_operations = false;
    for (size_t i = 0; i < S; ++i) {
        large->push(ls::LSNumber::get(12));
    }
    auto exe_end = std::chrono::high_resolution_clock::now();
    test("large.size", large->size(), S);
    auto execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
	std::cout << "time = " << (((double) execution_time / 1000) / 1000) << std::endl;

    section("large array");
    exe_start = std::chrono::high_resolution_clock::now();
    auto largea = new ls::LSArray<ls::LSValue*>();
    for (int i = 0; i < S; ++i) {
        largea->push_move(ls::LSNumber::get(12));
    }
    exe_end = std::chrono::high_resolution_clock::now();
    test("largea.size", largea->size(), S);
    execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
	std::cout << "time = " << (((double) execution_time / 1000) / 1000) << std::endl;

    section("large array int");
    exe_start = std::chrono::high_resolution_clock::now();
    auto largeai = new ls::LSArray<int>();
     for (int i = 0; i < S; ++i) {
        largeai->push_move(12);
    }
    exe_end = std::chrono::high_resolution_clock::now();
    test("largeai.size", largea->size(), S);
    execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
	std::cout << "time = " << (((double) execution_time / 1000) / 1000) << std::endl;
}