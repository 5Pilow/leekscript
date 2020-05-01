#include "Test.hpp"

void Test::test_files_euler() {

	section("Project Euler");
	file("test/code/euler/pe001.leek").equals("233168");
	file("test/code/euler/pe002.leek").equals("4613732");
	file("test/code/euler/pe003.leek").equals("6857");
	file("test/code/euler/pe004.leek").equals("906609");
	file("test/code/euler/pe005.leek").equals("232792560");
	file("test/code/euler/pe006.leek").equals("25164150");
	file("test/code/euler/pe007.leek").equals("104743");
	file("test/code/euler/pe008.leek").equals("23514624000");
	file("test/code/euler/pe009.leek").equals("31875000");
	file("test/code/euler/pe010.leek").equals("142913828922");
	file("test/code/euler/pe011.leek").equals("70600674");
	file("test/code/euler/pe012.leek").equals("76576500");
	file("test/code/euler/pe013.leek").equals("5537376230");
	DISABLED_file("test/code/euler/pe014.leek").equals("837799");
	DISABLED_file("test/code/euler/pe015.leek").equals("137846528820");
	file("test/code/euler/pe016.leek").equals("1366");
	file("test/code/euler/pe017.leek").equals("21124");
	// TODO leaks
	DISABLED_file("test/code/euler/pe018.leek").equals("1074");
	file("test/code/euler/pe019.leek").equals("171");
	file("test/code/euler/pe020.leek").equals("648");
	file("test/code/euler/pe021.leek").equals("31626");
	DISABLED_file("test/code/euler/pe022.leek").equals("871198282");
	DISABLED_file("test/code/euler/pe023.leek").equals("4179871");
	file("test/code/euler/pe024.leek").equals("2783915460");
	file("test/code/euler/pe025.leek").equals("4782");
	// TODO needs mpf numbers
	// file("test/code/euler/pe026.leek").equals("");
	file("test/code/euler/pe027.leek").equals("-59231");
	file("test/code/euler/pe028.leek").equals("669171001");
	file("test/code/euler/pe062.leek").equals("127035954683");
	file("test/code/euler/pe063.leek").equals("49");
	file("test/code/euler/pe064.leek").equals("1322");
	DISABLED_file("test/code/euler/pe206.leek").equals("1389019170");
}