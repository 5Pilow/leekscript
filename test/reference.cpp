#include "Test.hpp"

void Test::test_references() {

	header("References");

	success("let a = 2 let b = a a = 4 b", "2");
	success("let a = 2 let b = @a a = 5 b", "5");
	success("let a = 2 let b = @a b = 6 a", "6");
	success("let a = 2 let b = @a let c = @b a = 7 c", "7");
	success("let a = 2 let b = @a let c = @b c = 8 a", "8");

	success("let a = [1, 3, 2] let b = a.sort() b.push(4) a", "[1, 2, 3]");
	success("let a = [1, 3, 2] let b = @a.sort() b.push(4) a", "[1, 2, 3, 4]");
/*
	success("let a = 'hello' let b = @a a = 'world' b", "'world'");
	success("let a = true let b = @a b = false b", "false");
	success("let a = true let b = @a b = false a", "false");

	// Type change from int to string
	//success("let a = 12 let b = @a b = 'salut' a", "'salut'");
	success("let a = [1, 2, 3] let b = @a b = 'salut' a", "'salut'");


	success("let v = 10 let a = [v] a[0]++ v", "10");
//	success("let v = 10 let a = [@v] a[0]++ v", "11");

 * */
}
