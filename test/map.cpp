#include "Test.hpp"

void Test::test_map() {

	header("Map");

	success("map[1 : 1 2 : 2]", "map[1 : 1 2 : 2]");
	success("map[1 : 1 2 : '2']", "map[1 : 1 2 : '2']");

	success("let x = map[1 : 1 1 : 2 1 : 3] x.size()", "1");
	success("let x = map[1 : 1 1 : 2 2 : '3'] x.size()", "2");

	success("let x = map[1 : 1] x.insert(2, 2)", "map[1 : 1 2 : 2]");
	success("let x = map['a' : 'a'] x.insert(2, 2)", "map[2 : 2 'a' : 'a']");
	success("let x = map[1 : 'a'] x.insert(2, 3)", "map[1 : 'a' 2 : 3]");
	success("let x = map['a' : 1] x.insert(2, 3)", "map[2 : 3 'a' : 1]");

	success("let x = map[1 : 1] x.clear()", "map[]");
	success("let x = map['a' : 'a'] x.clear()", "map[]");

	success("let x = map[1 : 1] x.erase(1)", "map[]");
	success("let x = map['a' : 'a'] x.erase('a')", "map[]");
	success("let x = map['a' : 'a'] x.erase('b')", "map['a' : 'a']");
	success("let x = map['a' : 1] x.erase(3.14)", "map['a' : 1]");

	success("map['a':'b'] == map[1:1]", "false");
	success("let x = map['a' : 'b'] let y = map[1 : 1] x.clear() == y.clear()", "true");
}
