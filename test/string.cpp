#include "Test.hpp"

void Test::test_strings() {

	header("Strings");

	// General
	lex_err("'", ls::LexicalError::Type::UNTERMINATED_STRING);
	lex_err("\"", ls::LexicalError::Type::UNTERMINATED_STRING);
	lex_err("'hello world", ls::LexicalError::Type::UNTERMINATED_STRING);

//	success("'\\''", "'''");
	success("\"\\\"\"", "'\"'");
//	success("'aujourd\\'hui'", "'aujourd'hui'");
	success("\"aujourd\\\"hui\"", "'aujourd\"hui'");

	success("'salut ' + 'ça va ?'", "'salut ça va ?'");
	success("'salut' + 12", "'salut12'");
	success("'salut' + true", "'saluttrue'");
	success("'salut' + null", "'salutnull'");
	success("'salut' * 3", "'salutsalutsalut'");
	success("|'salut'|", "5");
	success("'abc' / '.'", "array['abc']");
	success("'ab.c' / '.'", "array['ab', 'c']");
	success("'.ab.c' / '.'", "array['', 'ab', 'c']");
	success("'abc.' / '.'", "array['abc', '']");
	success("'.aaaaa.bbbb.ccc.dd.e.' / '.'", "array['', 'aaaaa', 'bbbb', 'ccc', 'dd', 'e', '']");
	success("~'bonjour'", "'ruojnob'");
	success("'bonjour'[3]", "'j'");
	sem_err("'bonjour'['hello']", ls::SemanticException::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, "<key 1>");
	success("~('salut' + ' ca va ?')", "'? av ac tulas'");
	success("'bonjour'[2:5]", "'njou'");
	sem_err("'bonjour'['a':5]", ls::SemanticException::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 1>");
	sem_err("'bonjour'[2:'b']", ls::SemanticException::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 2>");
	sem_err("'bonjour'['a':'b']", ls::SemanticException::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, "<key 1>");
	success("'salut' * (1 + 2)", "'salutsalutsalut'");
	success("('salut' * 1) + 2", "'salut2'");
	success("('hello.world.how.are.you' / '.').size()", "5");

	// Unicode
	success("'韭'", "'韭'");
	success("'♫☯🐖👽'", "'♫☯🐖👽'");
	success("'a♫b☯c🐖d👽'", "'a♫b☯c🐖d👽'");
	success("var hello = '你好，世界' hello", "'你好，世界'");
	success("'♫☯🐖👽'[3]", "'👽'");
	success("'韭' + '♫'", "'韭♫'");
	success("|'♫👽'|", "2");
	success("'♫👽'.size()", "2");

	success("'☣🦆🧀𑚉𒒫𑓇𐏊'.size()", "7");
	success("'௵௵a௵௵' / 'a'", "array['௵௵', '௵௵']");
	success("'a☂a' / '☂'", "array['a', 'a']");
	success("~'∑∬∰∜∷⋙∳⌘⊛'", "'⊛⌘∳⋙∷∜∰∬∑'");
	success("'ↂↂ' × 3", "'ↂↂↂↂↂↂ'");
	success("'ḀḂḈḊḖḞḠḦḮḰḸḾṊṎṖ'[5:9]", "'ḞḠḦḮḰ'");

	// String standard library
	header("String standard library");
	success("String", "<class String>");
	success("String()", "''");
	success("new String", "''");
	success("new String()", "''");
	success("new String('salut')", "'salut'");
	success("String()", "''");
	success("String('yo')", "'yo'");
	success("String.size('salut')", "5");
	success("String.toUpper('salut')", "'SALUT'");
	success("String.length('salut')", "5");
	success("String.reverse('salut')", "'tulas'");
	success("String.replace('bonjour à tous', 'o', '_')", "'b_nj_ur à t_us'");
	success("String.map('salut', x -> '(' + x + ')')", "'(s)(a)(l)(u)(t)'");
	success("'salut'.map(char -> char + '.')", "'s.a.l.u.t.'");
	success("'♫☯🐖👽韭'.map(u -> u + ' ')", "'♫ ☯ 🐖 👽 韭 '");
	success("String.split('bonjour ça va', ' ')", "array['bonjour', 'ça', 'va']");
	success("String.split('bonjour_*_ça_*_va', '_*_')", "array['bonjour', 'ça', 'va']");
	success("String.split('salut', '')", "array['s', 'a', 'l', 'u', 't']");
	success("String.startsWith('salut ça va', 'salut')", "true");
	success("String.toArray('salut')", "array['s', 'a', 'l', 'u', 't']");
	success("String.charAt('salut', 1)", "'a'");
	success("'salut'.substring(3, 4)", "'ut'");

	// Integer conversions
	success("'A'.code()", "65");
	success("'ABC'.code(2)", "67");
	success("'©'.code()", "169");
	success("'é'.code()", "233");
	success("'♫'.code()", "9835");
	success("'🐨'.code()", "128040");
	success("String.code('🐨')", "128040");
	success("String.code('ABC', 2)", "67");
	success("(x -> x)(65).char()", "'A'");
	success("[128040][0].char()", "'🐨'");
	success("'hello'.map(x -> if x == ' ' then ' ' else x.code() - 'a'.code() + 1 + ' ' end)", "'8 5 12 12 15 '");

	success("String.number('1234567')", "1234567");
	success("String.number('1469215478186644')", "1469215478186644");
	success("'1234567'.number()", "1234567");
	success("'1469215478186644'.number()", "1469215478186644");
}
